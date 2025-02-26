cbuffer MaterialBuffer : register(b0)
{
    float4 emission;
    float4 specular;
    float4 ambient;
    float4 other;
};

// �V���h�E�}�b�v�p�̃e�N�X�`���ƃT���v���[
Texture2D shadowMap : register(t1);
SamplerComparisonState shadowSampler : register(s1);

struct PS_INPUT
{
    float4 pos : SV_POSITION; // ��ʍ��W
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
    float4 shadowPos : TEXCOORD1; // �V���h�E�}�b�v���W
};

// �����̈ʒu
static const float3 LightPosition = float3(0.0f, 5000.0f, -3000.0f);

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    
    // �@���x�N�g���̐��K��
    float3 normal = normalize(input.normal.xyz);

    // �����̕����x�N�g�����v�Z
    float3 lightDir = normalize(LightPosition - input.pos.xyz);

    // �@���ƌ����x�N�g���̓��ς����A�e���̋������v�Z
    float diff = max(dot(normal, lightDir), 0.0f);

    // ��{�̃f�B�t���[�Y����
    float3 diffuse = input.color.rgb * (diff + 0.4f);

    // ��������
    float3 amb = ambient.xyz * ambient.r;
    
    // ���������x�N�g�����v�Z
    float3 viewDir = normalize(-input.pos.xyz);

    // �e�F�����ɑ΂��Ă킸���ɈقȂ锽�˃x�N�g���̌v�Z
    float3 reflectR = reflect(-lightDir, normal + other.x * float3(1.0f, 0.0f, 0.0f));
    float3 reflectG = reflect(-lightDir, normal + other.x * float3(0.0f, 1.0f, 0.0f));
    float3 reflectB = reflect(-lightDir, normal + other.x * float3(0.0f, 0.0f, 1.0f));

    // ���ʔ��ː����̌v�Z
    float specR = pow(max(dot(viewDir, reflectR), 0.0f), specular.r);
    float specG = pow(max(dot(viewDir, reflectG), 0.0f), specular.r);
    float specB = pow(max(dot(viewDir, reflectB), 0.0f), specular.r);
    
     // ���F���ʂ𔽎˃J���[�ɓK�p
    float3 spec = specular.xyz * float3(specR, specG, specB);

    // �G�~�b�V��������
    float3 emis = emission.xyz * emission.r;

    // �V���h�E�}�b�v��UV���W���v�Z
    float2 screenUV = input.shadowPos.xy / input.shadowPos.w;
    screenUV = screenUV * 0.5f + 0.5f; // ���K��
    screenUV.y = 1.0f - screenUV.y;    // Y�����]

    // �I�u�W�F�N�g�̐[�x�l���v�Z
    float objDepth = input.shadowPos.z / input.shadowPos.w;

    // �V���h�E�o�C�A�X�̌v�Z
    float bias = 0.00001f;

    // �T���v���[�ŃV���h�E�}�b�v�̐[�x���擾
    float texDepth = shadowMap.SampleCmpLevelZero(shadowSampler, screenUV, objDepth - bias);

    // UV���͈͊O�̏ꍇ�͉e�𖳌��ɂ���
    if (screenUV.x < 0.0f || screenUV.x > 1.0f || screenUV.y < 0.0f || screenUV.y > 1.0f)
    {
        texDepth = 1.0f; // �͈͊O�Ȃ�e�Ȃ�
    }

    // �V���h�E�l���v�Z
    float shadowFactor = step(objDepth, texDepth);

    // �e�̔Z���𒲐�����W��
    float shadowStrength = 0.1f;
    shadowFactor = lerp(1.0f, shadowFactor, shadowStrength);

     // �ŏI���C�e�B���O�J���[������
    float3 lightingColor = diffuse + amb + spec + emis;
    
    // �V���h�E��K�p�����ŏI�J���[
    float3 finalColor = lightingColor * shadowFactor;

    return float4(finalColor, input.color.a);
}