#include "PlayerController.h"
#include "GameManager.h"
#include "xa2.h"
#include "Geometry.h"

using namespace DirectX;

// コンストラクタ
PlayerController::PlayerController(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:Player(_pos, _rot, _scale, _color)
{
	// 加速度を10段階に設定
	accelerations.resize(AccelerationsNum);

	// ドリフトエフェクトの初期サイズ設定
	driftFxScale = _scale;

	Geometry::VertexData vd = Geometry::GetTorusVertexData();
	sonicBoomModel.indexBuffer = vd.indexBuffer;
	sonicBoomModel.vertexBuffer = vd.vertexBuffer;
	sonicBoomModel.numIndex = vd.indexCount;
}

// 更新処理
void PlayerController::Update(float _deltaTime)
{
	// 1フレーム前の座標を格納
	oldPosition = position;

	// キー入力イベント
	HandleKeyEvent(_deltaTime);

	// ヒットストップ時ここでreturn (キー入力のみ受け付ける)
	if (isHitStop) return;

	// モーションブラー専用の過去の座標を格納
	OldPositionsMotionBlurUpdate(position, false);

	// 現在の速度割合を計算
	speedRatio = CalculateSpeedRatio(realSpeed, maxMoveSpeed);

	// 速度割合に応じたプレイヤーの色を設定
	UpdateSpeedRatioColor();

	// ジャンプ処理
	Jump(_deltaTime);

	// 移動処理
	Move(_deltaTime);

	// 回転処理
	RotationPlayer();

	// キー状態解除
	isInputJumpKey = false;
	isInputDriftKey = false;
	analogStick.LX = 0;
	analogStick.LY = 0;
	moveKey.x = 0;
	moveKey.y = 0;
}

// ドリフトエフェクトの描画
void PlayerController::DriftingEffectDraw()
{
	// ドリフト状態でないときreturn
	if (driftState != NOW_DRIFTING) return;

	// ドリフトの経過時間の割合を計算
	float ratio = nowDriftingElapsedTime / maxDriftTime;

	// 動的にスケールの上限値を設定 (ratioが小さいほど上限が高くなる)
	float maxScale = (scaling.x * driftFxScaleRatio) + (1.0f - ratio) * 0.1f; // 基本上限値に加算

	// スケールが動的な上限を超えたら元に戻す
	if (driftFxScale.x >= maxScale || driftFxScale.y >= maxScale || driftFxScale.z >= maxScale)
	{
		driftFxScale = scaling; // スケールをリセット
	}

	// エフェクトのワールド変換行列を計算
	XMMATRIX effectMatrixWorld = CalculateMatrixWorld(position, driftFxScale, rotation);

	// GPUに送信するバッファを設定
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose(effectMatrixWorld);
	ob.color = color;
	ob.color.w = driftFxAlpha;

	// 通常描画
	Renderer::DrawObject(model, ob, std::nullopt, useVS, Shader::PS_COLOR, false, false, false);
}

void PlayerController::SonicBoomDraw()
{
	if (speedRatio < changeColorRatioPhase3) return;

	// ソニックブームの描画
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose((GameObject::CalculateMatrixWorld(sonicBoomPos, sonicBoomScale, sonicBoomRotation)));
	ob.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	Renderer::DrawObject(sonicBoomModel, ob, std::nullopt, Shader::VS_OBJECT, Shader::PS_COLOR, true, false, false);

}

// キー入力イベントを管理する関数
void PlayerController::HandleKeyEvent(float _deltaTime)
{
	// 移動キーの判定
	if (Input::Get().GetKeyPress(DIK_W))  // 前進移動
	{
		moveKey.y += 1.0f;
	}
	if (Input::Get().GetKeyPress(DIK_S))  // 後退移動
	{
		moveKey.y -= 1.0f;
	}
	if (Input::Get().GetKeyPress(DIK_A))  // 右移動
	{
		moveKey.x -= 1.0f;
	}
	if (Input::Get().GetKeyPress(DIK_D))  // 左移動
	{
		moveKey.x += 1.0f;
	}

	// ゲームパッド専用処理
	analogStick = XInputGamePad::Get().GetAnalogStick();


	// ドリフトキーの判定
	if (isGround)
	{
		// 右ドリフト
		if (Input::Get().GetKeyPress(DIK_E) || XInputGamePad::Get().GetR2ButtonPress())
		{
			isInputDriftKey = true;
			driftAngle = DirectX::XMConvertToRadians(-DriftingAngle); // ドリフトの角度
		}
		// 左ドリフト
		else if (Input::Get().GetKeyPress(DIK_Q) || XInputGamePad::Get().GetL2ButtonPress())
		{
			isInputDriftKey = true;
			driftAngle = DirectX::XMConvertToRadians(DriftingAngle);  // ドリフトの角度
		}
	}
	
	// ジャンプキーの判定
	if (jumpState == PlayerController::CAN_JUMP)
	{
		if (Input::Get().GetKeyTrigger(DIK_SPACE) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_X))
		{
			isInputJumpKey = true;
		}
	}
}

// 移動処理
void PlayerController::Move(float _deltaTime)
{
	// 新しい座標に1フレーム前の座標を設定
	newPosition = position;

	// 入力の強度（スティックの傾き、またはキー入力の有無で変化）
	float inputSlope = 0.0f;

	// 進む方向の角度
	float targetAngle = angle;

	// ゲームパッドが接続されている場合の処理
	// スティック操作の角度を計算
	float stickAngle = atan2f(static_cast<float>(analogStick.LX), static_cast<float>(analogStick.LY)) * GameManager::RadToDeg;

	// 傾きを設定
	inputSlope = std::max(fabsf(analogStick.LX), fabsf(analogStick.LY));

	// 角度が負の値にならないよう補正
	if (stickAngle < 0)
	{
		stickAngle += GameManager::FullRotationDegress;
	}

	// スティック入力がある場合、向きをスティック方向に変更
	if (analogStick.LX != 0 || analogStick.LY != 0)
	{
		// カメラの方向に対してスティックの角度を加算する
		targetAngle = dirAngle + D2R(stickAngle);
	}

	// キーボード操作の場合
	if (moveKey.x != 0.0f || moveKey.y != 0.0f)
	{
		// カメラの方向に対してキー入力の方向を加算
		targetAngle = dirAngle + std::atan2(moveKey.x, moveKey.y);

		// キー操作の場合常に強度は1.0に設定
		inputSlope = 1.0f;
	}

	// プレイヤーの回転をスムーズに補間（急旋回を防ぐ）
	float turnRate = minTurnRate + (1.0f - (moveSpeed / maxMoveSpeed)) * (maxTurnRate - minTurnRate);
	turnRate *= _deltaTime;  // フレームごとの補正

	float angleDiff = targetAngle - angle;

	// 角度の差を -π ～ π の範囲に調整
	if (angleDiff > DirectX::XM_PI)
	{
		angleDiff -= DirectX::XM_2PI;
	}
	if (angleDiff < -DirectX::XM_PI)
	{
		angleDiff += DirectX::XM_2PI;
	}

	// スムーズな補間
	if (fabsf(angleDiff) > turnRate)
	{
		angle += (angleDiff > 0 ? turnRate : -turnRate);
	}
	else
	{
		angle = targetAngle; // 目標角度に一致
	}

	// 角度を正規化
	if (angleDiff > DirectX::XM_PI)
	{
		angleDiff -= DirectX::XM_2PI;  // 差が180度を超えたら-360度補正
	}
	else if (angleDiff < -DirectX::XM_PI)
	{
		angleDiff += DirectX::XM_2PI;  // 差が-180度未満なら+360度補正
	}

	// 移動速度の制御
	float targetSpeed = inputSlope * maxMoveSpeed; // 入力の傾きに応じた速度
	float accelerationSpeedRatio = CalculateSpeedRatio(moveSpeed, maxMoveSpeed);
	accelerationsIndex = static_cast<int>(accelerationSpeedRatio * 10.0f);
	accelerationsIndex = std::clamp(accelerationsIndex, 0, static_cast<int>(accelerations.size() - 1));

	// 加速処理
	if (moveSpeed + driftingAddSpeed < targetSpeed)
	{
		moveSpeed += accelerations[accelerationsIndex] * _deltaTime;
		moveSpeed = std::clamp(moveSpeed, 0.0f, maxMoveSpeed);

	}
	// 減速処理
	else if (moveSpeed > targetSpeed)
	{
		deceleration = CalculateDeceleration(frictionFactor, moveSpeed);
		moveSpeed -= std::max(deceleration, MinDeceleration) * _deltaTime;
		// 速度が0より下がらないようにする
		moveSpeed = std::max(moveSpeed, 0.0f);
	}
	
	// 移動方向の計算
	XMFLOAT3 moveDirection;
	moveDirection.x = sin(angle);
	moveDirection.y = 0;
	moveDirection.z = cos(angle);

	// 重力の影響を適用
	gravityDot = (collisionNormal.y > 0.0f) ? (collisionNormal.y * GameManager::GravityScale * mass) : (GameManager::GravityScale * mass);

	// ドリフト開始判定
	if (isInputDriftKey && driftState == NO_DRIFTING && moveSpeed > driftPossibleSpeed)
	{
		isDrifting = true;
		driftState = NOW_DRIFTING;
		nowDriftingElapsedTime = 0.0f;
		preDriftSpeed = moveSpeed;
		driftDirection = moveDirection;
		driftTargetAngle = dirAngle;
	}

	// ドリフト中の処理
	if (driftState == NOW_DRIFTING)
	{
		// 効果音が再生中か確認
		if (!XA_IsPlaying(CHARGE_SE))
		{
			// SE（効果音）の再生
			XA_Play(CHARGE_SE, ChargeSEVol * volRatio);
			XA_SetSpeed(CHARGE_SE, ChargeSESpeed);
		}

		// 入力に応じて次のターゲット方向を更新
		driftTargetAngle = dirAngle;
		angle = driftTargetAngle;

		// ドリフトの横方向移動計算
		XMFLOAT3 lateralDrift;
		lateralDrift.x = driftDirection.x * cos(driftAngle) - driftDirection.z * sin(driftAngle);
		lateralDrift.z = driftDirection.x * sin(driftAngle) + driftDirection.z * cos(driftAngle);
		lateralDrift.y = 0;
		moveDirection = lateralDrift;

		// 現在の速度を計算（ドリフト時は速度を落とすために摩擦係数を掛ける）
		realSpeed = preDriftSpeed * driftSpeedFactor;

		// ドリフト時間を加算
		nowDriftingElapsedTime += _deltaTime;
		nowDriftingElapsedTime = std::min(nowDriftingElapsedTime, maxDriftTime);

		// ドリフト解除時の処理
		if (!isInputDriftKey || nowDriftingElapsedTime == maxDriftTime)
		{
			// ドリフト後の状態に切り替え
			driftState = AFTER;

			// SEの再生
			XA_Play(DASH_SE, DashSEVol* volRatio);

			// SEの停止
			XA_Stop(CHARGE_SE);

			// ドリフト後の時間計測を開始
			afterDriftingElapsedTime = 0.0f;

			// ドリフトの加速割合を計算
			float driftingRatio = nowDriftingElapsedTime / maxDriftTime;

			// ドリフト後の最大加速度を計算
			driftingAddSpeed = maxDriftAddSpeed * driftingRatio;

			if (moveSpeed > maxMoveSpeed - driftingAddSpeed)
			{
				driftingAddSpeed = std::min(driftingAddSpeed, maxMoveSpeed - moveSpeed);
			}

			// ドリフト後の最初の速度を設定
			afterDriftingSpeed = realSpeed;

		}

		// ドリフトエフェクトのスケールを増加させる処理
		if (isDriftFxPlay)
		{
			driftFxScale.x += driftFxAddScaleSpeed;
			driftFxScale.y += driftFxAddScaleSpeed;
			driftFxScale.z += driftFxAddScaleSpeed;
		}
	}
	// ドリフト後の処理
	else if (driftState == AFTER)
	{
		
		// 経過時間を加算
		afterDriftingElapsedTime += _deltaTime;

		// 一定時間内に元のスピードに戻す
		float progress = std::min(afterDriftingElapsedTime / returnTime, 1.0f); // 0.0～1.0で進行割合を計算

		// 行割合に基づいてmoveSpeedを元の速度に戻す
		moveSpeed = afterDriftingSpeed + (preDriftSpeed - afterDriftingSpeed) * progress;

		// もし戻るべき速度に達したら
		if (afterDriftingElapsedTime >= returnTime)
		{
			moveSpeed = preDriftSpeed;  // 完全に戻す
			driftState = NO_DRIFTING;   // ドリフト終了

			// ドリフト終了後の加速時間を設定
			maxAddSpeedTime = maxDriftAddSpeedTime;
		}

		realSpeed = moveSpeed + driftingAddSpeed;
	}
	// ドリフト状態ではない通常の状態
	else
	{
		// 加速度の更新
		realSpeed = moveSpeed + driftingAddSpeed;

		if (maxAddSpeedTime > 0.0f)
		{
			maxAddSpeedTime -= _deltaTime;
		}
		else
		{
			if (driftingAddSpeed > 0)
			{
				// 減速値を計算し、最低減速値を保証
				float driftDeceleration = std::max(CalculateDeceleration(driftFrictionFactor, realSpeed), MinDeceleration);

				// 減速を適用し、0未満にならないように制限
				driftingAddSpeed = std::max(driftingAddSpeed - driftDeceleration * _deltaTime, 0.0f);
			}
		}
	}

	// 壁衝突時の減速処理
	if (isWallHit && speedRatio > WallDecelerationStartRatio)
	{
		// 壁衝突時の減速度を計算
		float wallDeceleration = CalculateDeceleration(wallFrictionFactor, moveSpeed);

		// ドリフト時はpreDriftSpeedを減速させる
		float& targetSpeed = (driftState == NOW_DRIFTING) ? preDriftSpeed : moveSpeed;

		// 減速
		targetSpeed -= wallDeceleration * _deltaTime;
	}

	// 速度が0より大きい場合にSEを再生する
	if (realSpeed > 0.0f)
	{
		// SEが再生中でないときSEを再生する
		if (!XA_IsPlaying(MOVE_SE)) XA_Play(MOVE_SE, MoveSEVol * volRatio);
	
		// SEの再生速度を速度割合に応じて変化させる
		XA_SetSpeed(MOVE_SE, MoveSESpeed * (1.0f + speedRatio));

		// 速度割合に応じて音量も上げる
		XA_SetVolume(MOVE_SE, MoveSEVol* speedRatio);
	}
	else
	{
		// SE停止
		XA_Stop(MOVE_SE);
	}

	// 速度を適用して位置を更新
	velocity.x = moveDirection.x * realSpeed;
	velocity.y += gravityDot * _deltaTime;
	velocity.z = moveDirection.z * realSpeed;

	// まずは仮の座標として計算して
	// 衝突判定時に正式にpositionに反映させる
	newPosition.x += velocity.x * _deltaTime;
	newPosition.y += velocity.y * _deltaTime;
	newPosition.z += velocity.z * _deltaTime;

	// ソニックブームの更新
	if (speedRatio > changeColorRatioPhase3)
	{
		if (sonicBoomElapsedCount == 0.0f)
		{
			sonicBoomPos = newPosition;
			sonicBoomRotation = XMFLOAT3(90.0f, rotation.y, 0.0f);
		}

		float ratioValue = (speedRatio - changeColorRatioPhase3) / (1.0 - changeColorRatioPhase3);

		sonicBoomElapsedCount += SonicBoomMaxSpeed * ratioValue;
		if (sonicBoomElapsedCount >= SonicBoomMaxScale * ratioValue)  sonicBoomElapsedCount = 0;

		// ソニックブームのスケール
		float sbScale = SonicBoomBaseScale * sonicBoomElapsedCount;
		sonicBoomScale = XMFLOAT3(sbScale, 1.0f, sbScale);
	}

	// 回転を適用
	rotation.y = XMConvertToDegrees(angle);
}

// ジャンプ処理
void PlayerController::Jump(float _deltaTime)
{
	switch (jumpState)
	{
	case PlayerController::CAN_JUMP:   // ジャンプ可能

		// ジャンプキーが押された場合
		if (isInputJumpKey)
		{
			// SE再生
			XA_Play(JUMP_SE, JumpSEVol * volRatio);

			// Y方向の速度を設定
			velocity.y = jumpPower;

			// 状態をジャンプ中に切り替え
			jumpState = PlayerController::JUMPING;
		}
		break;
	case PlayerController::JUMPING:   // ジャンプ中

		// ジャンプの高さが最大になったら
		if (velocity.y <= 0)
		{
			// 状態を落下に切り替え
			jumpState = PlayerController::FALLING;
		}
		break;
	case PlayerController::FALLING:   // 落下中
		// 落下中は特別な処理はなし
		break;
	default:
		break;
	}
}

// 減速度を計算
float PlayerController::CalculateDeceleration(float frictionFactor, float _moveSpeed)
{
	// 速度が高いほど減速率が上昇する
	return frictionFactor * _moveSpeed;
}

// 速度の現在の割合を計算
float PlayerController::CalculateSpeedRatio(float _realSpeed, float _maxSpeed)
{
	// 範囲を超えないように調整
	return std::clamp(_realSpeed / _maxSpeed, 0.0f, 1.0f);
}

// コースアウト後の設定
void PlayerController::HoleAfterSet()
{
	moveSpeed *= holeDecelerationRate;
	driftingAddSpeed *= holeDecelerationRate;
	velocity.y = 0;
}

// 速度割合に応じたプレイヤーの色を更新
void PlayerController::UpdateSpeedRatioColor()
{
	// Phase2までの色
	if (speedRatio < changeColorRatioPhase2)
	{
		SetRGBColor(speedRatioColor1);
	}
	// Phase3までの色
	else if (speedRatio < changeColorRatioPhase3)
	{
		SetRGBColor(speedRatioColor2);
	}
	// Phase4までの色
	else if (speedRatio < changeColorRatioPhase4)
	{
		SetRGBColor(speedRatioColor3);
	}
	// Phase4の色
	else
	{
		SetRGBColor(speedRatioColor4);
	}
}

// 各割合数を設定
void PlayerController::SetSpeedRatioColorPhase(float phase1, float phase2, float phase3, float phase4)
{
	changeColorRatioPhase1 = phase1;
	changeColorRatioPhase2 = phase2;
	changeColorRatioPhase3 = phase3;
	changeColorRatioPhase4 = phase4;
}

// 各割合の色を設定
void PlayerController::SetSpeedRatioColor(
	DirectX::XMFLOAT3 colorPhase1,
	DirectX::XMFLOAT3 colorPhase2, 
	DirectX::XMFLOAT3 colorPhase3,
	DirectX::XMFLOAT3 colorPhase4)
{
	speedRatioColor1 = colorPhase1;
	speedRatioColor2 = colorPhase2;
	speedRatioColor3 = colorPhase3;
	speedRatioColor4 = colorPhase4;
}

// 加速度を設定
void PlayerController::SetAccelerations(std::vector<float> accelers)
{
	accelerations = accelers;

	if (accelerations.size() != AccelerationsNum)
	{
		accelerations.resize(AccelerationsNum);
	}
}
