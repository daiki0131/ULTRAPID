struct BoundingSphere
{
    float3 center;
    float radius;
};

StructuredBuffer<BoundingSphere> boundingSpheres : register(t0);
RWStructuredBuffer<uint> results : register(u0); // �J�����O����

cbuffer FrustumPlanes : register(b0)
{
    float4 planes[6];
}

[numthreads(256, 1, 1)]
void cs_main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;

    // �o�E���f�B���O�X�t�B�A�擾
    BoundingSphere bs = boundingSpheres[index];
    float3 center = bs.center;
    float radius = bs.radius;

    // �J�����O�`�F�b�N
    bool isVisible = true;
    for (int i = 0; i < 6; i++)
    {
        float distance = dot(planes[i].xyz, center) + planes[i].w;
        if (distance + radius < 0)
        {
            isVisible = false;
            break;
        }
    }

    // ���ʂ�ۑ�
    results[index] = isVisible ? 1 : 0;
}