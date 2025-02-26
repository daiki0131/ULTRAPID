#include "Light.h"

using namespace DirectX;

// �R���X�g���N�^
Light::Light(DirectX::XMFLOAT3 pos)
	:position(pos)
{
	up = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

// �V���h�E�}�b�v�p�̃r���[�s��ƃv���W�F�N�V�����s��v�Z
void Light::CalculateShadowMatrix(const DirectX::XMFLOAT3 _target, const DirectX::XMMATRIX& cameraView)
{
	// �r���[�s��� XMFLOAT4X4 �ɕϊ�
	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view, cameraView);

	// �r���[�s��� z���i�J�����̋t�����j���擾
	DirectX::XMFLOAT3 cameraDirection;
	cameraDirection.x = -view._13; // -X����
	cameraDirection.y = -view._23; // -Y����
	cameraDirection.z = -view._33; // -Z����

	// ���K��
	DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&cameraDirection);
	dir = DirectX::XMVector3Normalize(dir);
	DirectX::XMStoreFloat3(&cameraDirection, dir);


	XMVECTOR lightPosition = XMVectorSet(position.x, position.y, position.z, 0.0f);  // ���C�g�̈ʒu
	XMVECTOR lightTarget = XMVectorSet(
		_target.x + (cameraDirection.x * -OffsetTarget),
		_target.y,
		_target.z + (cameraDirection.z * -OffsetTarget),
		0.0f);           // �Ƃ炷�Ώ� (�V�[���̒��S)
	XMVECTOR lightUp = XMVectorSet(up.x, up.y, up.z, 0.0f);

	// �r���[�s��
	viewMatrix = XMMatrixLookAtLH(lightPosition, lightTarget, lightUp);

	// �v���W�F�N�V�����s��
	projectionMatrix = XMMatrixOrthographicLH(OrthoSize, OrthoSize, LightNearZ, LightFarZ);
}

// �r���[�E�v���W�F�N�V�����s���Ԃ�
DirectX::XMMATRIX Light::GetViewProjection()
{
	return viewMatrix * projectionMatrix;
}
