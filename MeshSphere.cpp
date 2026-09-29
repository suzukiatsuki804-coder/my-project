#include "MeshSphere.h"
#include "Camera.h"
#include "Light.h"
#include <d3dcompiler.h>
#include <vector>
#include <cmath>

#pragma comment(lib, "d3dcompiler.lib")

// 頂点構造体
struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
};

void MeshSphere::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	// 球の分割数
	const UINT segments = 32;		// 水平分割数
	const UINT rings = 16;			// 垂直分割数
	const float radius = 1.0f;		// 半径

	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	// 頂点データの生成
	for (UINT i = 0; i <= rings; ++i)
	{
		float phi = XM_PI * i / rings;
		float sinPhi = sinf(phi);
		float cosPhi = cosf(phi);

		for (UINT j = 0; j <= segments; ++j)
		{
			float theta = 2.0f * XM_PI * j / segments;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);

			// 頂点座標を計算
			XMFLOAT3 position;
			position.x = radius * sinPhi * cosTheta;
			position.y = radius * cosPhi;
			position.z = radius * sinPhi * sinTheta;

			// 法線は球の中心からの方向（正規化済み）
			XMFLOAT3 normal;
			normal.x = sinPhi * cosTheta;
			normal.y = cosPhi;
			normal.z = sinPhi * sinTheta;

			vertices.push_back({ position, normal });
		}
	}

	// インデックスデータの生成
	for (UINT i = 0; i < rings; ++i)
	{
		UINT ring1 = i * (segments + 1);
		UINT ring2 = (i + 1) * (segments + 1);

		for (UINT j = 0; j < segments; ++j)
		{
			UINT first = ring1 + j;
			UINT second = ring1 + j + 1;
			UINT third = ring2 + j;
			UINT fourth = ring2 + j + 1;

			// 最初の三角形
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(third);

			// 2番目の三角形
			indices.push_back(second);
			indices.push_back(fourth);  //４に
			indices.push_back(third);  //3に
		}
	}

	m_indexCount = static_cast<UINT>(indices.size());

	// 頂点バッファの作成
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices.data();

	m_pDevice->CreateBuffer(&vbDesc, &vbData, &m_pVertexBuffer);

	// インデックスバッファの作成
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(UINT));
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices.data();

	m_pDevice->CreateBuffer(&ibDesc, &ibData, &m_pIndexBuffer);

	// 定数バッファの作成（Transform用）
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = sizeof(CBTransform);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pConstantBuffer);

	// 定数バッファの作成（色用）
	cbDesc.ByteWidth = sizeof(XMFLOAT4);
	m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pColorBuffer);

	// シェーダのコンパイル
	ID3D10Blob* pVSBlob = nullptr;
	ID3D10Blob* pPSBlob = nullptr;
	ID3D10Blob* pError = nullptr;

	D3DCompileFromFile(L"shader_vertex_3d.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &pVSBlob, &pError);
	D3DCompileFromFile(L"shader_pixel_3d.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pPSBlob, &pError);

	// 頂点シェーダの作成
	m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);

	// ピクセルシェーダの作成
	m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);

	// 入力レイアウトの定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_pDevice->CreateInputLayout(layout, _countof(layout), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout);

	pVSBlob->Release();
	pPSBlob->Release();
}

void MeshSphere::Uninit()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
	}
	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = nullptr;
	}
	if (m_pColorBuffer)
	{
		m_pColorBuffer->Release();
		m_pColorBuffer = nullptr;
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
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}
}

void MeshSphere::Update()
{
}

void MeshSphere::Draw()
{
	// 世界座標の行列計算（スケール・回転・平行移動）
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	XMMATRIX rotation = XMMatrixRotationX(m_rotation.x) * XMMatrixRotationY(m_rotation.y) * XMMatrixRotationZ(m_rotation.z);
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMMATRIX world = scale * rotation * translation;

	// ビュー行列・プロジェクション行列の取得
	XMMATRIX view, proj;
	Camera::GetCameraMatrix(view, proj);

	// ワールド・ビュー・プロジェクション行列の計算
	XMMATRIX wvp = world * view * proj;

	// 定数バッファのデータを更新
	CBTransform cbData = {};
	XMStoreFloat4x4(&cbData.wvp, XMMatrixTranspose(wvp));
	XMStoreFloat4x4(&cbData.world, XMMatrixTranspose(world));

	// 定数バッファを更新（マップ・アンマップ）
	D3D11_MAPPED_SUBRESOURCE msr = {};
	m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, &cbData, sizeof(CBTransform));
	m_pContext->Unmap(m_pConstantBuffer, 0);

	// 色の定数バッファを更新
	m_pContext->Map(m_pColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, &m_color, sizeof(XMFLOAT4));
	m_pContext->Unmap(m_pColorBuffer, 0);

	// 定数バッファを頂点シェーダにセット
	m_pContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	// 定数バッファをピクセルシェーダにセット
	m_pContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext->PSSetConstantBuffers(2, 1, &m_pColorBuffer);

	// 頂点バッファをセット
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// インデックスバッファをセット
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 入力レイアウトをセット
	m_pContext->IASetInputLayout(m_pInputLayout);

	// プリミティブトポロジーをセット
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// シェーダをセット
	m_pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// インデックスで描画
	m_pContext->DrawIndexed(m_indexCount, 0, 0);
}