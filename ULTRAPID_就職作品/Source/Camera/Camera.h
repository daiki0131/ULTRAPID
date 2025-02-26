#pragma once
#include "SceneManager.h"

class TitleScene;
class GameScene;
class SceneManager;

//===========================================//
// カメラ基底クラス
//===========================================//
class Camera
{
protected:
	DirectX::XMMATRIX matrixView;        // 変換行列
	DirectX::XMFLOAT3 eye = { 0,0,-10 }; // カメラの位置
	DirectX::XMFLOAT3 focus = { 0,0,0 }; // カメラの見ている位置
	DirectX::XMFLOAT3 up = { 0,1,0 };    // カメラの上方向ベクトル

public:
	
	// Set関数
	void SetEye(const DirectX::XMFLOAT3 _eye) { eye = _eye; };
	void SetFocus(const DirectX::XMFLOAT3 _focus) { focus = _focus; }
	// Get関数
	DirectX::XMFLOAT3 GetEye() { return eye; }
	DirectX::XMFLOAT3 GetFocus() { return focus; }
	DirectX::XMMATRIX GetViewProjectionMatrix();
	virtual DirectX::XMMATRIX GetProjectionMatrix() = 0;
	DirectX::XMMATRIX GetViewMatrix() { return matrixView; };
	// ビュー行列計算
	void CalculateViewMatrix(DirectX::XMFLOAT3 _eye, DirectX::XMFLOAT3 _focus, DirectX::XMFLOAT3 _up);
};

//===========================================//
// カメラ2Dクラス
//===========================================//
class Camera2D : public Camera
{
public:

	void Update(const float _deltaTime);
	// Get関数
	DirectX::XMMATRIX GetProjectionMatrix() override;
};

//===========================================//
// カメラ3Dクラス
//===========================================//
class Camera3D : public Camera
{
public:
	static const float MaxAdjustFocus;                  // Focusの調整値
	static const float MinEyeZ;                         // EyeのZ値最小の調整値

protected:
	float fovAngle = 45.0f;                             // カメラの視野角度
	float revoAngle = 0;                                // カメラの回転角度
	float revoAngleSpeed = 0.035f;                      // 回転速度
	DirectX::XMFLOAT3 adjustFocus = { 0.0f,2.0f,0.0f }; // カメラのターゲットの補正
	DirectX::XMFLOAT3 adjustEye = { 0.0f,6.0f,-10.0f }; // カメラの座標の補正
	DirectX::XMFLOAT3 afterEye = { 0,0,0 };             // 移動後のカメラ座標

public:
	// Set関数
	void SetAdjustFocus(DirectX::XMFLOAT3 _focus) { adjustFocus = _focus; }
	void SetRevoAngle(float _angle) { revoAngle = _angle; }
	void SetFovAngle(float _fov) { fovAngle = _fov; };
	// Get関数
	DirectX::XMMATRIX GetProjectionMatrix() override;
	DirectX::XMFLOAT3 GetAdjustFocus() { return adjustFocus; }
	DirectX::XMFLOAT3 const GetAfterEye() { return afterEye; }
	float const GetRevoAngle() { return revoAngle; }
	float const GetFovAngle() { return fovAngle; }

};

//===========================================//
// タイトルシーン専用カメラ
//===========================================//
class TitleSceneCamera : public Camera3D
{
public:
	void Update(const float _deltaTime, TitleScene& titleScene);

};

//===========================================//
// ゲームシーン専用カメラ
//===========================================//
class GameSceneCamera : public Camera3D
{
public:
	static const float MaxFov;                     // 最大視野角
	static const float MinFov;                     // 最小視野角
	static const float MaxShakeIntensity;          // 最大のカメラの揺れの強さ

private:
	float maxShakeIntensity = MaxShakeIntensity;   // 揺れの強さ（最大）
	float shakeIntensity = 0.0f;                   // 揺れの強さ
	float shakeDuration = 0.0f;                    // 揺れの持続時間
	float shakeElapsedTime = 0.0f;                 // 揺れ経過時間
	float minFov = 45.0f;                          // 最小視野角
	float maxFov = 150.0f;                         // 最大視野角

	bool isChaseCamera = true;                     // カメラが追従可否
	bool isInputCamera = true;                     // 入力可否
	bool isHitStop = false;                        // ヒットストップ可否

	DirectX::XMFLOAT3 nowPlayerPos;
	DirectX::XMFLOAT3 oldPlayerPos;

public:
	void Update(const float _deltaTime, GameScene& gameScene);
	// Set関数
	void StartCameraShake(float _hitStopTime, float intensityRatio);
	void SetIsChase(bool _isChase) { isChaseCamera = _isChase; }
	void SetIsInputCamera(bool _isInput) { isInputCamera = _isInput; }
	void SetIsHitStop(bool _isHitStop) { isHitStop = _isHitStop; }
	// Get関数
	DirectX::XMFLOAT3 GetAfterEye() { return afterEye; }

private:
	// カメラの揺れを反映
	DirectX::XMFLOAT3 ApplyCameraShake(DirectX::XMFLOAT3 _eye);
	// カメラの揺れを更新
	void UpdateCameraShake(const float _deltaTime);
};

//===========================================//
// デバッグ専用カメラ
//===========================================//
class DebugCamera : public Camera3D
{
private:
	const float MouseSensitivity = 0.004f;  // マウスの感度
	const float MoveSpeed = 50.0f;          // カメラの移動速度
private:
	DirectX::XMINT2 lastMouse{};            // 最後に右クリックしたときのマウスの座標
	bool isDragging = false;                // 右クリックしている状態

public:
	void Update(float _deltaTime);
	// マウス右クリック受付
	void UpdateMouseInput();
	// キー操作受付
	void UpdateKeyInput(float _deltaTime);
	// カメラ移動
	void MoveCamera(float _deltaX, float _deltaY, float _deltaZ);
	// カメラ回転（Eye中心軸）
	void RotateCamera(float _pitch, float _yaw);
};