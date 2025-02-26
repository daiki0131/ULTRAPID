cbuffer MaterialBuffer : register(b0)
{
    float4 emission;
    float4 specular;
    float4 ambient;
    float4 other;
};

Texture2D textureDiffuse : register(t0);
SamplerState samplerState : register(s0);
Texture2D shadowMap : register(t1);
SamplerComparisonState shadowSampler : register(s1);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD0;
    float4 normal : NORMAL;
    float4 shadowPos : TEXCOORD1;
};

// �����̈ʒu
static const float3 LightPosition = float3(0.0f, 5000.0f, -3000.0f);

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    // �@���x�N�g���̐��K��
    float3 normal = normalize(input.normal.xyz);

    // �����̕����x�N�g�����v�Z
    float3 lightDir = normalize(LightPosition - input.pos.xyz);

    // �@���ƌ����x�N�g���̓��ς��v�Z
    float diff = max(dot(normal, lightDir), 0.0f);

    // ��{�̃f�B�t���[�Y�����i�e�N�X�`�����g�p�j
    float4 sampledTexture = textureDiffuse.Sample(samplerState, input.tex);
    float3 diffuse = sampledTexture.rgb * diff;

    // ��������
    float3 amb = ambient.xyz * ambient.r * sampledTexture.rgb;

    // ���������x�N�g�����v�Z
    float3 viewDir = normalize(-input.pos.xyz);

    // �e�F�����ɑ΂��Ă킸���ɈقȂ锽�˃x�N�g���̌v�Z
    float3 reflectR = reflect(-lightDir, normal + other.r * float3(1.0f, 0.0f, 0.0f));
    float3 reflectG = reflect(-lightDir, normal + other.r * float3(0.0f, 1.0f, 0.0f));
    float3 reflectB = reflect(-lightDir, normal + other.r * float3(0.0f, 0.0f, 1.0f));

    // ���ʔ��ː����̌v�Z
    float specR = pow(max(dot(viewDir, reflectR), 0.0f), specular.r);
    float specG = pow(max(dot(viewDir, reflectG), 0.0f), specular.r);
    float specB = pow(max(dot(viewDir, reflectB), 0.0f), specular.r);

    // �e�N�X�`���̐F�������ǂ������m�F
    float isWhite = step(0.9f, max(sampledTexture.r, max(sampledTexture.g, sampledTexture.b))); // RGB��0.9f�ȏ�Ȃ甒

    // ���ȊO�̕����ɂ̂݋��ʔ��˂�K�p
    float3 spec = (1.0f - isWhite) * specular.xyz * float3(specR, specG, specB);

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
    float shadowStrength = 0.3f;
    shadowFactor = lerp(1.0f, shadowFactor, shadowStrength);

    float3 lightingColor = diffuse + amb + spec + emis;
    
    // �V���h�E��K�p�����ŏI�J���[
    float3 finalColor = lightingColor * shadowFactor;

    return float4(finalColor * input.color.rgb, input.color.a);
}