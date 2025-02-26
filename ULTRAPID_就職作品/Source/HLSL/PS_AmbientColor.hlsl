struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
    float4 normal : NORMAL;
};

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    // ���ڒl��ݒ�
    float4 ambientColor = float4(0.4f, 0.4f, 0.4f, 1.0f);         // �����̐F
    float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);           // �����̐F
    float3 lightDirection = normalize(float3(1.0f, -0.5f, 0.0f)); // �����̕���
    
    // ���͖@���𐳋K��
    float3 normal = normalize(input.normal);

    // ���̕����𐳋K��
    float3 lightDir = normalize(-lightDirection);

    // �f�B�t���[�Y���C�e�B���O�v�Z
    float diffuseFactor = max(dot(normal, lightDir), 0.0f);

    // �����ƃf�B�t���[�Y�������v
    float4 diffuse = lightColor * diffuseFactor;
    float4 resultColor = ambientColor + diffuse;
    
    resultColor.rgb *= input.color.rgb;
    
    resultColor.a = input.color.a;

    // �ŏI�o�͐F��Ԃ�
    return saturate(resultColor);
}