#define _CRT_SECURE_NO_WARNINGS
#include "Sprite.h"
#include "direct3d.h"
#include "Texture.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <memory.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// 頂点構造体
struct VERTEX_2D
{
	DirectX::XMFLOAT4 pos;		// ローカル座標
	DirectX::XMFLOAT4 color;	// 色
	DirectX::XMFLOAT2 uv;		// テクスチャ座標
};

// 定数バッファ構造体
struct CB_MATRIX
{
	DirectX::XMFLOAT4X4 mtx;	// 4x4 行列
};

Sprite::Sprite()
{
}

Sprite::~Sprite()
{
	Uninit();
}

void Sprite::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	// デフォルトサイズ設定
	m_width = 100.0f;
	m_height = 100.0f;

	// 頂点データ作成
	VERTEX_2D vertices[] = {
		{ DirectX::XMFLOAT4(-0.5f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
		{ DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
		{ DirectX::XMFLOAT4(-0.5f, -0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
		{ DirectX::XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }
	};

	// 頂点バッファ作成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = vertices;

	pDevice->CreateBuffer(&bd, &srd, &m_pVertexBuffer);

	// 定数バッファ作成
	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(CB_MATRIX);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	pDevice->CreateBuffer(&cbd, nullptr, &m_pConstantBuffer);

	// 入力レイアウト作成
	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// シェーダーコンパイル（頂点シェーダー）
	ID3DBlob* pBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
		L"shader_vertex_2d.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&pBlob,
		&pErrorBlob
	);

	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		return;
	}

	// 頂点シェーダー作成
	pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pVertexShader);

	// 入力レイアウト作成
	pDevice->CreateInputLayout(ied, 3, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pInputLayout);
	pBlob->Release();

	// ピクセルシェーダーコンパイル
	pErrorBlob = nullptr;
	hr = D3DCompileFromFile(
		L"shader_pixel_2d.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&pBlob,
		&pErrorBlob
	);

	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		return;
	}

	// ピクセルシェーダー作成
	pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPixelShader);
	pBlob->Release();

	// サンプラー状態作成
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	pDevice->CreateSamplerState(&sd, &m_pSamplerState);

	// ブレンドステート作成
	D3D11_BLEND_DESC bd_blend = {};
	bd_blend.AlphaToCoverageEnable = FALSE;
	bd_blend.IndependentBlendEnable = FALSE;
	bd_blend.RenderTarget[0].BlendEnable = TRUE;
	bd_blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd_blend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd_blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd_blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd_blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd_blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd_blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pDevice->CreateBlendState(&bd_blend, &m_pBlendState);
}

void Sprite::Uninit()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = nullptr;
	}
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = nullptr;
	}
	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
		m_pSamplerState = nullptr;
	}
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}
	if (m_pVertexShader)
	{
		m_pVertexShader->Release();
		m_pVertexShader = nullptr;
	}
	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
		m_pPixelShader = nullptr;
	}
	if (m_pBlendState)
	{
		m_pBlendState->Release();
		m_pBlendState = nullptr;
	}
}

void Sprite::Update()
{
}

void Sprite::Draw()
{
	if (!m_pTexture || !m_pVertexBuffer) return;

	// スクリーン解像度を取得
	float screenWidth = static_cast<float>(Direct3D_GetBackBufferWidth());
	float screenHeight = static_cast<float>(Direct3D_GetBackBufferHeight());

	// 頂点バッファをロック
	D3D11_MAPPED_SUBRESOURCE msr;
	m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_2D* vertices = static_cast<VERTEX_2D*>(msr.pData);

	// スケール後のサイズを計算
	float scaledWidth = m_width * m_scale.x;
	float scaledHeight = m_height * m_scale.y;

	// ローカル座標で四隅の位置を定義
	DirectX::XMFLOAT2 localPos[4] =
	{
		{ -scaledWidth / 2.0f, -scaledHeight / 2.0f }, // 左上
		{  scaledWidth / 2.0f, -scaledHeight / 2.0f }, // 右上
		{ -scaledWidth / 2.0f,  scaledHeight / 2.0f }, // 左下
		{  scaledWidth / 2.0f,  scaledHeight / 2.0f }  // 右下
	};

	// 回転角を取得
	float angle = m_rotation.z;

	// 各頂点を処理
	for (int i = 0; i < 4; ++i)
	{
		// ローカル座標を回転させる（2D回転行列）
		float x = localPos[i].x;
		float y = localPos[i].y;

		float rotX = x * cos(angle) - y * sin(angle);
		float rotY = x * sin(angle) + y * cos(angle);

		// スクリーン座標に変換（並行投影座標系）
		vertices[i].pos = DirectX::XMFLOAT4(
			m_position.x + rotX,
			m_position.y + rotY,
			0.0f,
			1.0f
		);

		// α値を頂点カラーに反映
		vertices[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, m_alpha);
	}

	// テクスチャ座標を設定
	vertices[0].uv = DirectX::XMFLOAT2(m_minTex.x, m_minTex.y);
	vertices[1].uv = DirectX::XMFLOAT2(m_maxTex.x, m_minTex.y);
	vertices[2].uv = DirectX::XMFLOAT2(m_minTex.x, m_maxTex.y);
	vertices[3].uv = DirectX::XMFLOAT2(m_maxTex.x, m_maxTex.y);

	// 頂点バッファのロック解除
	m_pContext->Unmap(m_pVertexBuffer, 0);

	// 並行投影行列を設定
	DirectX::XMMATRIX orthographicMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, screenWidth, screenHeight, 0.0f, 0.0f, 1.0f
	);

	CB_MATRIX cb;
	DirectX::XMStoreFloat4x4(&cb.mtx, DirectX::XMMatrixTranspose(orthographicMatrix));
	m_pContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	// シェーダー設定
	m_pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	// 入力レイアウト設定
	m_pContext->IASetInputLayout(m_pInputLayout);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// テクスチャ設定
	m_pContext->PSSetShaderResources(0, 1, &m_pTexture);
	m_pContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// プリミティブトポロジー設定
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ブレンドステートを設定
	float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pContext->OMSetBlendState(m_pBlendState, blend_factor, 0xffffffff);

	// 描画
	m_pContext->Draw(4, 0);

	// Draw の描画直前に追加（テスト用）
	// 現状の深度ステートをバックアップして無効化
	ID3D11DepthStencilState* prevDSS = nullptr;
	UINT prevStencilRef = 0;
	m_pContext->OMGetDepthStencilState(&prevDSS, &prevStencilRef);

	// 深度テスト無効化ステートを一時的に作成してセット（テスト用）
	D3D11_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthEnable = FALSE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_ALWAYS;
	ID3D11DepthStencilState* pDSSDisable = nullptr;
	m_pDevice->CreateDepthStencilState(&dsd, &pDSSDisable);
	m_pContext->OMSetDepthStencilState(pDSSDisable, 0);

	// 描画
	m_pContext->Draw(4, 0);

	// 復元
	m_pContext->OMSetDepthStencilState(prevDSS, prevStencilRef);
	if (pDSSDisable) pDSSDisable->Release();
	if (prevDSS) prevDSS->Release();
}

HRESULT Sprite::LoadTexture(const wchar_t* filePath)
{
	DirectX::TexMetadata metadata;
	HRESULT hr = CreateTextureFromFile(m_pDevice, filePath, &m_pTexture, &metadata);
	if (SUCCEEDED(hr))
	{
		m_width = static_cast<float>(metadata.width);
		m_height = static_cast<float>(metadata.height);
	}
	return hr;
}

HRESULT Sprite::LoadTexture(const char* filePath)
{
	WCHAR wszTexFName[_MAX_PATH];
	int nLen = MultiByteToWideChar(CP_ACP, 0, filePath, lstrlenA(filePath), wszTexFName, _countof(wszTexFName));
	if (nLen <= 0) return E_FAIL;
	wszTexFName[nLen] = L'\0';
	return LoadTexture(wszTexFName);
}

HRESULT Sprite::LoadTextureFromMemory(const uint8_t* pData, size_t dataSize)
{
	DirectX::TexMetadata metadata;
	HRESULT hr = CreateTextureFromMemory(m_pDevice, pData, dataSize, &m_pTexture, &metadata);
	if (SUCCEEDED(hr))
	{
		m_width = static_cast<float>(metadata.width);
		m_height = static_cast<float>(metadata.height);
	}
	return hr;
}

void Sprite::SetSize(float width, float height)
{
	m_width = width;
	m_height = height;
}
