/*==============================================================================

   ポリゴン描画 [polygon.cpp]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#include <d3d11.h>
#include <DirectXMath.h>
#include "DirectXTex.h"
#include "direct3d.h"
#include "polygon.h"
#include "shader.h"
#include "debug_ostream.h"
#include "Input.h"
using namespace DirectX;

namespace Polygon2D
{
	static constexpr int NUM_VERTEX = 4; // 頂点数
	// 02:変数追加
	static constexpr int ANIM_X = 3;	//テクスチャのアニメーション数（横方向分割数） 
	static constexpr int ANIM_Y = 4;	//テクスチャのアニメーション数（縦方向分割数）

	int g_animFrameX = 0;			//アニメーションの現在のフレーム
	int g_animFrameY = 0;			//アニメーションの現在のフレーム
	int g_animCounter = 0;			//アニメーションの更新用カウンタ

	ID3D11Buffer* g_pVertexBuffer = nullptr; // 頂点バッファ
	ID3D11ShaderResourceView* g_pTexture = nullptr; // テクスチャ

	// 注意！初期化で外部から設定されるもの。Release不要。
	ID3D11Device* g_pDevice = nullptr;
	ID3D11DeviceContext* g_pContext = nullptr;

	XMFLOAT2 g_position = { 100.0f,100.0f }; // 02:移動用の変数
	XMFLOAT2 g_size = { 200.0f,200.0f };    // 02:拡大縮小用の変数
	float g_angle = 0.0f;                      // 02:回転用の変数

	// 頂点構造体
	struct Vertex
	{
		XMFLOAT3 position; // 頂点座標
		XMFLOAT4 color;    // 頂点カラー
		XMFLOAT2 texcoord; // テクスチャ座標
	};


	void Polygon_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	{
		// デバイスとデバイスコンテキストのチェック
		if (!pDevice || !pContext) {
			hal::dout << "Polygon_Initialize() : 与えられたデバイスかコンテキストが不正です" << std::endl;
			return;
		}
		// デバイスとデバイスコンテキストの保存
		g_pDevice = pDevice;
		g_pContext = pContext;
		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(Vertex) * NUM_VERTEX;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		g_pDevice->CreateBuffer(&bd, NULL, &g_pVertexBuffer);

		// テクスチャ読み込み
		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICFile(L"pipo-airship01.png", WIC_FLAGS_NONE, &metadata, image);
		HRESULT hr = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &g_pTexture);
		if (FAILED(hr)) {
			MessageBox(nullptr, "テクスチャの読み込みに失敗しました", "エラー", MB_OK | MB_ICONERROR);
		}
		// 02:アニメーション用の変数初期化
		g_animFrameX = 0;
		g_animFrameY = 1;

	}

	void Polygon_Finalize(void)
	{
		SAFE_RELEASE(g_pTexture);
		SAFE_RELEASE(g_pVertexBuffer);
	}

	void Polygon_Update(void)
	{
		g_animCounter++; // アニメーション更新用のカウンタをインクリメント
		if (g_animCounter >= 10) { // カウンタが10以上になったらフレームを更新
			g_animCounter = 0; // カウンタをリセット
			// 02:アニメーションのフレーム更新
			g_animFrameX = (g_animFrameX + 1) % ANIM_X;
		}
		if (GetKeyPress(VK_RIGHT))
		{
			g_animFrameY = 2;
			g_position.x += 5.0f; // 右キーで右に移動
		}
		if (GetKeyPress(VK_LEFT))
		{
			g_animFrameY = 1;
			g_position.x -= 5.0f; // 左キーで左に移動
		}
		if (GetKeyPress(VK_UP))
		{
			g_animFrameY = 3;
			g_position.y -= 5.0f; // 上キーで上に移動
		}
		if (GetKeyPress(VK_DOWN))
		{
			g_animFrameY = 0;
			g_position.y += 5.0f; // 下キーで下に移動
		}
		if (GetKeyPress(VK_Z))
		{
			g_size.x += 5.0f; // Zキーで拡大
			g_size.y += 5.0f;
		}
		if (GetKeyPress(VK_X))
		{
			g_size.x -= 5.0f; // Xキーで縮小
			g_size.y -= 5.0f;
		}
		if (GetKeyPress(VK_A))
		{
			g_angle += XMConvertToRadians(1.0f); // Aキーで反時計回りに回転
			if (g_angle > XM_2PI) g_angle -= XM_2PI; // 角度が360度を超えたらリセット
		}

		// 02:画面外に出ないように制限
		if (g_position.x < 0.0f) g_position.x = 0.0f;
		if (g_position.y < 0.0f) g_position.y = 0.0f;
		const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
		const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();
		if (g_position.x > SCREEN_WIDTH) g_position.x = SCREEN_WIDTH;
		if (g_position.y > SCREEN_HEIGHT) g_position.y = SCREEN_HEIGHT;
	}

	void Polygon_Draw(void)
	{
		// 頂点バッファをロックする
		D3D11_MAPPED_SUBRESOURCE msr;
		g_pContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		// 頂点バッファへの仮想ポインタを取得
		Vertex* v = (Vertex*)msr.pData;

		// 頂点情報を書き込み
		const float SCREEN_WIDTH = (float)Direct3D_GetBackBufferWidth();
		const float SCREEN_HEIGHT = (float)Direct3D_GetBackBufferHeight();
		// ローカル座標で四隅の位置を定義
		XMFLOAT2 local[4] =
		{
			{ -g_size.x / 2.0f, -g_size.y / 2.0f }, // 左上
			{  g_size.x / 2.0f, -g_size.y / 2.0f }, // 右上
			{ -g_size.x / 2.0f,  g_size.y / 2.0f }, // 左下
			{  g_size.x / 2.0f,  g_size.y / 2.0f }  // 右下
		};

		for (int i = 0; i < NUM_VERTEX; ++i)
		{
			// ローカル座標を回転させる
			float x = local[i].x;
			float y = local[i].y;

			// 2D回転行列
			float rx = x * cos(g_angle) - y * sin(g_angle);
			float ry = x * sin(g_angle) + y * cos(g_angle);

			v[i].position =
			{
				g_position.x + rx,
				g_position.y + ry,
				0.0f
			};
			v[i].color = { 1.0f, 1.0f, 1.0f, 1.0f };
		}

		v[0].texcoord =
		{ g_animFrameX / (float)ANIM_X, g_animFrameY / (float)ANIM_Y };
		v[1].texcoord =
		{ (g_animFrameX + 1) / (float)ANIM_X, g_animFrameY / (float)ANIM_Y };
		v[2].texcoord =
		{ g_animFrameX / (float)ANIM_X, (g_animFrameY + 1) / (float)ANIM_Y };
		v[3].texcoord =
		{ (g_animFrameX + 1) / (float)ANIM_X, (g_animFrameY + 1) / (float)ANIM_Y };

		// 頂点バッファのロックを解除
		g_pContext->Unmap(g_pVertexBuffer, 0);

		// 頂点バッファを描画パイプラインに設定
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

		// プリミティブトポロジ設定
		g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// ピクセルシェーダーの定数バッファにカラー情報を転送
		Shader_SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 黄色

		// シェーダーを描画パイプラインに設定
		Shader_Begin();

		// 頂点シェーダーに変換行列を設定
		Shader_SetMatrix(XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));

		// テクスチャ設定
		g_pContext->PSSetShaderResources(0, 1, &g_pTexture);

		// ポリゴン描画命令発行
		g_pContext->Draw(NUM_VERTEX, 0);
	}

}
