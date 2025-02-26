#pragma once
#include "GameObject.h"
#include "Collision.h"

//===========================================//
// 坂道オブジェクト専用クラス
//===========================================//
class SlopeObject : public GameObject
{
public:
    const float sphereRadiusRatio = 1.5f;      // スフィア判定の半径の倍率

private:
    std::vector<Collision::Triangle> planes;  // 坂道の各平面
    Collision::BoundingSphere boundingSphere; // ブロードフェーズ用球体情報
    bool isCollisionCheck = false;            // プレイヤーとの衝突判定可否

#ifdef _DEBUG
    DirectX::XMFLOAT4 debugSphereColor = { 0.0f,1.0f,0.0f,0.3f };
#endif

public:
    SlopeObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    void Update(float _deltaTime) override {};
    void Draw() override {};
    // Set関数
    void SetPlane();
    void SetIsCollisionCheck(bool _isCheck) { isCollisionCheck = _isCheck; }
    // Get関数
    Collision::BoundingSphere& GetBoundingSphere() { return boundingSphere; }
    bool& GetIsCollisionCheck() { return isCollisionCheck; }
    std::vector<Collision::Triangle> GetPlanes() { return planes; }
    // 三角形の平面情報を抽出
    std::vector<Collision::Triangle> GetPlanesFromMesh(const std::vector<DirectX::XMFLOAT3>& vertices, const std::vector<UINT>& indices, const DirectX::XMMATRIX& transform);
    // 法線と平面方程式の係数を計算
    Collision::Triangle CalculatePlane(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const DirectX::XMFLOAT3& v3);
    //頂点を行列を使って計算 
    std::vector<DirectX::XMFLOAT3> TransformVertices(const std::vector<DirectX::XMFLOAT3>& vertices, const DirectX::XMMATRIX& transform);
   
#ifdef _DEBUG
    void SetDebugSphereColor(DirectX::XMFLOAT4 _color) { debugSphereColor = _color; }
    DirectX::XMFLOAT4 GetDebugSphereColor() { return debugSphereColor; }
#endif
};