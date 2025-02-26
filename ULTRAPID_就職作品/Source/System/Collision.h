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
// 当たり判定を行うクラス
//===========================================//
class Collision
{
public:
	// 当たり判定を行うための球体の情報
	struct BoundingSphere
	{
		DirectX::XMFLOAT3 center;    // オブジェクトの中心座標
		float radius;                // 球体の半径
	};
	// 当たり判定を行うための直方体の情報
	struct BoundingBox
	{
		DirectX::XMFLOAT3 center;    // オブジェクトの中心座標
		DirectX::XMFLOAT3 extents;   // 中心から各軸辺までの長さ
	};
	// 回転しない直方体の最小・最大のコーナーの座標を表す構造体
	// AABB = Axis Aligned Bounding Boxes
	struct AABB
	{
		DirectX::XMFLOAT3 min;       // 最小コーナーの座標
		DirectX::XMFLOAT3 max;       // 最大コーナーの座標
	};
	// 直線の開始・終点を格納する構造体
	struct Segment
	{
		DirectX::XMFLOAT3 start;
		DirectX::XMFLOAT3 end;
	};
	// 三角形平面
	struct Triangle
	{
		DirectX::XMFLOAT3 normal;
		float d;
		DirectX::XMFLOAT3 vertices[3];
	};
	// フラスタムの各平面
	struct FrustumPlane
	{
		DirectX::XMFLOAT4 planes[6];
	};
	// ゲーム内オブジェクトの球体情報と描画Activeの参照を格納する構造体
	struct ActiveBoundsData
	{
		Collision::BoundingSphere boundingSphere;
		std::reference_wrapper<bool> activeState;
	};

	static constexpr int MaxThreadDataNum = 256; // コンピュートシェーダーで計算するスレッド数

private:
	static ComPtr<ID3D11Buffer> sphereBoundsBuffer;          // 衝突判定対象のオブジェクトバッファ
	static ComPtr<ID3D11Buffer> frustumPlaneBuffer;          // カメラのフラスタムバッファ
	static ComPtr<ID3D11Buffer> playerSphereBoundsBuffer;    // プレイヤーのスフィアバッファ
	static ComPtr<ID3D11Buffer> visibilityBuffer;            // 衝突結果を保存するバッファ
	static ComPtr<ID3D11Buffer> visibilityStagingBuffer;     // visibilityBufferのCPU側での読み取り専用
	static ComPtr<ID3D11ShaderResourceView> sphereBoundsSRV; // sphereBoundsBufferのGPU側での読み取り専用
	static ComPtr<ID3D11UnorderedAccessView> visibilityUAV;  // visibilityBufferの書き込みを行うためのバッファ

public:
	static void Init();
	// 最小・最大のコーナーの座標を計算する計算する関数
	static AABB CalculateAABB(BoundingBox _bb);
	// 球体同士の当たり判定結果を返す関数
	static bool IsHitSphereAndSphere(BoundingSphere& _bs1, BoundingSphere& _bs2);
	// 球体と直方体（回転なし）の当たり判定結果を返す関数
	static bool IsHitSphereAndBox(BoundingSphere& _bs, AABB& _aabb, DirectX::XMFLOAT3& adjustPos, DirectX::XMFLOAT3& _CollisionNormal);
	// 点と直方体（回転なし）の当たり判定結果を返す関数
	static bool IsHitPointAndBox(DirectX::XMFLOAT3& _point, AABB& _aabb);
	// 点と球体の当たり判定結果を返す関数
	static bool IsHitPointAndSphere(DirectX::XMFLOAT3& _point, BoundingSphere& _bs);
	// 当たり判定後のめり込んだ分の座標調整関数
	static DirectX::XMFLOAT3 AdjustPositionSphereAndSphere(BoundingSphere& _adjustBs, BoundingSphere& _bs);
	// 直線と直方体の当たり判定関数（衝突した近い方の点と判定結果を返す）
	static bool IsHitLineAndBox(Segment& _segment, AABB& _aabb, DirectX::XMFLOAT3& _adjustPos);
	// 線分と平面の当たり判定
	static bool IsHitSegmentAndPlane(const Segment& seg, const Triangle& plane, DirectX::XMFLOAT3& intersectionPoint);
	// 球と平面の当たり判定
	static bool CheckSpherePlaneCollision(const Collision::Triangle& plane, const Collision::BoundingSphere& ps, DirectX::XMFLOAT3& _adjustPos, DirectX::XMFLOAT3& _CollisionNormal);
	// 球と稜線の当たり判定
	static bool CheckSphereEdgeCollision(const DirectX::XMVECTOR& v0, const DirectX::XMVECTOR& v1, const Collision::BoundingSphere& ps);
	// 球と頂点の当たり判定
	static bool CheckSphereVertexCollision(const DirectX::XMVECTOR& vertex, const Collision::BoundingSphere& ps);
	// 点が三角形の内部にあるか判定
	static bool IsPointInTriangle(DirectX::XMVECTOR point, DirectX::XMVECTOR v0, DirectX::XMVECTOR v1, DirectX::XMVECTOR v2);
	// フラスタムカリング専用当たり判定（コンピュートシェーダーでの計算）
	static void FrustumCheck(const DirectX::XMFLOAT4* planes, std::vector<ActiveBoundsData>& activeBounds);
	// ブロードフェーズ専用球と球の当たり判定（コンピュートシェーダーでの計算）
	static void BroadPhaseCheck(const BoundingSphere& playerBounds, std::vector<ActiveBoundsData>& activeBounds);
};