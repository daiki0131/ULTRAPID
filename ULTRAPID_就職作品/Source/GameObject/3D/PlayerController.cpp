#include "PlayerController.h"
#include "GameManager.h"
#include "xa2.h"
#include "Geometry.h"

using namespace DirectX;

// �R���X�g���N�^
PlayerController::PlayerController(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:Player(_pos, _rot, _scale, _color)
{
	// �����x��10�i�K�ɐݒ�
	accelerations.resize(AccelerationsNum);

	// �h���t�g�G�t�F�N�g�̏����T�C�Y�ݒ�
	driftFxScale = _scale;

	Geometry::VertexData vd = Geometry::GetTorusVertexData();
	sonicBoomModel.indexBuffer = vd.indexBuffer;
	sonicBoomModel.vertexBuffer = vd.vertexBuffer;
	sonicBoomModel.numIndex = vd.indexCount;
}

// �X�V����
void PlayerController::Update(float _deltaTime)
{
	// 1�t���[���O�̍��W���i�[
	oldPosition = position;

	// �L�[���̓C�x���g
	HandleKeyEvent(_deltaTime);

	// �q�b�g�X�g�b�v��������return (�L�[���͂̂ݎ󂯕t����)
	if (isHitStop) return;

	// ���[�V�����u���[��p�̉ߋ��̍��W���i�[
	OldPositionsMotionBlurUpdate(position, false);

	// ���݂̑��x�������v�Z
	speedRatio = CalculateSpeedRatio(realSpeed, maxMoveSpeed);

	// ���x�����ɉ������v���C���[�̐F��ݒ�
	UpdateSpeedRatioColor();

	// �W�����v����
	Jump(_deltaTime);

	// �ړ�����
	Move(_deltaTime);

	// ��]����
	RotationPlayer();

	// �L�[��ԉ���
	isInputJumpKey = false;
	isInputDriftKey = false;
	analogStick.LX = 0;
	analogStick.LY = 0;
	moveKey.x = 0;
	moveKey.y = 0;
}

// �h���t�g�G�t�F�N�g�̕`��
void PlayerController::DriftingEffectDraw()
{
	// �h���t�g��ԂłȂ��Ƃ�return
	if (driftState != NOW_DRIFTING) return;

	// �h���t�g�̌o�ߎ��Ԃ̊������v�Z
	float ratio = nowDriftingElapsedTime / maxDriftTime;

	// ���I�ɃX�P�[���̏���l��ݒ� (ratio���������قǏ���������Ȃ�)
	float maxScale = (scaling.x * driftFxScaleRatio) + (1.0f - ratio) * 0.1f; // ��{����l�ɉ��Z

	// �X�P�[�������I�ȏ���𒴂����猳�ɖ߂�
	if (driftFxScale.x >= maxScale || driftFxScale.y >= maxScale || driftFxScale.z >= maxScale)
	{
		driftFxScale = scaling; // �X�P�[�������Z�b�g
	}

	// �G�t�F�N�g�̃��[���h�ϊ��s����v�Z
	XMMATRIX effectMatrixWorld = CalculateMatrixWorld(position, driftFxScale, rotation);

	// GPU�ɑ��M����o�b�t�@��ݒ�
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose(effectMatrixWorld);
	ob.color = color;
	ob.color.w = driftFxAlpha;

	// �ʏ�`��
	Renderer::DrawObject(model, ob, std::nullopt, useVS, Shader::PS_COLOR, false, false, false);
}

void PlayerController::SonicBoomDraw()
{
	if (speedRatio < changeColorRatioPhase3) return;

	// �\�j�b�N�u�[���̕`��
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose((GameObject::CalculateMatrixWorld(sonicBoomPos, sonicBoomScale, sonicBoomRotation)));
	ob.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	Renderer::DrawObject(sonicBoomModel, ob, std::nullopt, Shader::VS_OBJECT, Shader::PS_COLOR, true, false, false);

}

// �L�[���̓C�x���g���Ǘ�����֐�
void PlayerController::HandleKeyEvent(float _deltaTime)
{
	// �ړ��L�[�̔���
	if (Input::Get().GetKeyPress(DIK_W))  // �O�i�ړ�
	{
		moveKey.y += 1.0f;
	}
	if (Input::Get().GetKeyPress(DIK_S))  // ��ވړ�
	{
		moveKey.y -= 1.0f;
	}
	if (Input::Get().GetKeyPress(DIK_A))  // �E�ړ�
	{
		moveKey.x -= 1.0f;
	}
	if (Input::Get().GetKeyPress(DIK_D))  // ���ړ�
	{
		moveKey.x += 1.0f;
	}

	// �Q�[���p�b�h��p����
	analogStick = XInputGamePad::Get().GetAnalogStick();


	// �h���t�g�L�[�̔���
	if (isGround)
	{
		// �E�h���t�g
		if (Input::Get().GetKeyPress(DIK_E) || XInputGamePad::Get().GetR2ButtonPress())
		{
			isInputDriftKey = true;
			driftAngle = DirectX::XMConvertToRadians(-DriftingAngle); // �h���t�g�̊p�x
		}
		// ���h���t�g
		else if (Input::Get().GetKeyPress(DIK_Q) || XInputGamePad::Get().GetL2ButtonPress())
		{
			isInputDriftKey = true;
			driftAngle = DirectX::XMConvertToRadians(DriftingAngle);  // �h���t�g�̊p�x
		}
	}
	
	// �W�����v�L�[�̔���
	if (jumpState == PlayerController::CAN_JUMP)
	{
		if (Input::Get().GetKeyTrigger(DIK_SPACE) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_X))
		{
			isInputJumpKey = true;
		}
	}
}

// �ړ�����
void PlayerController::Move(float _deltaTime)
{
	// �V�������W��1�t���[���O�̍��W��ݒ�
	newPosition = position;

	// ���͂̋��x�i�X�e�B�b�N�̌X���A�܂��̓L�[���̗͂L���ŕω��j
	float inputSlope = 0.0f;

	// �i�ޕ����̊p�x
	float targetAngle = angle;

	// �Q�[���p�b�h���ڑ�����Ă���ꍇ�̏���
	// �X�e�B�b�N����̊p�x���v�Z
	float stickAngle = atan2f(static_cast<float>(analogStick.LX), static_cast<float>(analogStick.LY)) * GameManager::RadToDeg;

	// �X����ݒ�
	inputSlope = std::max(fabsf(analogStick.LX), fabsf(analogStick.LY));

	// �p�x�����̒l�ɂȂ�Ȃ��悤�␳
	if (stickAngle < 0)
	{
		stickAngle += GameManager::FullRotationDegress;
	}

	// �X�e�B�b�N���͂�����ꍇ�A�������X�e�B�b�N�����ɕύX
	if (analogStick.LX != 0 || analogStick.LY != 0)
	{
		// �J�����̕����ɑ΂��ăX�e�B�b�N�̊p�x�����Z����
		targetAngle = dirAngle + D2R(stickAngle);
	}

	// �L�[�{�[�h����̏ꍇ
	if (moveKey.x != 0.0f || moveKey.y != 0.0f)
	{
		// �J�����̕����ɑ΂��ăL�[���͂̕��������Z
		targetAngle = dirAngle + std::atan2(moveKey.x, moveKey.y);

		// �L�[����̏ꍇ��ɋ��x��1.0�ɐݒ�
		inputSlope = 1.0f;
	}

	// �v���C���[�̉�]���X���[�Y�ɕ�ԁi�}�����h���j
	float turnRate = minTurnRate + (1.0f - (moveSpeed / maxMoveSpeed)) * (maxTurnRate - minTurnRate);
	turnRate *= _deltaTime;  // �t���[�����Ƃ̕␳

	float angleDiff = targetAngle - angle;

	// �p�x�̍��� -�� �` �� �͈̔͂ɒ���
	if (angleDiff > DirectX::XM_PI)
	{
		angleDiff -= DirectX::XM_2PI;
	}
	if (angleDiff < -DirectX::XM_PI)
	{
		angleDiff += DirectX::XM_2PI;
	}

	// �X���[�Y�ȕ��
	if (fabsf(angleDiff) > turnRate)
	{
		angle += (angleDiff > 0 ? turnRate : -turnRate);
	}
	else
	{
		angle = targetAngle; // �ڕW�p�x�Ɉ�v
	}

	// �p�x�𐳋K��
	if (angleDiff > DirectX::XM_PI)
	{
		angleDiff -= DirectX::XM_2PI;  // ����180�x�𒴂�����-360�x�␳
	}
	else if (angleDiff < -DirectX::XM_PI)
	{
		angleDiff += DirectX::XM_2PI;  // ����-180�x�����Ȃ�+360�x�␳
	}

	// �ړ����x�̐���
	float targetSpeed = inputSlope * maxMoveSpeed; // ���͂̌X���ɉ��������x
	float accelerationSpeedRatio = CalculateSpeedRatio(moveSpeed, maxMoveSpeed);
	accelerationsIndex = static_cast<int>(accelerationSpeedRatio * 10.0f);
	accelerationsIndex = std::clamp(accelerationsIndex, 0, static_cast<int>(accelerations.size() - 1));

	// ��������
	if (moveSpeed + driftingAddSpeed < targetSpeed)
	{
		moveSpeed += accelerations[accelerationsIndex] * _deltaTime;
		moveSpeed = std::clamp(moveSpeed, 0.0f, maxMoveSpeed);

	}
	// ��������
	else if (moveSpeed > targetSpeed)
	{
		deceleration = CalculateDeceleration(frictionFactor, moveSpeed);
		moveSpeed -= std::max(deceleration, MinDeceleration) * _deltaTime;
		// ���x��0��艺����Ȃ��悤�ɂ���
		moveSpeed = std::max(moveSpeed, 0.0f);
	}
	
	// �ړ������̌v�Z
	XMFLOAT3 moveDirection;
	moveDirection.x = sin(angle);
	moveDirection.y = 0;
	moveDirection.z = cos(angle);

	// �d�͂̉e����K�p
	gravityDot = (collisionNormal.y > 0.0f) ? (collisionNormal.y * GameManager::GravityScale * mass) : (GameManager::GravityScale * mass);

	// �h���t�g�J�n����
	if (isInputDriftKey && driftState == NO_DRIFTING && moveSpeed > driftPossibleSpeed)
	{
		isDrifting = true;
		driftState = NOW_DRIFTING;
		nowDriftingElapsedTime = 0.0f;
		preDriftSpeed = moveSpeed;
		driftDirection = moveDirection;
		driftTargetAngle = dirAngle;
	}

	// �h���t�g���̏���
	if (driftState == NOW_DRIFTING)
	{
		// ���ʉ����Đ������m�F
		if (!XA_IsPlaying(CHARGE_SE))
		{
			// SE�i���ʉ��j�̍Đ�
			XA_Play(CHARGE_SE, ChargeSEVol * volRatio);
			XA_SetSpeed(CHARGE_SE, ChargeSESpeed);
		}

		// ���͂ɉ����Ď��̃^�[�Q�b�g�������X�V
		driftTargetAngle = dirAngle;
		angle = driftTargetAngle;

		// �h���t�g�̉������ړ��v�Z
		XMFLOAT3 lateralDrift;
		lateralDrift.x = driftDirection.x * cos(driftAngle) - driftDirection.z * sin(driftAngle);
		lateralDrift.z = driftDirection.x * sin(driftAngle) + driftDirection.z * cos(driftAngle);
		lateralDrift.y = 0;
		moveDirection = lateralDrift;

		// ���݂̑��x���v�Z�i�h���t�g���͑��x�𗎂Ƃ����߂ɖ��C�W�����|����j
		realSpeed = preDriftSpeed * driftSpeedFactor;

		// �h���t�g���Ԃ����Z
		nowDriftingElapsedTime += _deltaTime;
		nowDriftingElapsedTime = std::min(nowDriftingElapsedTime, maxDriftTime);

		// �h���t�g�������̏���
		if (!isInputDriftKey || nowDriftingElapsedTime == maxDriftTime)
		{
			// �h���t�g��̏�Ԃɐ؂�ւ�
			driftState = AFTER;

			// SE�̍Đ�
			XA_Play(DASH_SE, DashSEVol* volRatio);

			// SE�̒�~
			XA_Stop(CHARGE_SE);

			// �h���t�g��̎��Ԍv�����J�n
			afterDriftingElapsedTime = 0.0f;

			// �h���t�g�̉����������v�Z
			float driftingRatio = nowDriftingElapsedTime / maxDriftTime;

			// �h���t�g��̍ő�����x���v�Z
			driftingAddSpeed = maxDriftAddSpeed * driftingRatio;

			if (moveSpeed > maxMoveSpeed - driftingAddSpeed)
			{
				driftingAddSpeed = std::min(driftingAddSpeed, maxMoveSpeed - moveSpeed);
			}

			// �h���t�g��̍ŏ��̑��x��ݒ�
			afterDriftingSpeed = realSpeed;

		}

		// �h���t�g�G�t�F�N�g�̃X�P�[���𑝉������鏈��
		if (isDriftFxPlay)
		{
			driftFxScale.x += driftFxAddScaleSpeed;
			driftFxScale.y += driftFxAddScaleSpeed;
			driftFxScale.z += driftFxAddScaleSpeed;
		}
	}
	// �h���t�g��̏���
	else if (driftState == AFTER)
	{
		
		// �o�ߎ��Ԃ����Z
		afterDriftingElapsedTime += _deltaTime;

		// ��莞�ԓ��Ɍ��̃X�s�[�h�ɖ߂�
		float progress = std::min(afterDriftingElapsedTime / returnTime, 1.0f); // 0.0�`1.0�Ői�s�������v�Z

		// �s�����Ɋ�Â���moveSpeed�����̑��x�ɖ߂�
		moveSpeed = afterDriftingSpeed + (preDriftSpeed - afterDriftingSpeed) * progress;

		// �����߂�ׂ����x�ɒB������
		if (afterDriftingElapsedTime >= returnTime)
		{
			moveSpeed = preDriftSpeed;  // ���S�ɖ߂�
			driftState = NO_DRIFTING;   // �h���t�g�I��

			// �h���t�g�I����̉������Ԃ�ݒ�
			maxAddSpeedTime = maxDriftAddSpeedTime;
		}

		realSpeed = moveSpeed + driftingAddSpeed;
	}
	// �h���t�g��Ԃł͂Ȃ��ʏ�̏��
	else
	{
		// �����x�̍X�V
		realSpeed = moveSpeed + driftingAddSpeed;

		if (maxAddSpeedTime > 0.0f)
		{
			maxAddSpeedTime -= _deltaTime;
		}
		else
		{
			if (driftingAddSpeed > 0)
			{
				// �����l���v�Z���A�Œጸ���l��ۏ�
				float driftDeceleration = std::max(CalculateDeceleration(driftFrictionFactor, realSpeed), MinDeceleration);

				// ������K�p���A0�����ɂȂ�Ȃ��悤�ɐ���
				driftingAddSpeed = std::max(driftingAddSpeed - driftDeceleration * _deltaTime, 0.0f);
			}
		}
	}

	// �ǏՓˎ��̌�������
	if (isWallHit && speedRatio > WallDecelerationStartRatio)
	{
		// �ǏՓˎ��̌����x���v�Z
		float wallDeceleration = CalculateDeceleration(wallFrictionFactor, moveSpeed);

		// �h���t�g����preDriftSpeed������������
		float& targetSpeed = (driftState == NOW_DRIFTING) ? preDriftSpeed : moveSpeed;

		// ����
		targetSpeed -= wallDeceleration * _deltaTime;
	}

	// ���x��0���傫���ꍇ��SE���Đ�����
	if (realSpeed > 0.0f)
	{
		// SE���Đ����łȂ��Ƃ�SE���Đ�����
		if (!XA_IsPlaying(MOVE_SE)) XA_Play(MOVE_SE, MoveSEVol * volRatio);
	
		// SE�̍Đ����x�𑬓x�����ɉ����ĕω�������
		XA_SetSpeed(MOVE_SE, MoveSESpeed * (1.0f + speedRatio));

		// ���x�����ɉ����ĉ��ʂ��グ��
		XA_SetVolume(MOVE_SE, MoveSEVol* speedRatio);
	}
	else
	{
		// SE��~
		XA_Stop(MOVE_SE);
	}

	// ���x��K�p���Ĉʒu���X�V
	velocity.x = moveDirection.x * realSpeed;
	velocity.y += gravityDot * _deltaTime;
	velocity.z = moveDirection.z * realSpeed;

	// �܂��͉��̍��W�Ƃ��Čv�Z����
	// �Փ˔��莞�ɐ�����position�ɔ��f������
	newPosition.x += velocity.x * _deltaTime;
	newPosition.y += velocity.y * _deltaTime;
	newPosition.z += velocity.z * _deltaTime;

	// �\�j�b�N�u�[���̍X�V
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

		// �\�j�b�N�u�[���̃X�P�[��
		float sbScale = SonicBoomBaseScale * sonicBoomElapsedCount;
		sonicBoomScale = XMFLOAT3(sbScale, 1.0f, sbScale);
	}

	// ��]��K�p
	rotation.y = XMConvertToDegrees(angle);
}

// �W�����v����
void PlayerController::Jump(float _deltaTime)
{
	switch (jumpState)
	{
	case PlayerController::CAN_JUMP:   // �W�����v�\

		// �W�����v�L�[�������ꂽ�ꍇ
		if (isInputJumpKey)
		{
			// SE�Đ�
			XA_Play(JUMP_SE, JumpSEVol * volRatio);

			// Y�����̑��x��ݒ�
			velocity.y = jumpPower;

			// ��Ԃ��W�����v���ɐ؂�ւ�
			jumpState = PlayerController::JUMPING;
		}
		break;
	case PlayerController::JUMPING:   // �W�����v��

		// �W�����v�̍������ő�ɂȂ�����
		if (velocity.y <= 0)
		{
			// ��Ԃ𗎉��ɐ؂�ւ�
			jumpState = PlayerController::FALLING;
		}
		break;
	case PlayerController::FALLING:   // ������
		// �������͓��ʂȏ����͂Ȃ�
		break;
	default:
		break;
	}
}

// �����x���v�Z
float PlayerController::CalculateDeceleration(float frictionFactor, float _moveSpeed)
{
	// ���x�������قǌ��������㏸����
	return frictionFactor * _moveSpeed;
}

// ���x�̌��݂̊������v�Z
float PlayerController::CalculateSpeedRatio(float _realSpeed, float _maxSpeed)
{
	// �͈͂𒴂��Ȃ��悤�ɒ���
	return std::clamp(_realSpeed / _maxSpeed, 0.0f, 1.0f);
}

// �R�[�X�A�E�g��̐ݒ�
void PlayerController::HoleAfterSet()
{
	moveSpeed *= holeDecelerationRate;
	driftingAddSpeed *= holeDecelerationRate;
	velocity.y = 0;
}

// ���x�����ɉ������v���C���[�̐F���X�V
void PlayerController::UpdateSpeedRatioColor()
{
	// Phase2�܂ł̐F
	if (speedRatio < changeColorRatioPhase2)
	{
		SetRGBColor(speedRatioColor1);
	}
	// Phase3�܂ł̐F
	else if (speedRatio < changeColorRatioPhase3)
	{
		SetRGBColor(speedRatioColor2);
	}
	// Phase4�܂ł̐F
	else if (speedRatio < changeColorRatioPhase4)
	{
		SetRGBColor(speedRatioColor3);
	}
	// Phase4�̐F
	else
	{
		SetRGBColor(speedRatioColor4);
	}
}

// �e��������ݒ�
void PlayerController::SetSpeedRatioColorPhase(float phase1, float phase2, float phase3, float phase4)
{
	changeColorRatioPhase1 = phase1;
	changeColorRatioPhase2 = phase2;
	changeColorRatioPhase3 = phase3;
	changeColorRatioPhase4 = phase4;
}

// �e�����̐F��ݒ�
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

// �����x��ݒ�
void PlayerController::SetAccelerations(std::vector<float> accelers)
{
	accelerations = accelers;

	if (accelerations.size() != AccelerationsNum)
	{
		accelerations.resize(AccelerationsNum);
	}
}
