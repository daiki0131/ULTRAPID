struct VS_INPUT
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float3 worldPos : TEXCOORD0;
    float4 color : COLOR;
};

cbuffer CameraBuffer : register(b0)
{
    matrix viewProjection;
}

cbuffer ObjectBuffer : register(b1)
{
    matrix matrixWorld;
    float4 color;
}

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;

    // ワールド座標計算
    output.worldPos = mul(input.pos, matrixWorld).xyz;

    // ワールド座標での法線
    output.normal = mul(input.normal, matrixWorld);

    // 最終的な画面位置
    output.pos = mul(input.pos, matrixWorld);
    output.pos = mul(output.pos, viewProjection);
    
    output.color = color;

    return output;
   
}