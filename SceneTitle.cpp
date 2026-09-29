#include "SceneTitle.h"

void SceneTitle::Init()
{
	//タイトルスプライトのインスタンスを生成
	m_titleSprite = new Sprite();
	//タイトルスプライトの初期化
	m_titleSprite->Init(
		Direct3D_GetDevice(),
		Direct3D_GetDeviceContext());
	//テクスチャを読み込む
	m_titleSprite->LoadTexture("Texture\\Title.png");
	//スプライトのサイズを取得(全体画面表示)
	float screenWidth = static_cast<float>
		(Direct3D_GetBackBufferWidth());
	float screenHeight = static_cast<float>
		(Direct3D_GetBackBufferHeight());
	//サイズの設定
	m_titleSprite->SetSize(screenWidth, screenHeight);
	//位置を画面中央に(左上が0,0)
	m_titleSprite->SetPosition(XMFLOAT3(screenWidth / 2.0f, screenHeight / 2.0f, 0.0f));

	//タイトル題字のインスタンス生成
	m_golfDai = new Sprite();
	m_golfDai->Init(
		Direct3D_GetDevice(),
		Direct3D_GetDeviceContext());
	//テクスチャ読み込み
	m_golfDai->LoadTexture("Texture\\golf_logo.png");
	//サイズ設定
	m_golfDai->SetSize(500.0f, 400.0f);
	//位置(とても適当、調整する)
	m_golfDai->SetPosition(XMFLOAT3(screenWidth / 2.0f, screenHeight / 2.5f, 0.0f));

	//スプライトのインスタンスを生成
	m_pushStart = new TitlePushStart();
	//PushStartの初期化
	m_pushStart->Init(
		Direct3D_GetDevice(),
		Direct3D_GetDeviceContext());	
	//テクスチャを読み込む
	m_pushStart->LoadTexture("Texture\\push_start.png");
	//サイズの設定
	m_pushStart->SetSize(400.0f, 300.0f);
	//位置を画面中央の少し下に（左上が0.0）
	m_pushStart->SetPosition(XMFLOAT3(screenWidth / 2.0f,screenHeight / 2.0f + 250.0f, 0.0f));

	//自作ゴルフ画像
	m_golfclub = new TitleGolfClub();
	m_golfclub->Init(
		Direct3D_GetDevice(),
		Direct3D_GetDeviceContext());
	m_golfclub->LoadTexture("Texture\\golfimage.png");
	m_golfclub->SetSize(400.0f, 300.0f);
	m_golfclub->SetPosition(XMFLOAT3(screenWidth / 2.0f + 100.0f, screenHeight / 2.0f + 250.0f, 0.0f));
	
}

void SceneTitle::Uninit()
{
	m_titleSprite->Uninit();
	delete m_titleSprite;
	m_titleSprite = nullptr;

	m_golfDai->Uninit();
	delete m_golfDai;
	m_golfDai = nullptr;  //三行6.17追加 

	m_pushStart->Uninit();
	delete m_pushStart;
	m_pushStart = nullptr;

	m_golfclub->Uninit();
	delete m_golfclub;
	m_golfclub = nullptr;
}

void SceneTitle::Update()
{
	m_titleSprite->Update();
	m_golfDai->Update();
	m_golfclub->Update(); //pushstartの前
	m_pushStart->Update();

	
}

void SceneTitle::Draw()
{
	//先に書いたほうが後ろに描画される
	m_titleSprite->Draw();
	m_golfDai->Draw();
	m_golfclub->Draw();  //自分で追加
	m_pushStart->Draw();
}
