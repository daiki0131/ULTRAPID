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
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
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

cbuffer UVBuffer : register(b3)
{
    matrix matrixUV;
}

VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output;
 
    // ワールド変換行列を頂点に掛ける
    output.pos = mul(input.pos, matrixWorld);
    output.pos = mul(output.pos, viewProjection);
    
    float4 tex4;
    tex4.xy = input.tex;
    tex4.z = 0;
    tex4.w = 1;
    output.tex = mul(tex4, matrixUV).xy;
    
    float3x3 rotationMatrix = (float3x3) matrixWorld;

    rotationMatrix[0] = normalize(rotationMatrix[0]); // X軸
    rotationMatrix[1] = normalize(rotationMatrix[1]); // Y軸
    rotationMatrix[2] = normalize(rotationMatrix[2]); // Z軸

    float3 normal = mul((float3) input.normal.xyz, rotationMatrix);
    output.normal = float4(normal, 0.0f);
    
    output.color = color;
   
    return output;
}