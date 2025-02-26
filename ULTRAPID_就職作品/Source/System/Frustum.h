#pragma once
#include "Collision.h"

//===========================================//
// �t���X�^���J�����O�N���X
//===========================================//
class Frustum
{
public:
	static constexpr float ExpandAmount = 50.0f; // �t���X�^���̑傫������

public:
	
	DirectX::XMFLOAT4 planes[6];  // �J�����̕���

	// �J������ViewProjection����t���X�^�����v�Z
	void ConstructFrustum(const DirectX::XMMATRIX& viewProjMatrix);
	// �t���X�^���Ƌ��̂Ƃ̓����蔻��
	bool CheckSphere(const Collision::BoundingSphere& bs);
};