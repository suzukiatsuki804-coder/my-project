// ライトの定数バッファ
cbuffer CBLight : register(b1)
{
    float3 LightDir; // 光の方向
    float pad;

    float4 Diffuse; // 拡散光。主にここで色が変わる
    float4 Ambient; // 環境光
};
cbuffer CBColor : register(b2)
{
    float4 ObjColor;   // オブジェクトで定義している色
};
// 頂点シェーダから渡された構造体
struct VS_OUT
{
    float4 pos : SV_POSITION; // 頂点の座標情報
    float3 normal : NORMAL; // 法線情報（向き）
};

float4 main(VS_OUT pin) : SV_TARGET
{
#if 1
    // normalize: 計算しやすいように単位ベクトル化
    float3 N = normalize(pin.normal);
    float3 L = normalize(-LightDir);
    // 法線の方向と光の方向でDiffuseの反映度が変わってくる
    // dot: 内積。2つのベクトルの角度を求められる。
    float dotProduct = dot(N, L);
    
    // smoothstep でグラデーションを滑らかにする
    // smoothstep(a, b, x): a <= x <= b の間で 0～1 に滑らかに変化
    // 負の値も含めて処理することで、より自然なぼかしが実現される
    float diff = smoothstep(-1.0f, 1.0f, dotProduct);
    
    // カラー:環境光のrgb＋拡散光のrgb*diff
    float3 color = Ambient.rgb *  ObjColor.rgb + Diffuse.rgb * diff * ObjColor.rgb;
    // 最終的な色を出力
    return float4(color, 1.0f);
#else    
    // デバッグ用　法線を色にして表示
    float3 N = normalize(pin.normal);
    return float4(N * 0.5f + 0.5f, 1.0f);
#endif
}