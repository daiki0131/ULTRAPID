#include "Camera.h"
#include "Input.h"
#include "XInputGamePad.h"
#include "DebugUI.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "GameManager.h"

using namespace DirectX;

// 定数定義
const float Camera3D::MaxAdjustFocus = 15.0f;
const float Camera3D::MinEyeZ = -50.0f;
const float GameSceneCamera::MaxFov = 130.0f;
const float GameSceneCamera::MinFov = 45.0f;
const float GameSceneCamera::MaxShakeIntensity = 0.003f;

// ビュー行列計算関数
void Camera::CalculateViewMatrix(DirectX::XMFLOAT3 _eye, DirectX::XMFLOAT3 _focus, DirectX::XMFLOAT3 _up)
{
	XMVECTOR vEye = XMVectorSet(_eye.x, _eye.y, _eye.z, 0);         // 位置
	XMVECTOR vFocus = XMVectorSet(_focus.x, _focus.y, _focus.z, 0); // 注視点
	XMVECTOR vUp = XMVectorSet(_up.x, _up.y, _up.z, 0);             // 上方向ベクトル

	// ビュー行列計算
	matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);
}

// ビュー・プロジェクション行列を返す関数
DirectX::XMMATRIX Camera::GetViewProjectionMatrix()
{
	return XMMatrixMultiply(matrixView, GetProjectionMatrix());
}


// ======== Camera2D ========= //
// 更新処理
void Camera2D::Update(const float _deltaTime)
{
	CalculateViewMatrix(eye, focus, up);
}

// 2Dカメラの射影行列を返す関数
DirectX::XMMATRIX Camera2D::GetProjectionMatrix()
{
	return  XMMatrixOrthographicLH(16.0f * 0.3f, 9.0f * 0.3f, 0.0f, 3.0f);
}

// ======== Camera3D ========= //
// 3Dカメラの射影行列を返す関数
DirectX::XMMATRIX Camera3D::GetProjectionMatrix()
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(fovAngle), 16.0f / 9.0f, 0.01f, 3000.0f);
}

// ======== TitleSceneCamera ========= //
// 更新処理
void TitleSceneCamera::Update(const float _deltaTime, TitleScene& titleScene)
{
	focus = titleScene.GetPlayer().GetPosition(); // ターゲット設定
	
	// ターゲットの座標を調整
	focus.x += adjustFocus.x;
	focus.y += adjustFocus.y;

	// Eyeを設定（X・Y軸移動）
	eye = XMFLOAT3(focus.x, focus.y, eye.z);
	
	CalculateViewMatrix(eye, focus, up);
}

// ======== GameSceneCamera ========= //
void GameSceneCamera::Update(const float _deltaTime, GameScene& gameScene)
{
	focus = gameScene.GetPlayer().GetPosition(); // ターゲット設定

	// ヒットストップ時ではないときに
	// 調整したEyeの座標を設定
	if (!isHitStop)
	{
		eye.y = focus.y + adjustEye.y;
		eye.z = focus.z + adjustEye.z;
	}

	// ターゲットの座標を調整
	focus.y += adjustFocus.y;

	// カメラが追従可能状態の場合
	if (isChaseCamera)
	{
		// 現在と前のプレイヤーの座標を取得
		nowPlayerPos = gameScene.GetPlayer().GetPosition();
		oldPlayerPos = gameScene.GetPlayer().GetOldPosition();
 		
		// XMFLOAT3からXMVECTORに変換する
		XMVECTOR nowPlayerPosVec = XMLoadFloat3(&nowPlayerPos);
		XMVECTOR oldPlayerPosVec = XMLoadFloat3(&oldPlayerPos);

		// カメラを移動させるためのベクトルを計算
		XMVECTOR moveVec = nowPlayerPosVec - oldPlayerPosVec;

		XMFLOAT3 cameraEye = eye;
		// 移動後のカメラの位置を表す
		XMVECTOR eyeVec = XMLoadFloat3(&cameraEye);
		eyeVec = eyeVec + moveVec;

		// XMVECTORからXMFLOAT3に変換する
		XMStoreFloat3(&eye, eyeVec);

		// XMFLOAT3からXMVECTORに変換する
		XMVECTOR vEye = XMVectorSet(eye.x, eye.y, eye.z, 0);
		XMVECTOR vFocus = XMVectorSet(focus.x, focus.y, focus.z, 0);
		XMVECTOR vUp = XMVectorSet(up.x, up.y, up.z, 0);

		// ヒットストップ時のカメラの揺れを実行
		UpdateCameraShake(_deltaTime);
		XMFLOAT3 shakeEye = ApplyCameraShake(eye);

		// プレイヤーによるカメラ操作
		if (isInputCamera)
		{
			// ゲームパッドのスティックの傾きを取得
			XInputGamePad::AnalogStick analogStick = XInputGamePad::Get().GetAnalogStick();

			// 入力の傾き
			float inputSlope = analogStick.RX;

			if (Input::Get().GetKeyPress(DIK_LEFTARROW))
			{
				inputSlope = -1.0f;
			}
			if (Input::Get().GetKeyPress(DIK_RIGHTARROW))
			{
				inputSlope = 1.0f;
			}

			revoAngle += revoAngleSpeed * inputSlope;
		}
		// 回転軸を表すベクトル
		XMVECTOR axis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		// 回転軸と角度に基づいてクォータニオンを計算
		XMVECTOR quaternion = XMQuaternionRotationAxis(axis, revoAngle);

		// キャラクターからの相対位置
		XMVECTOR relativePos = XMVectorSet(shakeEye.x, shakeEye.y, shakeEye.z, 0) - vFocus;
		relativePos = XMVector3Rotate(relativePos, quaternion);
		// 回転後のカメラ位置
		vEye = relativePos + vFocus;

		XMStoreFloat3(&afterEye, vEye);
		matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);
	}
	else
	{
		CalculateViewMatrix(afterEye, focus, up);
	}
	
}

// ヒットストップ時のカメラの揺れを実行する関数
void GameSceneCamera::StartCameraShake(float _hitStopTime, float intensityRatio)
{
	shakeIntensity = maxShakeIntensity * (1.0f - intensityRatio); // 揺れを設定
	shakeDuration = _hitStopTime;       // ヒットストップの時間を設定
	shakeElapsedTime = 0;               // 経過時間を0リセット
}

// カメラの揺れを反映する関数
DirectX::XMFLOAT3 GameSceneCamera::ApplyCameraShake(DirectX::XMFLOAT3 _eye)
{
	// 揺れの強さが0以上の場合
	if (shakeIntensity > 0.0f)
	{
		std::uniform_real_distribution<float> rand(-100, 99);  // 揺れの方向のランダム設定

		// ランダムに揺れるオフセット
		float shakeX = rand(GameManager::mt) * shakeIntensity;
		float shakeY = rand(GameManager::mt) * shakeIntensity;
		float shakeZ = rand(GameManager::mt) * shakeIntensity;

		// 揺れのオフセットをカメラ位置に加算
		_eye.x += shakeX;
		_eye.y += shakeY;
		_eye.z += shakeZ;
	}

	return _eye;
}

// カメラの揺れを更新
void GameSceneCamera::UpdateCameraShake(const float _deltaTime)
{
	// 揺れが継続中ならば
	if (shakeElapsedTime < shakeDuration)
	{
		shakeElapsedTime += _deltaTime;
		// 時間の経過に基づいて揺れ強度を減少させる
		float t = shakeElapsedTime / shakeDuration;      // 経過時間の割合 (0.0 ～ 1.0)
		t = std::clamp(t, 0.0f, 1.0f);

		shakeIntensity = maxShakeIntensity * (1.0f - t); // 線形補間で徐々に減衰
	}
	else
	{
		shakeIntensity = 0.0f; // 揺れが終了したら強さを0に
	}
}

// ======== DebugCamera ========= //
// 更新処理
void DebugCamera::Update(float _deltaTime)
{
	UpdateMouseInput();                  // マウス取得
	UpdateKeyInput(_deltaTime);          // キーインプット取得
	CalculateViewMatrix(eye, focus, up);
}

// カメラの移動処理
void DebugCamera::MoveCamera(float _deltaX, float _deltaY, float _deltaZ)
{
	XMVECTOR EyeVec = XMLoadFloat3(&eye);
	XMVECTOR TargetVec = XMLoadFloat3(&focus);
	XMVECTOR UpVec = XMLoadFloat3(&up);

	// 前方向ベクトル
	XMVECTOR Look = XMVector3Normalize(TargetVec - EyeVec);
	// 右方向ベクトル
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(UpVec, Look));

	// 移動処理
	EyeVec += Right * _deltaX;    // A/Dキー（左右移動）
	EyeVec += UpVec * _deltaY;    // Q/Eキー（上昇・下降）
	EyeVec += Look * _deltaZ;     // W/Sキー（前進・後退）
	TargetVec += Right * _deltaX;
	TargetVec += UpVec * _deltaY;
	TargetVec += Look * _deltaZ;

	// 新しい位置を適用
	XMStoreFloat3(&eye, EyeVec);
	XMStoreFloat3(&focus, TargetVec);
}

// マウスのクリックを取得する関数
void DebugCamera::UpdateMouseInput()
{
	POINT mousePos;
	GetCursorPos(&mousePos); // マウスのスクリーン上の位置を取得

	// マウスの右クリックが押されたら
	if (Input::Get().GetMousePress(1))
	{
		// 最初にマウスがクリックされたときのみ通す
		if (!isDragging)
		{
			lastMouse.x = mousePos.x;
			lastMouse.y = mousePos.y;
			isDragging = true;
		}
		float dx = (mousePos.x - lastMouse.x) * MouseSensitivity; // 横方向の移動量計算
		float dy = (mousePos.y - lastMouse.y) * MouseSensitivity; // 縦方向の移動量計算

		RotateCamera(dy, dx); // 回転処理実行

		// マウスの現在位置を保存
		lastMouse.x = mousePos.x;
		lastMouse.y = mousePos.y;
	}
	else
	{
		isDragging = false;
	}
}

// キー操作を取得する関数
void DebugCamera::UpdateKeyInput(float _deltaTime)
{
	// 右クリックが押されてないときはreturn
	if (!isDragging) return;

	float realSpeed = MoveSpeed * _deltaTime; // カメラの移動速度

	if (Input::Get().GetKeyPress(DIK_W)) MoveCamera(0, 0, realSpeed);  // 右クリック＋Wキー
	if (Input::Get().GetKeyPress(DIK_S)) MoveCamera(0, 0, -realSpeed); // 右クリック＋Sキー
	if (Input::Get().GetKeyPress(DIK_A)) MoveCamera(-realSpeed, 0, 0); // 右クリック＋Aキー
	if (Input::Get().GetKeyPress(DIK_D)) MoveCamera(realSpeed, 0, 0);  // 右クリック＋Dキー
	if (Input::Get().GetKeyPress(DIK_Q)) MoveCamera(0, realSpeed, 0);  // 右クリック＋Qキー
	if (Input::Get().GetKeyPress(DIK_E)) MoveCamera(0, -realSpeed, 0); // 右クリック＋Eキー
}

// カメラをEyeを軸に回転させる
void DebugCamera::RotateCamera(float _pitch, float _yaw)
{

	XMVECTOR EyeVec = XMLoadFloat3(&eye);
	XMVECTOR TargetVec = XMLoadFloat3(&focus);
	XMVECTOR UpVec = XMLoadFloat3(&up);

	// カメラの向いている方向を計算
	XMVECTOR Look = XMVector3Normalize(TargetVec - EyeVec);

	// 左右回転
	XMMATRIX yawMatrix = XMMatrixRotationAxis(UpVec, _yaw);
	Look = XMVector3TransformNormal(Look, yawMatrix);

	// 上下回転
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(UpVec, Look));
	XMMATRIX pitchMatrix = XMMatrixRotationAxis(Right, _pitch);
	Look = XMVector3TransformNormal(Look, pitchMatrix);

	TargetVec = EyeVec + Look;

	// 回転適用
	XMStoreFloat3(&focus, TargetVec);
}