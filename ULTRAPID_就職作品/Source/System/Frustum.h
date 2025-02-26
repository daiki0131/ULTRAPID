#pragma once
#include "Collision.h"

//===========================================//
// フラスタムカリングクラス
//===========================================//
class Frustum
{
public:
	static constexpr float ExpandAmount = 50.0f; // フラスタムの大きさ調整

public:
	
	DirectX::XMFLOAT4 planes[6];  // カメラの平面

	// カメラのViewProjectionからフラスタムを計算
	void ConstructFrustum(const DirectX::XMMATRIX& viewProjMatrix);
	// フラスタムと球体との当たり判定
	bool CheckSphere(const Collision::BoundingSphere& bs);
};