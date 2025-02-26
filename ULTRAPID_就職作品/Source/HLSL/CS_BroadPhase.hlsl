struct BoundingSphere
{
    float3 center;
    float radius;
};

// GPU 側のバッファ
cbuffer PlayerData : register(b0)
{
    BoundingSphere playerSphere; // プレイヤーの球
}

StructuredBuffer<BoundingSphere> inputSpheres : register(t0);; // 他の球体のデータ
RWStructuredBuffer<uint> results : register(u0);;     // 結果格納

[numthreads(256, 1, 1)]
void cs_main(uint3 threadID : SV_DispatchThreadID)
{
    uint index = threadID.x;

    BoundingSphere bs = inputSpheres[index];

    // プレイヤーの球と他の球の距離を計算
    float3 distVec = bs.center - playerSphere.center;
    float distSqrt = dot(distVec, distVec);
    float radiusSum = bs.radius + playerSphere.radius;

    // 判定結果を保存（1:衝突あり, 0:衝突なし）
    results[index] = (distSqrt <= radiusSum * radiusSum) ? 1 : 0;
}