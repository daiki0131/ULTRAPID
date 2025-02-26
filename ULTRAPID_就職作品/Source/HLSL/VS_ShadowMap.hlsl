cbuffer LightBuffer : register(b4)
{
    matrix lightViewProj;
};

cbuffer ObjectBuffer : register(b1)
{
    matrix worldMatrix;  // ワールド行列
    float4 color;
};

struct VSInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput vs_main(VSInput input)
{
   
    VSOutput output;

   // ワールド空間から光源の視点空間に変換
    float4 worldPosition = mul(input.position, worldMatrix);
    
    // ライトビュー・プロジェクション行列で計算
    output.position = mul(worldPosition, lightViewProj);

    return output;
}