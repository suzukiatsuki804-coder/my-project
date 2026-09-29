#pragma once
#include "GameObject.h"

enum class GroundType {
	Fairway,
	Rough,
	Bunker,
	Water,
	Goal,
	OB,
};




class MeshBox : public CGameObject
{
private:
	// 頂点バッファ
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	// インデックスバッファ
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	// 定数バッファ（Transform用）
	ID3D11Buffer* m_pConstantBuffer = nullptr;
	// 定数バッファ（色用）
	ID3D11Buffer* m_pColorBuffer = nullptr;
	// 頂点シェーダ
	ID3D11VertexShader* m_pVertexShader = nullptr;
	// ピクセルシェーダ
	ID3D11PixelShader* m_pPixelShader = nullptr;
	// 入力レイアウト
	ID3D11InputLayout* m_pInputLayout = nullptr;
	// インデックス数
	UINT m_indexCount = 0;

public:
	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Uninit() override;
	virtual void Update()override;
	virtual void Draw() override;
	GroundType type = GroundType::Fairway;  //地面とタイプを追加
};