#include "Frustum.h"

using namespace DirectX;

// ViewProjection�s�񂩂�t���X�^���̕��ʂ��v�Z
void Frustum::ConstructFrustum(const DirectX::XMMATRIX& viewProjMatrix)
{
    XMFLOAT4X4 m;
    XMStoreFloat4x4(&m, viewProjMatrix);

    // �e���ʂ̌v�Z (Ax + By + Cz + D = 0 �̌`)
    planes[0] = XMFLOAT4(m._14 + m._11, m._24 + m._21, m._34 + m._31, m._44 + m._41); // Left
    planes[1] = XMFLOAT4(m._14 - m._11, m._24 - m._21, m._34 - m._31, m._44 - m._41); // Right
    planes[2] = XMFLOAT4(m._14 - m._12, m._24 - m._22, m._34 - m._32, m._44 - m._42); // Top
    planes[3] = XMFLOAT4(m._14 + m._12, m._24 + m._22, m._34 + m._32, m._44 + m._42); // Bottom
    planes[4] = XMFLOAT4(m._13, m._23, m._33, m._43);                                 // Near
    planes[5] = XMFLOAT4(m._14 + m._13, m._24 + m._23, m._34 + m._33, m._44 + m._43); // Far


    // �e���ʂ̖@���x�N�g���𐳋K�����āAD �𒲐�����
    for (int i = 0; i < 6; i++)
    {
        // ���ʂ�XMVECTOR�Ƃ��ă��[�h
        XMVECTOR plane = XMLoadFloat4(&planes[i]);

        // ���ʂ̖@���x�N�g���� D �����擾
        XMVECTOR normal = XMVectorSet(planes[i].x, planes[i].y, planes[i].z, 0.0f);
        float d = planes[i].w;

        // D �����I�t�Z�b�g (expandAmount * �@���̒���)
        float adjustment = ExpandAmount;
        d += adjustment;

        // ���ʂ��Čv�Z���ĕۑ�
        XMVECTOR adjustedPlane = XMVectorSetW(normal, d);
        adjustedPlane = XMPlaneNormalize(adjustedPlane); // ���K��
        XMStoreFloat4(&planes[i], adjustedPlane);
    }
}

// �X�t�B�A���t���X�^���̓����蔻����s
bool Frustum::CheckSphere(const Collision::BoundingSphere& bs)
{
    // ���̒��S�Ɣ��a��XMVECTOR�^�ɕϊ�
    XMVECTOR center = XMLoadFloat3(&bs.center);
    XMVECTOR radiusVec = XMVectorReplicate(bs.radius);

    // �e���ʂƂ̏Փ˃`�F�b�N
    for (int i = 0; i < 6; i++)
    {
        // ���݂̕��ʂ�XMVECTOR�Ƃ��ă��[�h
        XMVECTOR plane = XMLoadFloat4(&planes[i]);

        // ���̒��S�ƕ��ʂƂ̋������v�Z
        float distance = XMVectorGetX(XMPlaneDotCoord(plane, center));

        // ���� + ���a�����ł���΁A���̓t���X�^���̊O���ɂ���
        if (distance + XMVectorGetX(radiusVec) < 0) return false;
    }
    return true;
}