
cbuffer CBTransform : register(b0)
{
    matrix wvp;     // World*View*Projection
    matrix world;   // ワールド行列。光の計算に必要
};

struct VS_IN
{
    // 頂点の位置
    // POSITIONは、入力レイアウトで定義されたセマンティクス
    float3 pos : POSITION;
    float3 normal : NORMAL; // 法線の方向ベクトル
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL; // 法線の方向ベクトル
};

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    // 最終座標は、ワールド座標変換、ビュー座標変換、プロジェクション座標変換を行って、スクリーンに表示される座標に変換する必要がある。
    vout.pos = mul(float4(vin.pos, 1), wvp);
    // normalize: 単位ベクトル化（長さ1のベクトル）
    // mul: 行列の掛け算。法線とワールド行列をかけて、ピクセルシェーダーで使う時の法線を計算している
    vout.normal = normalize(mul(vin.normal, (float3x3)world));
    return vout;
}
