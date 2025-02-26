cbuffer GSBuffer : register(b0)
{
    float  time;
    float3 center;
}

cbuffer CameraBuffer : register(b1)
{
    matrix viewProjection;
}

struct GS_INPUT
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float3 worldPos : TEXCOORD0;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
};

// �����_���������p�̃w���p�[�֐�
float RandomValue(float3 seed)
{
    return frac(sin(dot(seed, float3(12.9898, 78.233, 45.164))) * 43758.5453);
}


[maxvertexcount(6)]
void gs_main(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> outputStream)
{
    // �e�O�p�`�̒��_������
    for (int i = 0; i < 3; ++i)
    {
        // �����_�����������邽�߂̎��ݒ�
        float randomScale = RandomValue(input[i].worldPos) + 0.5f;
        
        // �����̕����x�N�g�����v�Z
        float3 direction = normalize(input[i].worldPos - center);
        float3 displacement = direction * time * randomScale * 3.5f;

        // �V�����ʒu���v�Z
        float3 newPos1 = input[i].worldPos + displacement;
        float3 newPos2 = input[(i + 1) % 3].worldPos + displacement;

        // �V�����ʒu���N���b�v��Ԃɕϊ�
        PS_INPUT output1, output2;

        output1.pos = mul(float4(newPos1, 1.0f), viewProjection);
        output2.pos = mul(float4(newPos2, 1.0f), viewProjection);
        
        output1.color = input[i].color;
        output2.color = input[(i + 1) % 3].color;
        
        output1.tex = input[i].worldPos.xy;
        output2.tex = input[(i + 1) % 3].worldPos.xy;
        
        output1.normal = input[i].normal;
        output2.normal = input[(i + 1) % 3].normal;

        // �X�g���[���ɐV�������_��ǉ�
        outputStream.Append(output1);
        outputStream.Append(output2);
    }

    // �O�p�`�̍ĊJ
    outputStream.RestartStrip();
}