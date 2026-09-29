// shader_pixel_2d.hlsl
Texture2D major_texture : register(t0); // テクスチャ（スロット0）
SamplerState major_sampler : register(s0); // サンプラー（スロット0）

struct PS_INPUT
{
    float4 posH : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT ps_in) : SV_TARGET
{
    float4 texColor = major_texture.Sample(major_sampler, ps_in.uv);
    // テクスチャのアルファ（texColor.a）と頂点カラーのアルファを乗算して返す
    return float4(texColor.rgb, texColor.a * ps_in.color.a);
}