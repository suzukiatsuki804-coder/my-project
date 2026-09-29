// light.cpp
#include <Windows.h> 
#include "Light.h"
#include "input.h"
#include "direct3d.h"
using namespace DirectX;
// マクロ定義
#define LIGHT0_DIRECTION	XMFLOAT3(1.0f, -1.0f, 0.0f)
#define LIGHT0_DIFFUSE		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_AMBIENT		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f)
#define LIGHT0_SPECULAR		XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)

// グローバル変数
static CLight g_light;			// 光
ID3D11Device* g_pDevice;		// GPUに色々送る時に使うデバイス情報
ID3D11DeviceContext* g_pContext;// 描画時の設定情報
ID3D11Buffer* g_cbLight;		// 定数バッファ情報

// コンストラクタ
CLight::CLight()
{
}
// 初期化
void CLight::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	g_pDevice = pDevice;
	g_pContext = pContext;
	// 方向の初期値設定
	XMFLOAT3 vDir = LIGHT0_DIRECTION;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vDir)));
	// 光のあたり方の設定（Diffuseが影響大きい）
	m_diffuse = LIGHT0_DIFFUSE;
	m_ambient = LIGHT0_AMBIENT;
	m_specular = LIGHT0_SPECULAR;
	m_bEnable = true;

	// 定数バッファの設定
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBLight);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	g_pDevice->CreateBuffer(&bd, nullptr, &g_cbLight);
}

void CLight::Update()
{	
	
	XMFLOAT3 vDir = LIGHT0_DIRECTION;
	//static float rad;
	//rad += 0.01f;
	//vDir.x = sinf(rad);

	XMStoreFloat3(&m_direction,
		XMVector3Normalize(XMLoadFloat3(&vDir)));
	if (GetKeyPress(VK_R))
	{	// Rを押している間ライトを赤くする
		m_diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		// 放しているので白
		m_diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	// 定数バッファに入れるライトの設定
	CBLight cb{};

	CLight* light = GetLight();
	cb.lightDir = light->GetDir();
	cb.diffuse = light->GetDiffuse();
	cb.ambient = light->GetAmbient();
	cb.specular = light->GetSpecular();
	// GPU側にライトの情報を送りつける
	g_pContext->UpdateSubresource(
		g_cbLight,
		0, nullptr,
		&cb,
		0, 0
	);
	// シェーダーにも送りつける
	g_pContext->VSSetConstantBuffers(1, 1, &g_cbLight);
	g_pContext->PSSetConstantBuffers(1, 1, &g_cbLight);
}

// 光源方向取得
XMFLOAT3& CLight::GetDir()
{
	if (m_bEnable) return m_direction;
	static XMFLOAT3 off(0.0f, 0.0f, 0.0f);
	return off;
}

CLight * GetLight()
{
	return &g_light;
}
