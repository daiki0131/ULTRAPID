#pragma once
#include <DirectXMath.h>

//===========================================//
// ライトクラス
//===========================================//
class Light
{
public:
	static constexpr float OrthoSize = 150.0f;   // シャドウマップ領域のサイズ
	static constexpr float OffsetTarget = 60.0f;
	static constexpr float LightNearZ = 0.01f;
	static constexpr float LightFarZ = 10000.0f;

private:
	DirectX::XMFLOAT3 position{};          // 光源の位置
	DirectX::XMFLOAT3 up{};                // 上向きベクトル
	DirectX::XMFLOAT3 direction{};         // 光の方向
	DirectX::XMMATRIX viewMatrix{};        // ビュー行列
	DirectX::XMMATRIX projectionMatrix{};  // プロジェクション行列

public:
	Light(DirectX::XMFLOAT3 pos);
	void CalculateShadowMatrix(const DirectX::XMFLOAT3 _target, const DirectX::XMMATRIX& cameraView);
	DirectX::XMMATRIX GetViewProjection();
};

