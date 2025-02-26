cbuffer LightBuffer : register(b4)
{
    matrix lightViewProj;
};

cbuffer ObjectBuffer : register(b1)
{
    matrix worldMatrix;  // ���[���h�s��
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

   // ���[���h��Ԃ�������̎��_��Ԃɕϊ�
    float4 worldPosition = mul(input.position, worldMatrix);
    
    // ���C�g�r���[�E�v���W�F�N�V�����s��Ōv�Z
    output.position = mul(worldPosition, lightViewProj);

    return output;
}