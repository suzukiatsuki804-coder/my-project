#pragma once
#include "GameObject.h"
#include "Sprite.h"

class Gauge : public CGameObject
{
private:
	Sprite* m_sprite = nullptr;  //ゲージのスプライト
	Sprite* m_backgroundSprite = nullptr;  //ゲージの背景
	float m_gaugeValue = 0.0f;  //ゲージの値(0.0f～0.1f)
	//フルサイズと基準位置を保持して
	//(サイズ/位置)調整を使う
	float m_fullWidth = 0.0f;
	float m_fullHeight = 0.0f;
	//基準位置（フレームの中心）
	//スプライトは左寄せで配置するため
	//ここをずらしてゲージ本体を配置する
	DirectX::XMFLOAT3 m_basePosition = { 0.0f, 0.0f, 0.0f };
public:
	void SetGaugeValue(float value)
	{
		m_gaugeValue = value;
		if (m_gaugeValue < 0.0f) m_gaugeValue = 0.0f;
		if (m_gaugeValue > 1.0f) m_gaugeValue = 1.0f;
	}
	float GetGaugeValue() const { return m_gaugeValue; }
	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void SetGaugePosition(DirectX::XMFLOAT3 position);
	void SetGaugeSize(float width, float height);
};

