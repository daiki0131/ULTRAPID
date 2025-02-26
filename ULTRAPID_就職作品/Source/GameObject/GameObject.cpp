#include "GameObject.h"

using namespace DirectX;

// �R���X�g���N�^
GameObject::GameObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	: position(_pos), rotation(_rot), scaling(_scale), color(_color)
{
	// ���[���h�ϊ��s����v�Z
	matrixWorld = CalculateMatrixWorld(_pos, _scale, _rot);

	// �����̐F��ݒ�
	firstColor = _color;
}

// UV�s��擾
DirectX::XMMATRIX GameObject::GetUvMatrix()
{
	return XMMatrixIdentity();
}

// ���l������RGB��ݒ肷��
void GameObject::SetRGBColor(DirectX::XMFLOAT3 _rgb)
{
	color.x = _rgb.x;
	color.y = _rgb.y;
	color.z = _rgb.z;
}

// ���l������RGB��Ԃ�
DirectX::XMFLOAT3 GameObject::GetRGBColor()
{
	return XMFLOAT3(color.x, color.y, color.z);
}

// ���[���h�ϊ��s����v�Z����
DirectX::XMMATRIX GameObject::CalculateMatrixWorld(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT3 _rot)
{
	// �ړ��s����v�Z
	XMMATRIX matrixMove = XMMatrixTranslation(_pos.x, _pos.y, _pos.z);

	// �X�P�[���s����v�Z
	XMMATRIX matrixScale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);

	// ��]�s����v�Z
	XMMATRIX matrixRotateY = XMMatrixRotationY(XMConvertToRadians(_rot.y));
	XMMATRIX matrixRotateX = XMMatrixRotationX(XMConvertToRadians(_rot.x));
	XMMATRIX matrixRotateZ = XMMatrixRotationZ(XMConvertToRadians(_rot.z));
	// �ŏI�I�ȉ�]�s��
	DirectX::XMMATRIX matrixRotate = matrixRotateX * matrixRotateY * matrixRotateZ;

	// ���[���h�s����v�Z���ĕԂ�
	return  matrixScale * matrixRotate * matrixMove;
}