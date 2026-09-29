#pragma once
#include "GameObject.h"
#include <DirectXMath.h>

class Sprite : public CGameObject
{
private:
	// テクスチャリソース
	ID3D11ShaderResourceView* m_pTexture = nullptr;

	// 頂点関連
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pConstantBuffer = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

	// シェーダー
	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;

	// サンプラー状態
	ID3D11SamplerState* m_pSamplerState = nullptr;

	// ブレンドステート
	ID3D11BlendState* m_pBlendState = nullptr;
protected:
	// テクスチャサイズ
	float m_width = 0.0f;
	float m_height = 0.0f;

	// α値
	float m_alpha = 1.0f;
	XMFLOAT2 m_minTex = { 0.0f, 0.0f };
	XMFLOAT2 m_maxTex = { 1.0f, 1.0f };
public:
	Sprite();
	virtual ~Sprite();

	virtual void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	// テクスチャ読み込み（ファイルパス）
	HRESULT LoadTexture(const wchar_t* filePath);
	HRESULT LoadTexture(const char* filePath);

	// テクスチャ読み込み（メモリ）
	HRESULT LoadTextureFromMemory(const uint8_t* pData, size_t dataSize);

	// サイズ設定
	void SetSize(float width, float height);
	float GetWidth() const { return m_width; }
	float GetHeight() const { return m_height; }
	XMFLOAT2 GetMinTex() const { return m_minTex; }
	XMFLOAT2 GetMaxTex() const { return m_maxTex; }
	void SetMinTex(XMFLOAT2 minTex) { m_minTex = minTex; }
	void SetMaxTex(XMFLOAT2 maxTex) { m_maxTex = maxTex; }
	// α値設定
	void SetAlpha(float alpha) { m_alpha = alpha; }
	float GetAlpha() const { return m_alpha; }
};

