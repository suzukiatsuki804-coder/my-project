#pragma once
#include "direct3d.h"
#include <DirectXMath.h>
using namespace DirectX;
// 定数バッファ構造体
struct CBTransform
{
	XMFLOAT4X4 wvp;     // World*View*Projection
	XMFLOAT4X4 world;   // ワールド行列
};

class CGameObject
{
protected:
	// 描画デバイス
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	// Transform。位置、回転、拡縮
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_rotation = XMFLOAT3(0.0f,0.0f,0.0f);
	XMFLOAT3 m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT3 m_velocity; 
public:

	// 初期化、終了、更新、描画
	// =0とすると純粋仮想関数になる。派生側で必ず処理を書かないとエラーになる
	// わざわざこうする理由は、派生側で必要な処理を書かせて漏れを防ぐため
	virtual void Init(ID3D11Device* pDevice,ID3D11DeviceContext* pContext) = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	XMFLOAT3 GetForward();
	XMFLOAT3 GetPosition() { return m_position; }
	XMFLOAT3 GetRotation() { return m_rotation; }
	XMFLOAT3 GetScale() { return m_scale; }
	XMFLOAT3 GetVelocity() { return m_velocity; }  //5/27追加

	void SetPosition(XMFLOAT3 pos) { m_position = pos; }
	void SetRotation(XMFLOAT3 rot) { m_rotation = rot; }
	void SetScale(XMFLOAT3 scale) { m_scale = scale; }
	void SetColor(XMFLOAT4 color) { m_color = color; }	
	void SetVelocity(XMFLOAT3 vel) { m_velocity = vel; }  //5/27追加

};

