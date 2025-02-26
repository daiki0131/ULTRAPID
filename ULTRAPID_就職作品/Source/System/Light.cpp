#include "Light.h"

using namespace DirectX;

// コンストラクタ
Light::Light(DirectX::XMFLOAT3 pos)
	:position(pos)
{
	up = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

// シャドウマップ用のビュー行列とプロジェクション行列計算
void Light::CalculateShadowMatrix(const DirectX::XMFLOAT3 _target, const DirectX::XMMATRIX& cameraView)
{
	// ビュー行列を XMFLOAT4X4 に変換
	DirectX::XMFLOAT4X4 view;
	DirectX::XMStoreFloat4x4(&view, cameraView);

	// ビュー行列の z軸（カメラの逆方向）を取得
	DirectX::XMFLOAT3 cameraDirection;
	cameraDirection.x = -view._13; // -X方向
	cameraDirection.y = -view._23; // -Y方向
	cameraDirection.z = -view._33; // -Z方向

	// 正規化
	DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&cameraDirection);
	dir = DirectX::XMVector3Normalize(dir);
	DirectX::XMStoreFloat3(&cameraDirection, dir);


	XMVECTOR lightPosition = XMVectorSet(position.x, position.y, position.z, 0.0f);  // ライトの位置
	XMVECTOR lightTarget = XMVectorSet(
		_target.x + (cameraDirection.x * -OffsetTarget),
		_target.y,
		_target.z + (cameraDirection.z * -OffsetTarget),
		0.0f);           // 照らす対象 (シーンの中心)
	XMVECTOR lightUp = XMVectorSet(up.x, up.y, up.z, 0.0f);

	// ビュー行列
	viewMatrix = XMMatrixLookAtLH(lightPosition, lightTarget, lightUp);

	// プロジェクション行列
	projectionMatrix = XMMatrixOrthographicLH(OrthoSize, OrthoSize, LightNearZ, LightFarZ);
}

// ビュー・プロジェクション行列を返す
DirectX::XMMATRIX Light::GetViewProjection()
{
	return viewMatrix * projectionMatrix;
}
