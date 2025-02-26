cbuffer MaterialBuffer : register(b0)
{
    float4 emission;
    float4 specular;
    float4 ambient;
    float4 other;
};

// シャドウマップ用のテクスチャとサンプラー
Texture2D shadowMap : register(t1);
SamplerComparisonState shadowSampler : register(s1);

struct PS_INPUT
{
    float4 pos : SV_POSITION; // 画面座標
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
    float4 shadowPos : TEXCOORD1; // シャドウマップ座標
};

// 光源の位置
static const float3 LightPosition = float3(0.0f, 5000.0f, -3000.0f);

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    
    // 法線ベクトルの正規化
    float3 normal = normalize(input.normal.xyz);

    // 光源の方向ベクトルを計算
    float3 lightDir = normalize(LightPosition - input.pos.xyz);

    // 法線と光源ベクトルの内積を取り、影響の強さを計算
    float diff = max(dot(normal, lightDir), 0.0f);

    // 基本のディフューズ成分
    float3 diffuse = input.color.rgb * (diff + 0.4f);

    // 環境光成分
    float3 amb = ambient.xyz * ambient.r;
    
    // 視線方向ベクトルを計算
    float3 viewDir = normalize(-input.pos.xyz);

    // 各色成分に対してわずかに異なる反射ベクトルの計算
    float3 reflectR = reflect(-lightDir, normal + other.x * float3(1.0f, 0.0f, 0.0f));
    float3 reflectG = reflect(-lightDir, normal + other.x * float3(0.0f, 1.0f, 0.0f));
    float3 reflectB = reflect(-lightDir, normal + other.x * float3(0.0f, 0.0f, 1.0f));

    // 鏡面反射成分の計算
    float specR = pow(max(dot(viewDir, reflectR), 0.0f), specular.r);
    float specG = pow(max(dot(viewDir, reflectG), 0.0f), specular.r);
    float specB = pow(max(dot(viewDir, reflectB), 0.0f), specular.r);
    
     // 虹色効果を反射カラーに適用
    float3 spec = specular.xyz * float3(specR, specG, specB);

    // エミッション成分
    float3 emis = emission.xyz * emission.r;

    // シャドウマップのUV座標を計算
    float2 screenUV = input.shadowPos.xy / input.shadowPos.w;
    screenUV = screenUV * 0.5f + 0.5f; // 正規化
    screenUV.y = 1.0f - screenUV.y;    // Y軸反転

    // オブジェクトの深度値を計算
    float objDepth = input.shadowPos.z / input.shadowPos.w;

    // シャドウバイアスの計算
    float bias = 0.00001f;

    // サンプラーでシャドウマップの深度を取得
    float texDepth = shadowMap.SampleCmpLevelZero(shadowSampler, screenUV, objDepth - bias);

    // UVが範囲外の場合は影を無効にする
    if (screenUV.x < 0.0f || screenUV.x > 1.0f || screenUV.y < 0.0f || screenUV.y > 1.0f)
    {
        texDepth = 1.0f; // 範囲外なら影なし
    }

    // シャドウ値を計算
    float shadowFactor = step(objDepth, texDepth);

    // 影の濃さを調整する係数
    float shadowStrength = 0.1f;
    shadowFactor = lerp(1.0f, shadowFactor, shadowStrength);

     // 最終ライティングカラーを合成
    float3 lightingColor = diffuse + amb + spec + emis;
    
    // シャドウを適用した最終カラー
    float3 finalColor = lightingColor * shadowFactor;

    return float4(finalColor, input.color.a);
}