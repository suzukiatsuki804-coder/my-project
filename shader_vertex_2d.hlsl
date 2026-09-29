// 定数バッファ（こっちの書き方もDX11向けにするのが正しい）
cbuffer ConstantBuffer : register(b0)
{
    float4x4 mtx;
};

struct VS_INPUT
{
    float4 posL : POSITION; // ← 修正
    float4 color : COLOR; // ← 修正
    float2 uv : TEXCOORD; // ← 修正
};

struct VS_OUTPUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

// 頂点シェーダ
VS_OUTPUT main(VS_INPUT vs_in)
{
    VS_OUTPUT vs_out;

    vs_out.posH = mul(vs_in.posL, mtx);
    vs_out.color = vs_in.color;
    vs_out.uv = vs_in.uv;

    return vs_out;
}