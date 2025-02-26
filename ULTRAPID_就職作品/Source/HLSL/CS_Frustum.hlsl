struct BoundingSphere
{
    float3 center;
    float radius;
};

StructuredBuffer<BoundingSphere> boundingSpheres : register(t0);
RWStructuredBuffer<uint> results : register(u0); // カリング結果

cbuffer FrustumPlanes : register(b0)
{
    float4 planes[6];
}

[numthreads(256, 1, 1)]
void cs_main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;

    // バウンディングスフィア取得
    BoundingSphere bs = boundingSpheres[index];
    float3 center = bs.center;
    float radius = bs.radius;

    // カリングチェック
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

    // 結果を保存
    results[index] = isVisible ? 1 : 0;
}