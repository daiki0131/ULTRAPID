struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
};

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    // 直接値を設定
    float4 ambientColor = float4(0.4f, 0.4f, 0.4f, 1.0f);         // 環境光の色
    float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);           // 光源の色
    float3 lightDirection = normalize(float3(1.0f, -0.5f, 0.0f)); // 光源の方向
    
    // 入力法線を正規化
    float3 normal = normalize(input.normal);

    // 光の方向を正規化
    float3 lightDir = normalize(-lightDirection);

    // ディフューズライティング計算
    float diffuseFactor = max(dot(normal, lightDir), 0.0f);

    // 環境光とディフューズ光を合計
    float4 diffuse = lightColor * diffuseFactor;
    float4 resultColor = ambientColor + diffuse;
    
    resultColor.rgb *= input.color.rgb;
    
    resultColor.a = input.color.a;

    // 最終出力色を返す
    return saturate(resultColor);
}