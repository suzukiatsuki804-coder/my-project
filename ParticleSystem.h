#pragma once
#include "direct3D.h"
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

// 粒
constexpr int MAX_PARTICLE = 1000;
// フレーム毎に何個粒を出すか
//1フレームの間隔が短いので見た感じの数はそこまで減らない(数字が小さいと逆三角の形に近い)
constexpr int PARTICLE_PER_FLAME = 100;  

struct Particle
{
	XMFLOAT3 pos;
	XMFLOAT3 vel;
	float life;
	float alpha;
};

// 定数バッファ
struct ConstantBuffer
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
	float padding[4];  // ← パディング追加
};

// 頂点構造体
struct ParticleVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

// パーティクルシステム
// 粒粒を合わせてエフェクト表現する
class ParticleSystem
{
private:
	std::vector<Particle> particles;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* constBuffer;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
	ID3D11ShaderResourceView* particleTexture;
	ID3D11BlendState* blendState;
	ID3D11BlendState* defaultBlendState;  // デフォルトブレンドステートを保存
	bool isLoop = false;
	XMFLOAT3 m_basePosition = {};
public:
	void Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Update();
	void Emit(bool loop = false);
	void Draw(const XMMATRIX& view, const XMMATRIX& proj);
	void Uninit();
	void SetParticleTexture(ID3D11ShaderResourceView* texture);
	void SetBasePosition(XMFLOAT3 pos);

};