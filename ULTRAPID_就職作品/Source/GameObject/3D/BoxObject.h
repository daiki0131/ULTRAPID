#pragma once
#include "GameObject.h"
#include "Collision.h"

//===========================================//
// 3Dボックスオブジェクト専用クラス
//===========================================//
class BoxObject : public GameObject
{
private:
	Collision::BoundingBox boundingBox;        // ボックス情報
    Collision::AABB aabb;                      // ボックスのAABB
	Collision::BoundingSphere boundingSphere;  // ブロードフェーズ用球体情報
	const float sphereRadiusRatio = 1.0f;      // スフィア判定の半径の倍率
	bool isCollisionCheck = false;             // プレイヤーとの判定を行うか
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
	// Set関数
	void SetBoundingBox(DirectX::XMFLOAT3 _center, DirectX::XMFLOAT3 _extents);
	void SetBoundingSpehre(DirectX::XMFLOAT3  _center, float _radius);
	void SetAABB(Collision::BoundingBox _bb) { aabb = Collision::CalculateAABB(_bb); }
	void SetIsCollisionCheck(bool _isCheck) { isCollisionCheck = _isCheck; }
	// Get関数
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
