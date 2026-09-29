// Light.h
#pragma once
#include <DirectXMath.h>
#include "direct3d.h"
using namespace DirectX;
// 定数バッファ用
struct CBLight
{
	XMFLOAT3 lightDir;	// 光の方向ベクトル
	float padding1;

	XMFLOAT4 diffuse;	// 拡散光
	XMFLOAT4 ambient;	// 環境光
	XMFLOAT4 specular;	// 反射光
};

class CLight
{
private:
	XMFLOAT4 m_diffuse;		// 拡散光
	XMFLOAT4 m_ambient;		// 環境光
	XMFLOAT4 m_specular;	// 反射光
	XMFLOAT3 m_direction;	// 光の方向
	bool	m_bEnable;		// ライトの有効、無効
public:
	CLight();

	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Update();
	// ゲッター。変数を隠蔽したいけど外でアクセスするときに使う
	XMFLOAT4& GetDiffuse() { return m_diffuse; }
	XMFLOAT4& GetAmbient() { return m_ambient; }
	XMFLOAT4& GetSpecular() { return m_specular; }
	XMFLOAT3& GetDir();
	void SetEnable(bool bEnable = true) { m_bEnable = bEnable; }
	void SetDisable(bool bDisable = true) { m_bEnable = !bDisable; }
};
// ライト用のクラスの取得
CLight* GetLight();
