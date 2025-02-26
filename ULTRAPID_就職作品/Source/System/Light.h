#pragma once
#include <DirectXMath.h>

//===========================================//
// ���C�g�N���X
//===========================================//
class Light
{
public:
	static constexpr float OrthoSize = 150.0f;   // �V���h�E�}�b�v�̈�̃T�C�Y
	static constexpr float OffsetTarget = 60.0f;
	static constexpr float LightNearZ = 0.01f;
	static constexpr float LightFarZ = 10000.0f;

private:
	DirectX::XMFLOAT3 position{};          // �����̈ʒu
	DirectX::XMFLOAT3 up{};                // ������x�N�g��
	DirectX::XMFLOAT3 direction{};         // ���̕���
	DirectX::XMMATRIX viewMatrix{};        // �r���[�s��
	DirectX::XMMATRIX projectionMatrix{};  // �v���W�F�N�V�����s��

public:
	Light(DirectX::XMFLOAT3 pos);
	void CalculateShadowMatrix(const DirectX::XMFLOAT3 _target, const DirectX::XMMATRIX& cameraView);
	DirectX::XMMATRIX GetViewProjection();
};

