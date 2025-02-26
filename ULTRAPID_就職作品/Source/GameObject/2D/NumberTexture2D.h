#pragma once
#include "Renderer.h"
#include "Object2D.h"
#include <memory>

//===========================================//
// ����UI�������N���X
//===========================================//
class NumberTexture2D
{
protected:
	static constexpr float UiLowAlpha = 0.2f;// �������̃��l
	DirectX::XMFLOAT2 uv = { 0,0 };          // UV���W
	DirectX::XMFLOAT3 parentPosition{};      // �ꊇ����̂��߂̊�{���W
	DirectX::XMFLOAT3 parentScale{};         // �ꊇ����̂��߂̊�{�X�P�[��

public:
	NumberTexture2D() = default;
	// Set�֐�
	void SetUV(const int _num);
	void SetWorldMatrix(std::vector<std::unique_ptr<Object2D>>& _objects);
	virtual void SetParentPosition(DirectX::XMFLOAT3 _pos) = 0;
	virtual void SetParentScale(DirectX::XMFLOAT3 _scale) = 0;
	// Get�֐�
	DirectX::XMFLOAT3 GetParentPosition() { return parentPosition; }
	DirectX::XMFLOAT3 GetParentScale() { return parentScale; }
};

