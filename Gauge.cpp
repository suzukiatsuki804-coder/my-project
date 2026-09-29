#include "Gauge.h"
using namespace DirectX;

void Gauge::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_sprite = new Sprite();
	m_sprite->Init(pDevice, pContext);
	m_backgroundSprite = new Sprite();
	m_backgroundSprite->Init(pDevice, pContext);
	m_sprite->LoadTexture("Texture\\HyperGageBar.png");
	m_backgroundSprite->LoadTexture("Texture\\HyperGageFrame.png");
	m_gaugeValue = 0.0f;
	//初期のフルサイズは規定サイズに合わせておく、
	// （必要ならSetGaugeSizeで上書き）
	m_fullWidth = m_sprite->GetWidth();
	m_fullHeight = m_sprite->GetHeight();
}

void Gauge::Uninit()
{
	//ここの処理はいつもと大体同じ
	m_sprite->Uninit();
	delete m_sprite;
	m_sprite = nullptr;
	m_backgroundSprite->Uninit();
	delete m_backgroundSprite;
	m_backgroundSprite = nullptr;
}

void Gauge::Update()
{
	//現在の表示幅(フル幅に対する比率)
	float currentWidth = m_fullWidth * m_gaugeValue;
	float currentHeight = m_fullHeight;
	m_sprite->SetSize(currentWidth, currentHeight);
	//左端を固定して縮小するために中心位置をシフトする(左端から延びるゲージを作る)
	//フルサイズ時の左端 = basePosition.x + 4.0f -(m_fullWidth / 2)
	//新中心　＝　左端 + currentWidth / 2
	float newCenterX = m_basePosition.x + 4.0f + (currentWidth - m_fullWidth) * 0.5f;
	XMFLOAT3 newPos = { newCenterX, m_basePosition.y, m_basePosition.z };
	m_sprite->SetPosition(newPos);

}

void Gauge::Draw()
{
	//ゲージ本体を描画
	m_sprite->Draw();
	//フレーム本体を描画
	m_backgroundSprite->Draw();
}

void Gauge::SetGaugePosition(DirectX::XMFLOAT3 position)
{
	//基準位置を表示(背景の中心)
	m_basePosition = position;

	//スプライトは左寄せのためにオフセットしていたので。初期配置は同じ方式を維持
	//4.0fはいい感じの調整値
	auto offset = XMFLOAT3(position.x + 4.0f, position.y, position.z);
	m_sprite->SetPosition(offset);
	m_backgroundSprite->SetPosition(position);
}

void Gauge::SetGaugeSize(float width, float height)
{
	//フレームサイズを受け取りゲージ本体のフルサイズを記憶する
	m_fullWidth = width - 25.0f;
	m_fullHeight = height - 30.0f;
	m_sprite->SetSize(m_fullWidth, m_fullHeight);
	m_backgroundSprite->SetSize(width, height);
}
