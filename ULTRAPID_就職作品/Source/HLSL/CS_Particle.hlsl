struct ParticleData
{
    float3 position;
    float4 color;
    float3 velocity;
    float speed;
    float lifeTime;
    float ageTime;
    unsigned int isActive;
    unsigned int isVisible;
};

RWStructuredBuffer<ParticleData> particles : register(u0);

cbuffer Parameters : register(b0)
{
    float deltaTime;
    float gravity;
    float maxAlpha;
    unsigned int drawNum;
    float3 cameraPosition;
    float maxViewDistance;
    float4 frustumPlanes[6];
}

[numthreads(256, 1, 1)]
void cs_main(uint3 tid : SV_DispatchThreadID)
{
    
   // �O���[�o��ID�v�Z
    uint index = tid.x;

    // �͈͊O�̃C���f�b�N�X���`�F�b�N
    if (index >= drawNum)
        return;

    // �p�[�e�B�N���f�[�^���擾
    ParticleData particle = particles[index];

    // �p�[�e�B�N�����A�N�e�B�u�łȂ��ꍇ�A�X�L�b�v
    if (particle.isActive == 0)
        return;

    // ���x�ɂ��ʒu�̍X�V
    particle.position += particle.velocity * particle.speed * deltaTime;

    // �d�͂̓K�p
    particle.velocity.y -= gravity * deltaTime;

    // �����̍X�V
    particle.ageTime += deltaTime;

    // �A���t�@�l�̍X�V (�������߂����ꍇ�̑Ή��܂�)
    if (particle.ageTime >= particle.lifeTime)
    {
        particle.isActive = 0;
        particle.isVisible = 0;
    }
    else
    {
        float lifeRatio = saturate(1.0f - (particle.ageTime / particle.lifeTime));
        particle.color.a = lifeRatio * maxAlpha;
    }

    // �J�����Ƃ̋������v�Z
    float3 distDir = particle.position - cameraPosition;
    float distSqrt = dot(distDir, distDir);

    // �t���X�^�����`�F�b�N
    bool isInsideFrustum = true;
    for (int i = 0; i < 6; i++)
    {
        float dotProduct = dot(particle.position, frustumPlanes[i].xyz) + frustumPlanes[i].w;
        if (dotProduct < 0.0f) // ���ʂ̊O�ɂ���ꍇ
        {
            isInsideFrustum = false;
            break;
        }
    }

    // ����Ԃ̍X�V (�ő勗���ƃt���X�^�����`�F�b�N)
    particle.isVisible = (distSqrt <= maxViewDistance * maxViewDistance && isInsideFrustum) ? 1 : 0;

    // �X�V�����p�[�e�B�N�����o�b�t�@�ɏ����߂�
    particles[index] = particle;
  
}