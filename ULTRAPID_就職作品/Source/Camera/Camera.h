#pragma once
#include "SceneManager.h"

class TitleScene;
class GameScene;
class SceneManager;

//===========================================//
// �J�������N���X
//===========================================//
class Camera
{
protected:
	DirectX::XMMATRIX matrixView;        // �ϊ��s��
	DirectX::XMFLOAT3 eye = { 0,0,-10 }; // �J�����̈ʒu
	DirectX::XMFLOAT3 focus = { 0,0,0 }; // �J�����̌��Ă���ʒu
	DirectX::XMFLOAT3 up = { 0,1,0 };    // �J�����̏�����x�N�g��

public:
	
	// Set�֐�
	void SetEye(const DirectX::XMFLOAT3 _eye) { eye = _eye; };
	void SetFocus(const DirectX::XMFLOAT3 _focus) { focus = _focus; }
	// Get�֐�
	DirectX::XMFLOAT3 GetEye() { return eye; }
	DirectX::XMFLOAT3 GetFocus() { return focus; }
	DirectX::XMMATRIX GetViewProjectionMatrix();
	virtual DirectX::XMMATRIX GetProjectionMatrix() = 0;
	DirectX::XMMATRIX GetViewMatrix() { return matrixView; };
	// �r���[�s��v�Z
	void CalculateViewMatrix(DirectX::XMFLOAT3 _eye, DirectX::XMFLOAT3 _focus, DirectX::XMFLOAT3 _up);
};

//===========================================//
// �J����2D�N���X
//===========================================//
class Camera2D : public Camera
{
public:

	void Update(const float _deltaTime);
	// Get�֐�
	DirectX::XMMATRIX GetProjectionMatrix() override;
};

//===========================================//
// �J����3D�N���X
//===========================================//
class Camera3D : public Camera
{
public:
	static const float MaxAdjustFocus;                  // Focus�̒����l
	static const float MinEyeZ;                         // Eye��Z�l�ŏ��̒����l

protected:
	float fovAngle = 45.0f;                             // �J�����̎���p�x
	float revoAngle = 0;                                // �J�����̉�]�p�x
	float revoAngleSpeed = 0.035f;                      // ��]���x
	DirectX::XMFLOAT3 adjustFocus = { 0.0f,2.0f,0.0f }; // �J�����̃^�[�Q�b�g�̕␳
	DirectX::XMFLOAT3 adjustEye = { 0.0f,6.0f,-10.0f }; // �J�����̍��W�̕␳
	DirectX::XMFLOAT3 afterEye = { 0,0,0 };             // �ړ���̃J�������W

public:
	// Set�֐�
	void SetAdjustFocus(DirectX::XMFLOAT3 _focus) { adjustFocus = _focus; }
	void SetRevoAngle(float _angle) { revoAngle = _angle; }
	void SetFovAngle(float _fov) { fovAngle = _fov; };
	// Get�֐�
	DirectX::XMMATRIX GetProjectionMatrix() override;
	DirectX::XMFLOAT3 GetAdjustFocus() { return adjustFocus; }
	DirectX::XMFLOAT3 const GetAfterEye() { return afterEye; }
	float const GetRevoAngle() { return revoAngle; }
	float const GetFovAngle() { return fovAngle; }

};

//===========================================//
// �^�C�g���V�[����p�J����
//===========================================//
class TitleSceneCamera : public Camera3D
{
public:
	void Update(const float _deltaTime, TitleScene& titleScene);

};

//===========================================//
// �Q�[���V�[����p�J����
//===========================================//
class GameSceneCamera : public Camera3D
{
public:
	static const float MaxFov;                     // �ő压��p
	static const float MinFov;                     // �ŏ�����p
	static const float MaxShakeIntensity;          // �ő�̃J�����̗h��̋���

private:
	float maxShakeIntensity = MaxShakeIntensity;   // �h��̋����i�ő�j
	float shakeIntensity = 0.0f;                   // �h��̋���
	float shakeDuration = 0.0f;                    // �h��̎�������
	float shakeElapsedTime = 0.0f;                 // �h��o�ߎ���
	float minFov = 45.0f;                          // �ŏ�����p
	float maxFov = 150.0f;                         // �ő压��p

	bool isChaseCamera = true;                     // �J�������Ǐ]��
	bool isInputCamera = true;                     // ���͉�
	bool isHitStop = false;                        // �q�b�g�X�g�b�v��

	DirectX::XMFLOAT3 nowPlayerPos;
	DirectX::XMFLOAT3 oldPlayerPos;

public:
	void Update(const float _deltaTime, GameScene& gameScene);
	// Set�֐�
	void StartCameraShake(float _hitStopTime, float intensityRatio);
	void SetIsChase(bool _isChase) { isChaseCamera = _isChase; }
	void SetIsInputCamera(bool _isInput) { isInputCamera = _isInput; }
	void SetIsHitStop(bool _isHitStop) { isHitStop = _isHitStop; }
	// Get�֐�
	DirectX::XMFLOAT3 GetAfterEye() { return afterEye; }

private:
	// �J�����̗h��𔽉f
	DirectX::XMFLOAT3 ApplyCameraShake(DirectX::XMFLOAT3 _eye);
	// �J�����̗h����X�V
	void UpdateCameraShake(const float _deltaTime);
};

//===========================================//
// �f�o�b�O��p�J����
//===========================================//
class DebugCamera : public Camera3D
{
private:
	const float MouseSensitivity = 0.004f;  // �}�E�X�̊��x
	const float MoveSpeed = 50.0f;          // �J�����̈ړ����x
private:
	DirectX::XMINT2 lastMouse{};            // �Ō�ɉE�N���b�N�����Ƃ��̃}�E�X�̍��W
	bool isDragging = false;                // �E�N���b�N���Ă�����

public:
	void Update(float _deltaTime);
	// �}�E�X�E�N���b�N��t
	void UpdateMouseInput();
	// �L�[�����t
	void UpdateKeyInput(float _deltaTime);
	// �J�����ړ�
	void MoveCamera(float _deltaX, float _deltaY, float _deltaZ);
	// �J������]�iEye���S���j
	void RotateCamera(float _pitch, float _yaw);
};