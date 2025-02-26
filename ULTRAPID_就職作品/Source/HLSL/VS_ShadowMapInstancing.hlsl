
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

    // ���[���h��Ԃ�������̎��_��Ԃɕϊ�
    float4 worldPosition = mul(input.position, input.worldMatrix);
    
    // ���C�g�r���[�E�v���W�F�N�V�����s��Ōv�Z
    output.position = mul(worldPosition, lightViewProj);

    return output;
}