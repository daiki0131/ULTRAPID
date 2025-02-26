#include "Camera.h"
#include "Input.h"
#include "XInputGamePad.h"
#include "DebugUI.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "GameManager.h"

using namespace DirectX;

// �萔��`
const float Camera3D::MaxAdjustFocus = 15.0f;
const float Camera3D::MinEyeZ = -50.0f;
const float GameSceneCamera::MaxFov = 130.0f;
const float GameSceneCamera::MinFov = 45.0f;
const float GameSceneCamera::MaxShakeIntensity = 0.003f;

// �r���[�s��v�Z�֐�
void Camera::CalculateViewMatrix(DirectX::XMFLOAT3 _eye, DirectX::XMFLOAT3 _focus, DirectX::XMFLOAT3 _up)
{
	XMVECTOR vEye = XMVectorSet(_eye.x, _eye.y, _eye.z, 0);         // �ʒu
	XMVECTOR vFocus = XMVectorSet(_focus.x, _focus.y, _focus.z, 0); // �����_
	XMVECTOR vUp = XMVectorSet(_up.x, _up.y, _up.z, 0);             // ������x�N�g��

	// �r���[�s��v�Z
	matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);
}

// �r���[�E�v���W�F�N�V�����s���Ԃ��֐�
DirectX::XMMATRIX Camera::GetViewProjectionMatrix()
{
	return XMMatrixMultiply(matrixView, GetProjectionMatrix());
}


// ======== Camera2D ========= //
// �X�V����
void Camera2D::Update(const float _deltaTime)
{
	CalculateViewMatrix(eye, focus, up);
}

// 2D�J�����̎ˉe�s���Ԃ��֐�
DirectX::XMMATRIX Camera2D::GetProjectionMatrix()
{
	return  XMMatrixOrthographicLH(16.0f * 0.3f, 9.0f * 0.3f, 0.0f, 3.0f);
}

// ======== Camera3D ========= //
// 3D�J�����̎ˉe�s���Ԃ��֐�
DirectX::XMMATRIX Camera3D::GetProjectionMatrix()
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(fovAngle), 16.0f / 9.0f, 0.01f, 3000.0f);
}

// ======== TitleSceneCamera ========= //
// �X�V����
void TitleSceneCamera::Update(const float _deltaTime, TitleScene& titleScene)
{
	focus = titleScene.GetPlayer().GetPosition(); // �^�[�Q�b�g�ݒ�
	
	// �^�[�Q�b�g�̍��W�𒲐�
	focus.x += adjustFocus.x;
	focus.y += adjustFocus.y;

	// Eye��ݒ�iX�EY���ړ��j
	eye = XMFLOAT3(focus.x, focus.y, eye.z);
	
	CalculateViewMatrix(eye, focus, up);
}

// ======== GameSceneCamera ========= //
void GameSceneCamera::Update(const float _deltaTime, GameScene& gameScene)
{
	focus = gameScene.GetPlayer().GetPosition(); // �^�[�Q�b�g�ݒ�

	// �q�b�g�X�g�b�v���ł͂Ȃ��Ƃ���
	// ��������Eye�̍��W��ݒ�
	if (!isHitStop)
	{
		eye.y = focus.y + adjustEye.y;
		eye.z = focus.z + adjustEye.z;
	}

	// �^�[�Q�b�g�̍��W�𒲐�
	focus.y += adjustFocus.y;

	// �J�������Ǐ]�\��Ԃ̏ꍇ
	if (isChaseCamera)
	{
		// ���݂ƑO�̃v���C���[�̍��W���擾
		nowPlayerPos = gameScene.GetPlayer().GetPosition();
		oldPlayerPos = gameScene.GetPlayer().GetOldPosition();
 		
		// XMFLOAT3����XMVECTOR�ɕϊ�����
		XMVECTOR nowPlayerPosVec = XMLoadFloat3(&nowPlayerPos);
		XMVECTOR oldPlayerPosVec = XMLoadFloat3(&oldPlayerPos);

		// �J�������ړ������邽�߂̃x�N�g�����v�Z
		XMVECTOR moveVec = nowPlayerPosVec - oldPlayerPosVec;

		XMFLOAT3 cameraEye = eye;
		// �ړ���̃J�����̈ʒu��\��
		XMVECTOR eyeVec = XMLoadFloat3(&cameraEye);
		eyeVec = eyeVec + moveVec;

		// XMVECTOR����XMFLOAT3�ɕϊ�����
		XMStoreFloat3(&eye, eyeVec);

		// XMFLOAT3����XMVECTOR�ɕϊ�����
		XMVECTOR vEye = XMVectorSet(eye.x, eye.y, eye.z, 0);
		XMVECTOR vFocus = XMVectorSet(focus.x, focus.y, focus.z, 0);
		XMVECTOR vUp = XMVectorSet(up.x, up.y, up.z, 0);

		// �q�b�g�X�g�b�v���̃J�����̗h������s
		UpdateCameraShake(_deltaTime);
		XMFLOAT3 shakeEye = ApplyCameraShake(eye);

		// �v���C���[�ɂ��J��������
		if (isInputCamera)
		{
			// �Q�[���p�b�h�̃X�e�B�b�N�̌X�����擾
			XInputGamePad::AnalogStick analogStick = XInputGamePad::Get().GetAnalogStick();

			// ���͂̌X��
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
		// ��]����\���x�N�g��
		XMVECTOR axis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		// ��]���Ɗp�x�Ɋ�Â��ăN�H�[�^�j�I�����v�Z
		XMVECTOR quaternion = XMQuaternionRotationAxis(axis, revoAngle);

		// �L�����N�^�[����̑��Έʒu
		XMVECTOR relativePos = XMVectorSet(shakeEye.x, shakeEye.y, shakeEye.z, 0) - vFocus;
		relativePos = XMVector3Rotate(relativePos, quaternion);
		// ��]��̃J�����ʒu
		vEye = relativePos + vFocus;

		XMStoreFloat3(&afterEye, vEye);
		matrixView = XMMatrixLookAtLH(vEye, vFocus, vUp);
	}
	else
	{
		CalculateViewMatrix(afterEye, focus, up);
	}
	
}

// �q�b�g�X�g�b�v���̃J�����̗h������s����֐�
void GameSceneCamera::StartCameraShake(float _hitStopTime, float intensityRatio)
{
	shakeIntensity = maxShakeIntensity * (1.0f - intensityRatio); // �h���ݒ�
	shakeDuration = _hitStopTime;       // �q�b�g�X�g�b�v�̎��Ԃ�ݒ�
	shakeElapsedTime = 0;               // �o�ߎ��Ԃ�0���Z�b�g
}

// �J�����̗h��𔽉f����֐�
DirectX::XMFLOAT3 GameSceneCamera::ApplyCameraShake(DirectX::XMFLOAT3 _eye)
{
	// �h��̋�����0�ȏ�̏ꍇ
	if (shakeIntensity > 0.0f)
	{
		std::uniform_real_distribution<float> rand(-100, 99);  // �h��̕����̃����_���ݒ�

		// �����_���ɗh���I�t�Z�b�g
		float shakeX = rand(GameManager::mt) * shakeIntensity;
		float shakeY = rand(GameManager::mt) * shakeIntensity;
		float shakeZ = rand(GameManager::mt) * shakeIntensity;

		// �h��̃I�t�Z�b�g���J�����ʒu�ɉ��Z
		_eye.x += shakeX;
		_eye.y += shakeY;
		_eye.z += shakeZ;
	}

	return _eye;
}

// �J�����̗h����X�V
void GameSceneCamera::UpdateCameraShake(const float _deltaTime)
{
	// �h�ꂪ�p�����Ȃ��
	if (shakeElapsedTime < shakeDuration)
	{
		shakeElapsedTime += _deltaTime;
		// ���Ԃ̌o�߂Ɋ�Â��ėh�ꋭ�x������������
		float t = shakeElapsedTime / shakeDuration;      // �o�ߎ��Ԃ̊��� (0.0 �` 1.0)
		t = std::clamp(t, 0.0f, 1.0f);

		shakeIntensity = maxShakeIntensity * (1.0f - t); // ���`��Ԃŏ��X�Ɍ���
	}
	else
	{
		shakeIntensity = 0.0f; // �h�ꂪ�I�������狭����0��
	}
}

// ======== DebugCamera ========= //
// �X�V����
void DebugCamera::Update(float _deltaTime)
{
	UpdateMouseInput();                  // �}�E�X�擾
	UpdateKeyInput(_deltaTime);          // �L�[�C���v�b�g�擾
	CalculateViewMatrix(eye, focus, up);
}

// �J�����̈ړ�����
void DebugCamera::MoveCamera(float _deltaX, float _deltaY, float _deltaZ)
{
	XMVECTOR EyeVec = XMLoadFloat3(&eye);
	XMVECTOR TargetVec = XMLoadFloat3(&focus);
	XMVECTOR UpVec = XMLoadFloat3(&up);

	// �O�����x�N�g��
	XMVECTOR Look = XMVector3Normalize(TargetVec - EyeVec);
	// �E�����x�N�g��
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(UpVec, Look));

	// �ړ�����
	EyeVec += Right * _deltaX;    // A/D�L�[�i���E�ړ��j
	EyeVec += UpVec * _deltaY;    // Q/E�L�[�i�㏸�E���~�j
	EyeVec += Look * _deltaZ;     // W/S�L�[�i�O�i�E��ށj
	TargetVec += Right * _deltaX;
	TargetVec += UpVec * _deltaY;
	TargetVec += Look * _deltaZ;

	// �V�����ʒu��K�p
	XMStoreFloat3(&eye, EyeVec);
	XMStoreFloat3(&focus, TargetVec);
}

// �}�E�X�̃N���b�N���擾����֐�
void DebugCamera::UpdateMouseInput()
{
	POINT mousePos;
	GetCursorPos(&mousePos); // �}�E�X�̃X�N���[����̈ʒu���擾

	// �}�E�X�̉E�N���b�N�������ꂽ��
	if (Input::Get().GetMousePress(1))
	{
		// �ŏ��Ƀ}�E�X���N���b�N���ꂽ�Ƃ��̂ݒʂ�
		if (!isDragging)
		{
			lastMouse.x = mousePos.x;
			lastMouse.y = mousePos.y;
			isDragging = true;
		}
		float dx = (mousePos.x - lastMouse.x) * MouseSensitivity; // �������̈ړ��ʌv�Z
		float dy = (mousePos.y - lastMouse.y) * MouseSensitivity; // �c�����̈ړ��ʌv�Z

		RotateCamera(dy, dx); // ��]�������s

		// �}�E�X�̌��݈ʒu��ۑ�
		lastMouse.x = mousePos.x;
		lastMouse.y = mousePos.y;
	}
	else
	{
		isDragging = false;
	}
}

// �L�[������擾����֐�
void DebugCamera::UpdateKeyInput(float _deltaTime)
{
	// �E�N���b�N��������ĂȂ��Ƃ���return
	if (!isDragging) return;

	float realSpeed = MoveSpeed * _deltaTime; // �J�����̈ړ����x

	if (Input::Get().GetKeyPress(DIK_W)) MoveCamera(0, 0, realSpeed);  // �E�N���b�N�{W�L�[
	if (Input::Get().GetKeyPress(DIK_S)) MoveCamera(0, 0, -realSpeed); // �E�N���b�N�{S�L�[
	if (Input::Get().GetKeyPress(DIK_A)) MoveCamera(-realSpeed, 0, 0); // �E�N���b�N�{A�L�[
	if (Input::Get().GetKeyPress(DIK_D)) MoveCamera(realSpeed, 0, 0);  // �E�N���b�N�{D�L�[
	if (Input::Get().GetKeyPress(DIK_Q)) MoveCamera(0, realSpeed, 0);  // �E�N���b�N�{Q�L�[
	if (Input::Get().GetKeyPress(DIK_E)) MoveCamera(0, -realSpeed, 0); // �E�N���b�N�{E�L�[
}

// �J������Eye�����ɉ�]������
void DebugCamera::RotateCamera(float _pitch, float _yaw)
{

	XMVECTOR EyeVec = XMLoadFloat3(&eye);
	XMVECTOR TargetVec = XMLoadFloat3(&focus);
	XMVECTOR UpVec = XMLoadFloat3(&up);

	// �J�����̌����Ă���������v�Z
	XMVECTOR Look = XMVector3Normalize(TargetVec - EyeVec);

	// ���E��]
	XMMATRIX yawMatrix = XMMatrixRotationAxis(UpVec, _yaw);
	Look = XMVector3TransformNormal(Look, yawMatrix);

	// �㉺��]
	XMVECTOR Right = XMVector3Normalize(XMVector3Cross(UpVec, Look));
	XMMATRIX pitchMatrix = XMMatrixRotationAxis(Right, _pitch);
	Look = XMVector3TransformNormal(Look, pitchMatrix);

	TargetVec = EyeVec + Look;

	// ��]�K�p
	XMStoreFloat3(&focus, TargetVec);
}