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
    
   // グローバルID計算
    uint index = tid.x;

    // 範囲外のインデックスをチェック
    if (index >= drawNum)
        return;

    // パーティクルデータを取得
    ParticleData particle = particles[index];

    // パーティクルがアクティブでない場合、スキップ
    if (particle.isActive == 0)
        return;

    // 速度による位置の更新
    particle.position += particle.velocity * particle.speed * deltaTime;

    // 重力の適用
    particle.velocity.y -= gravity * deltaTime;

    // 寿命の更新
    particle.ageTime += deltaTime;

    // アルファ値の更新 (寿命が過ぎた場合の対応含む)
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

    // カメラとの距離を計算
    float3 distDir = particle.position - cameraPosition;
    float distSqrt = dot(distDir, distDir);

    // フラスタム内チェック
    bool isInsideFrustum = true;
    for (int i = 0; i < 6; i++)
    {
        float dotProduct = dot(particle.position, frustumPlanes[i].xyz) + frustumPlanes[i].w;
        if (dotProduct < 0.0f) // 平面の外にいる場合
        {
            isInsideFrustum = false;
            break;
        }
    }

    // 可視状態の更新 (最大距離とフラスタム内チェック)
    particle.isVisible = (distSqrt <= maxViewDistance * maxViewDistance && isInsideFrustum) ? 1 : 0;

    // 更新したパーティクルをバッファに書き戻す
    particles[index] = particle;
  
}