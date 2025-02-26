struct BoundingSphere
{
    float3 center;
    float radius;
};

// GPU ���̃o�b�t�@
cbuffer PlayerData : register(b0)
{
    BoundingSphere playerSphere; // �v���C���[�̋�
}

StructuredBuffer<BoundingSphere> inputSpheres : register(t0);; // ���̋��̂̃f�[�^
RWStructuredBuffer<uint> results : register(u0);;     // ���ʊi�[

[numthreads(256, 1, 1)]
void cs_main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;

    BoundingSphere bs = inputSpheres[index];

    // �v���C���[�̋��Ƒ��̋��̋������v�Z
    float3 distVec = bs.center - playerSphere.center;
    float distSqrt = dot(distVec, distVec);
    float radiusSum = bs.radius + playerSphere.radius;

    // ���茋�ʂ�ۑ��i1:�Փ˂���, 0:�Փ˂Ȃ��j
    results[index] = (distSqrt <= radiusSum * radiusSum) ? 1 : 0;
}