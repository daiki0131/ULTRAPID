struct VS_INPUT
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
    float4 shadowPos : TEXCOORD1;
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

cbuffer LightBuffer : register(b4)
{
    matrix lightViewProjection;
}

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
 
    // ワールド変換行列を頂点に掛ける
    float4 pos = mul(input.pos, matrixWorld);
    output.pos = mul(pos, viewProjection);
    
    float3x3 rotationMatrix = (float3x3) matrixWorld;

    rotationMatrix[0] = normalize(rotationMatrix[0]); // X軸
    rotationMatrix[1] = normalize(rotationMatrix[1]); // Y軸
    rotationMatrix[2] = normalize(rotationMatrix[2]); // Z軸

    float3 normal = mul((float3) input.normal.xyz, rotationMatrix);
    output.normal = float4(normal, 0.0f);
    
    output.color = color;
    
    output.shadowPos = mul(pos, lightViewProjection);
   
    return output;
}