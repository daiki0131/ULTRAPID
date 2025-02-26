#pragma once
#include "GameObject.h"
#include "Collision.h"

//===========================================//
// �⓹�I�u�W�F�N�g��p�N���X
//===========================================//
class SlopeObject : public GameObject
{
public:
    const float sphereRadiusRatio = 1.5f;      // �X�t�B�A����̔��a�̔{��

private:
    std::vector<Collision::Triangle> planes;  // �⓹�̊e����
    Collision::BoundingSphere boundingSphere; // �u���[�h�t�F�[�Y�p���̏��
    bool isCollisionCheck = false;            // �v���C���[�Ƃ̏Փ˔����

#ifdef _DEBUG
    DirectX::XMFLOAT4 debugSphereColor = { 0.0f,1.0f,0.0f,0.3f };
#endif

public:
    SlopeObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    void Update(float _deltaTime) override {};
    void Draw() override {};
    // Set�֐�
    void SetPlane();
    void SetIsCollisionCheck(bool _isCheck) { isCollisionCheck = _isCheck; }
    // Get�֐�
    Collision::BoundingSphere& GetBoundingSphere() { return boundingSphere; }
    bool& GetIsCollisionCheck() { return isCollisionCheck; }
    std::vector<Collision::Triangle> GetPlanes() { return planes; }
    // �O�p�`�̕��ʏ��𒊏o
    std::vector<Collision::Triangle> GetPlanesFromMesh(const std::vector<DirectX::XMFLOAT3>& vertices, const std::vector<UINT>& indices, const DirectX::XMMATRIX& transform);
    // �@���ƕ��ʕ������̌W�����v�Z
    Collision::Triangle CalculatePlane(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const DirectX::XMFLOAT3& v3);
    //���_���s����g���Čv�Z 
    std::vector<DirectX::XMFLOAT3> TransformVertices(const std::vector<DirectX::XMFLOAT3>& vertices, const DirectX::XMMATRIX& transform);
   
#ifdef _DEBUG
    void SetDebugSphereColor(DirectX::XMFLOAT4 _color) { debugSphereColor = _color; }
    DirectX::XMFLOAT4 GetDebugSphereColor() { return debugSphereColor; }
#endif
};