#include "GameObject.h"

using namespace DirectX;

// コンストラクタ
GameObject::GameObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	: position(_pos), rotation(_rot), scaling(_scale), color(_color)
{
	// ワールド変換行列を計算
	matrixWorld = CalculateMatrixWorld(_pos, _scale, _rot);

	// 初期の色を設定
	firstColor = _color;
}

// UV行列取得
DirectX::XMMATRIX GameObject::GetUvMatrix()
{
	return XMMatrixIdentity();
}

// α値を除くRGBを設定する
void GameObject::SetRGBColor(DirectX::XMFLOAT3 _rgb)
{
	color.x = _rgb.x;
	color.y = _rgb.y;
	color.z = _rgb.z;
}

// α値を除くRGBを返す
DirectX::XMFLOAT3 GameObject::GetRGBColor()
{
	return XMFLOAT3(color.x, color.y, color.z);
}

// ワールド変換行列を計算する
DirectX::XMMATRIX GameObject::CalculateMatrixWorld(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT3 _rot)
{
	// 移動行列を計算
	XMMATRIX matrixMove = XMMatrixTranslation(_pos.x, _pos.y, _pos.z);

	// スケール行列を計算
	XMMATRIX matrixScale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);

	// 回転行列を計算
	XMMATRIX matrixRotateY = XMMatrixRotationY(XMConvertToRadians(_rot.y));
	XMMATRIX matrixRotateX = XMMatrixRotationX(XMConvertToRadians(_rot.x));
	XMMATRIX matrixRotateZ = XMMatrixRotationZ(XMConvertToRadians(_rot.z));
	// 最終的な回転行列
	DirectX::XMMATRIX matrixRotate = matrixRotateX * matrixRotateY * matrixRotateZ;

	// ワールド行列を計算して返す
	return  matrixScale * matrixRotate * matrixMove;
}