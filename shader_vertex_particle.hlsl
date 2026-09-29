cbuffer ConstantBuffer : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float4 padding;
};

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    float4 pos = float4(vin.pos, 1.0f);

    pos = mul(pos, world);
    pos = mul(pos, view);
    pos = mul(pos, proj);

    vout.pos = pos;
    vout.uv = vin.uv;
    vout.color = float4(1.0f, 1.0f, 1.0f, padding[0]);

    return vout;
}
