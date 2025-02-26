cbuffer CameraBuffer : register(b0)
{
    matrix viewProjection;
}

cbuffer LightBuffer : register(b4)
{
    matrix lightViewProjection;
}

struct VS_INPUT
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
    matrix worldMatrix : INSTANCE_MATRIX;
    float4 color : INSTANCE_COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
    float4 shadowPos : TEXCOORD1;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;

    // ワールド空間での位置計算
    float4 pos = mul(input.pos, input.worldMatrix);
    output.pos = mul(pos, viewProjection);
    
    float3x3 rotationMatrix = (float3x3) input.worldMatrix;

    rotationMatrix[0] = normalize(rotationMatrix[0]); // X軸
    rotationMatrix[1] = normalize(rotationMatrix[1]); // Y軸
    rotationMatrix[2] = normalize(rotationMatrix[2]); // Z軸
    
    float3 normal = mul((float3) input.normal.xyz, rotationMatrix);
    output.normal = float4(normal, 0.0f);
    
    output.color = input.color;
    
    output.shadowPos = mul(pos, lightViewProjection);

    return output;
    
}
