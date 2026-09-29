/*==============================================================================

   Direct3D初期化 [main.cpp]
														 Author : Youhei Sato
														 Date   : 2025/05/12
--------------------------------------------------------------------------------

==============================================================================*/
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <algorithm>
#include "direct3d.h"
#include "shader.h"
#include "polygon.h"
#include "Input.h"
#include "Camera.h"
#include "Light.h"
#include "main.h"  //一応追加
#include "SceneInGame.h"  //ここ追加
#include "SceneTitle.h"  //6/1追加
#include "TitlePushStart.h"
#include "audio.h"  //6/10追加


// 02:フレームレート制御用のヘッダ
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
const int TARGET_FPS = 60; // 目標フレームレート




/*------------------------------------------------------------------------------
	ウィンドウ情報
------------------------------------------------------------------------------*/
static constexpr char WINDOW_CLASS[] = "GameWindow"; // メインウィンドウクラス名
static constexpr char TITLE[] = "ポリゴン描画"; // タイトルバーのテキスト

SceneBase* g_scene = nullptr;  //現在のシーンを指すポインタ


/*------------------------------------------------------------------------------
    ウィンドウプロシージャ プロトタイプ宣言
------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


/*------------------------------------------------------------------------------
    メイン
------------------------------------------------------------------------------*/
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	/* ウィンドウクラスの登録 */
	WNDCLASSEX wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	RegisterClassEx(&wcex);


	// クライアント領域のサイズを持った矩形 (左からleft, top, right, bottom)
	RECT window_rect = { 0, 0, 1600, 900 };

	// ウィンドウのスタイル
	DWORD window_style = WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);

	// 指定したクライアント領域を確保するために新たな矩形座標を計算
	AdjustWindowRect(&window_rect, window_style, FALSE);
	
	// 新たなWindowの矩形座標から幅と高さを算出
	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;

	// プライマリモニターの画面解像度取得
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	// デスクトップの真ん中にウィンドウが生成されるように座標を計算
	// ※ただし万が一、デスクトップよりウィンドウが大きい場合は左上に表示
	int window_x = std::max((desktop_width - window_width) / 2, 0);
	int window_y = std::max((desktop_height - window_height) / 2, 0);

	/* メインウィンドウの作成 */
	HWND hWnd = CreateWindow(WINDOW_CLASS, TITLE, window_style,
		window_x, window_y, window_width, window_height, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	Direct3D_Initialize(hWnd); // Direct3Dの初期化
	Shader_Initialize(Direct3D_GetDevice(), Direct3D_GetDeviceContext()); // シェーダの初期化
	Camera::Initialize();
	// ライトの取得
	CLight* light = GetLight();
	//音源の初期化準備 6/10追加
	Audio::InitMaster();

	//作ったシーンを生成する 6/1変更
	g_scene = new SceneTitle();
	
	//追加シーンの初期化
	g_scene->Init();
	
	// ライトの初期化
	light->Init(Direct3D_GetDevice(),Direct3D_GetDeviceContext());

	/* メッセージ & ゲームループ */
	MSG msg;

	//02:フレームカウント初期化
	DWORD dwExecLastTime = 0;
	DWORD dwCurrentTime = 0;
	timeBeginPeriod(1); // タイマーの精度を1msに設定
	dwExecLastTime = timeGetTime();
	dwCurrentTime = 0;

	do {

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // ウィンドウメッセージが来ていたら
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else { 
			// ゲームの処理
			dwCurrentTime = timeGetTime();
			if (dwCurrentTime - dwExecLastTime >=1.0f/(float)TARGET_FPS)
			{
				dwExecLastTime = dwCurrentTime;
				light->Update();
				UpdateInput(); // 入力の更新
				Camera::Update();
				g_scene->Update();  // 5/27修正
				
				Direct3D_Clear(); // バックバッファのクリア
				
				Camera::Set3DCamera(); // カメラ行列の設定
				g_scene->Draw();  //5/27修正
				
				Direct3D_Present();
			}
		}

	} while (msg.message != WM_QUIT);
	//5/27修正 終了セット
	g_scene->Uninit();
	delete g_scene;
	Shader_Finalize(); // シェーダの終了処理
	//音の破棄処理 6/10追加
	Audio::UninitMaster();

	Direct3D_Finalize(); // Direct3Dの終了処理


	return (int)msg.wParam;
}


/*------------------------------------------------------------------------------
	ウィンドウプロシージャ
------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			SendMessage(hWnd, WM_CLOSE, 0, 0); // WM_CLOSEメッセージの送信
		}
		break;

	case WM_CLOSE: // ウィンドウを閉じるメッセージ
		if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd); // 指定のウィンドウにWM_DESTROYメッセージを送る
		}
		break; // DefWindowProc関数にメッセージを流さず終了することによって何もなかったことにする
	case WM_MOUSEMOVE:
		
		break;
	case WM_DESTROY: // ウィンドウの破棄メッセージ
		PostQuitMessage(0); // WM_QUITメッセージの送信
		break;

	default:
		// 通常のメッセージ処理はこの関数に任せる
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
