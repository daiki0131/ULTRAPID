#include "Frustum.h"

using namespace DirectX;

// ViewProjection行列からフラスタムの平面を計算
void Frustum::ConstructFrustum(const DirectX::XMMATRIX& viewProjMatrix)
{
    XMFLOAT4X4 m;
    XMStoreFloat4x4(&m, viewProjMatrix);

    // 各平面の計算 (Ax + By + Cz + D = 0 の形)
    planes[0] = XMFLOAT4(m._14 + m._11, m._24 + m._21, m._34 + m._31, m._44 + m._41); // Left
    planes[1] = XMFLOAT4(m._14 - m._11, m._24 - m._21, m._34 - m._31, m._44 - m._41); // Right
    planes[2] = XMFLOAT4(m._14 - m._12, m._24 - m._22, m._34 - m._32, m._44 - m._42); // Top
    planes[3] = XMFLOAT4(m._14 + m._12, m._24 + m._22, m._34 + m._32, m._44 + m._42); // Bottom
    planes[4] = XMFLOAT4(m._13, m._23, m._33, m._43);                                 // Near
    planes[5] = XMFLOAT4(m._14 + m._13, m._24 + m._23, m._34 + m._33, m._44 + m._43); // Far


    // 各平面の法線ベクトルを正規化して、D を調整する
    for (int i = 0; i < 6; i++)
    {
        // 平面をXMVECTORとしてロード
        XMVECTOR plane = XMLoadFloat4(&planes[i]);

        // 平面の法線ベクトルと D 項を取得
        XMVECTOR normal = XMVectorSet(planes[i].x, planes[i].y, planes[i].z, 0.0f);
        float d = planes[i].w;

        // D 項をオフセット (expandAmount * 法線の長さ)
        float adjustment = ExpandAmount;
        d += adjustment;

        // 平面を再計算して保存
        XMVECTOR adjustedPlane = XMVectorSetW(normal, d);
        adjustedPlane = XMPlaneNormalize(adjustedPlane); // 正規化
        XMStoreFloat4(&planes[i], adjustedPlane);
    }
}

// スフィアをフラスタムの当たり判定実行
bool Frustum::CheckSphere(const Collision::BoundingSphere& bs)
{
    // 球の中心と半径をXMVECTOR型に変換
    XMVECTOR center = XMLoadFloat3(&bs.center);
    XMVECTOR radiusVec = XMVectorReplicate(bs.radius);

    // 各平面との衝突チェック
    for (int i = 0; i < 6; i++)
    {
        // 現在の平面をXMVECTORとしてロード
        XMVECTOR plane = XMLoadFloat4(&planes[i]);

        // 球の中心と平面との距離を計算
        float distance = XMVectorGetX(XMPlaneDotCoord(plane, center));

        // 距離 + 半径が負であれば、球はフラスタムの外側にある
        if (distance + XMVectorGetX(radiusVec) < 0) return false;
    }
    return true;
}