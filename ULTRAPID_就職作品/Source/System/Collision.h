#pragma once
#include <iostream>
#include <DirectXMath.h>
#include <vector>
#include "Vector.h"
#include "Renderer.h"
#undef max
#undef min

class GameObject;

//===========================================//
// �����蔻����s���N���X
//===========================================//
class Collision
{
public:
	// �����蔻����s�����߂̋��̂̏��
	struct BoundingSphere
	{
		DirectX::XMFLOAT3 center;    // �I�u�W�F�N�g�̒��S���W
		float radius;                // ���̂̔��a
	};
	// �����蔻����s�����߂̒����̂̏��
	struct BoundingBox
	{
		DirectX::XMFLOAT3 center;    // �I�u�W�F�N�g�̒��S���W
		DirectX::XMFLOAT3 extents;   // ���S����e���ӂ܂ł̒���
	};
	// ��]���Ȃ������̂̍ŏ��E�ő�̃R�[�i�[�̍��W��\���\����
	// AABB = Axis Aligned Bounding Boxes
	struct AABB
	{
		DirectX::XMFLOAT3 min;       // �ŏ��R�[�i�[�̍��W
		DirectX::XMFLOAT3 max;       // �ő�R�[�i�[�̍��W
	};
	// �����̊J�n�E�I�_���i�[����\����
	struct Segment
	{
		DirectX::XMFLOAT3 start;
		DirectX::XMFLOAT3 end;
	};
	// �O�p�`����
	struct Triangle
	{
		DirectX::XMFLOAT3 normal;
		float d;
		DirectX::XMFLOAT3 vertices[3];
	};
	// �t���X�^���̊e����
	struct FrustumPlane
	{
		DirectX::XMFLOAT4 planes[6];
	};
	// �Q�[�����I�u�W�F�N�g�̋��̏��ƕ`��Active�̎Q�Ƃ��i�[����\����
	struct ActiveBoundsData
	{
		Collision::BoundingSphere boundingSphere;
		std::reference_wrapper<bool> activeState;
	};

	static constexpr int MaxThreadDataNum = 256; // �R���s���[�g�V�F�[�_�[�Ōv�Z����X���b�h��

private:
	static ComPtr<ID3D11Buffer> sphereBoundsBuffer;          // �Փ˔���Ώۂ̃I�u�W�F�N�g�o�b�t�@
	static ComPtr<ID3D11Buffer> frustumPlaneBuffer;          // �J�����̃t���X�^���o�b�t�@
	static ComPtr<ID3D11Buffer> playerSphereBoundsBuffer;    // �v���C���[�̃X�t�B�A�o�b�t�@
	static ComPtr<ID3D11Buffer> visibilityBuffer;            // �Փˌ��ʂ�ۑ�����o�b�t�@
	static ComPtr<ID3D11Buffer> visibilityStagingBuffer;     // visibilityBuffer��CPU���ł̓ǂݎ���p
	static ComPtr<ID3D11ShaderResourceView> sphereBoundsSRV; // sphereBoundsBuffer��GPU���ł̓ǂݎ���p
	static ComPtr<ID3D11UnorderedAccessView> visibilityUAV;  // visibilityBuffer�̏������݂��s�����߂̃o�b�t�@

public:
	static void Init();
	// �ŏ��E�ő�̃R�[�i�[�̍��W���v�Z����v�Z����֐�
	static AABB CalculateAABB(BoundingBox _bb);
	// ���̓��m�̓����蔻�茋�ʂ�Ԃ��֐�
	static bool IsHitSphereAndSphere(BoundingSphere& _bs1, BoundingSphere& _bs2);
	// ���̂ƒ����́i��]�Ȃ��j�̓����蔻�茋�ʂ�Ԃ��֐�
	static bool IsHitSphereAndBox(BoundingSphere& _bs, AABB& _aabb, DirectX::XMFLOAT3& adjustPos, DirectX::XMFLOAT3& _CollisionNormal);
	// �_�ƒ����́i��]�Ȃ��j�̓����蔻�茋�ʂ�Ԃ��֐�
	static bool IsHitPointAndBox(DirectX::XMFLOAT3& _point, AABB& _aabb);
	// �_�Ƌ��̂̓����蔻�茋�ʂ�Ԃ��֐�
	static bool IsHitPointAndSphere(DirectX::XMFLOAT3& _point, BoundingSphere& _bs);
	// �����蔻���̂߂荞�񂾕��̍��W�����֐�
	static DirectX::XMFLOAT3 AdjustPositionSphereAndSphere(BoundingSphere& _adjustBs, BoundingSphere& _bs);
	// �����ƒ����̂̓����蔻��֐��i�Փ˂����߂����̓_�Ɣ��茋�ʂ�Ԃ��j
	static bool IsHitLineAndBox(Segment& _segment, AABB& _aabb, DirectX::XMFLOAT3& _adjustPos);
	// �����ƕ��ʂ̓����蔻��
	static bool IsHitSegmentAndPlane(const Segment& seg, const Triangle& plane, DirectX::XMFLOAT3& intersectionPoint);
	// ���ƕ��ʂ̓����蔻��
	static bool CheckSpherePlaneCollision(const Collision::Triangle& plane, const Collision::BoundingSphere& ps, DirectX::XMFLOAT3& _adjustPos, DirectX::XMFLOAT3& _CollisionNormal);
	// ���ƗŐ��̓����蔻��
	static bool CheckSphereEdgeCollision(const DirectX::XMVECTOR& v0, const DirectX::XMVECTOR& v1, const Collision::BoundingSphere& ps);
	// ���ƒ��_�̓����蔻��
	static bool CheckSphereVertexCollision(const DirectX::XMVECTOR& vertex, const Collision::BoundingSphere& ps);
	// �_���O�p�`�̓����ɂ��邩����
	static bool IsPointInTriangle(DirectX::XMVECTOR point, DirectX::XMVECTOR v0, DirectX::XMVECTOR v1, DirectX::XMVECTOR v2);
	// �t���X�^���J�����O��p�����蔻��i�R���s���[�g�V�F�[�_�[�ł̌v�Z�j
	static void FrustumCheck(const DirectX::XMFLOAT4* planes, std::vector<ActiveBoundsData>& activeBounds);
	// �u���[�h�t�F�[�Y��p���Ƌ��̓����蔻��i�R���s���[�g�V�F�[�_�[�ł̌v�Z�j
	static void BroadPhaseCheck(const BoundingSphere& playerBounds, std::vector<ActiveBoundsData>& activeBounds);
};