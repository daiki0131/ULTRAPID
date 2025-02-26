
cbuffer LightBuffer : register(b4)
{
    matrix lightViewProj;
};


struct VSInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
    float4x4 worldMatrix : INSTANCE_MATRIX;
    float4 color : INSTANCE_COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput vs_main(VSInput input)
{
   
    VSOutput output;

    // ワールド空間から光源の視点空間に変換
    float4 worldPosition = mul(input.position, input.worldMatrix);
    
    // ライトビュー・プロジェクション行列で計算
    output.position = mul(worldPosition, lightViewProj);

    return output;
}