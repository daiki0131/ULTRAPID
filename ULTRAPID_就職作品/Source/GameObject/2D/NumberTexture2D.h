#pragma once
#include "Renderer.h"
#include "Object2D.h"
#include <memory>

//===========================================//
// 数字UIを扱うクラス
//===========================================//
class NumberTexture2D
{
protected:
	static constexpr float UiLowAlpha = 0.2f;// 透明時のα値
	DirectX::XMFLOAT2 uv = { 0,0 };          // UV座標
	DirectX::XMFLOAT3 parentPosition{};      // 一括操作のための基本座標
	DirectX::XMFLOAT3 parentScale{};         // 一括操作のための基本スケール

public:
	NumberTexture2D() = default;
	// Set関数
	void SetUV(const int _num);
	void SetWorldMatrix(std::vector<std::unique_ptr<Object2D>>& _objects);
	virtual void SetParentPosition(DirectX::XMFLOAT3 _pos) = 0;
	virtual void SetParentScale(DirectX::XMFLOAT3 _scale) = 0;
	// Get関数
	DirectX::XMFLOAT3 GetParentPosition() { return parentPosition; }
	DirectX::XMFLOAT3 GetParentScale() { return parentScale; }
};

