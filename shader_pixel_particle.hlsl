Texture2D tex : register(t0);
SamplerState samp : register(s0);

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = tex.Sample(samp, pin.uv);

    // アルファが0の場合は破棄
    if (color.a < 0.01f)
        discard;

    // パーティクルのアルファを適用
    color.a *= pin.color.a;

    return color;
}
