#pragma once
#include "GameObject.h"
#include "Collision.h"

//===========================================//
// 3D�{�b�N�X�I�u�W�F�N�g��p�N���X
//===========================================//
class BoxObject : public GameObject
{
private:
	Collision::BoundingBox boundingBox;        // �{�b�N�X���
    Collision::AABB aabb;                      // �{�b�N�X��AABB
	Collision::BoundingSphere boundingSphere;  // �u���[�h�t�F�[�Y�p���̏��
	const float sphereRadiusRatio = 1.0f;      // �X�t�B�A����̔��a�̔{��
	bool isCollisionCheck = false;             // �v���C���[�Ƃ̔�����s����
#ifdef _DEBUG
	DirectX::XMMATRIX debugWorldMatrix;
#endif

#ifdef _DEBUG
	DirectX::XMFLOAT4 debugSphereColor = { 0.0f,1.0f,0.0f,0.3f };
#endif

public:
    BoxObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
	void Draw() override;
	float DistanceFromCamera(const DirectX::XMFLOAT3& cameraPosition);
	// Set�֐�
	void SetBoundingBox(DirectX::XMFLOAT3 _center, DirectX::XMFLOAT3 _extents);
	void SetBoundingSpehre(DirectX::XMFLOAT3  _center, float _radius);
	void SetAABB(Collision::BoundingBox _bb) { aabb = Collision::CalculateAABB(_bb); }
	void SetIsCollisionCheck(bool _isCheck) { isCollisionCheck = _isCheck; }
	// Get�֐�
	Collision::BoundingBox& GetBoundingBox(){ return boundingBox; };
	Collision::BoundingSphere& GetBoundingSphere() { return boundingSphere; };
	Collision::AABB GetAABB() { return aabb; }
	bool& GetIsCollisionCheck() { return isCollisionCheck; }

#ifdef _DEBUG
	void SetDebugSphereColor(DirectX::XMFLOAT4 _color) { debugSphereColor = _color; }
	DirectX::XMFLOAT4 GetDebugSphereColor() { return debugSphereColor; }
	void SetDebugSphereWorldMatrix(DirectX::XMMATRIX _matrix) { debugWorldMatrix = _matrix; }
	DirectX::XMMATRIX GetDebugSphereWorldMatrix() { return debugWorldMatrix; }
protected:

#endif
};
