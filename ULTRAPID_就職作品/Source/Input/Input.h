#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <wrl/client.h>
#include <memory>
#include <dinput.h>

//===========================================//
// Direct Input���g���ăL�[�{�[�h���擾����N���X
//===========================================//
class Input
{

private:
	static std::unique_ptr<Input> instance;                  // �V���O���g���C���X�^���X
	Microsoft::WRL::ComPtr<IDirectInput8> mDInput;           // DirectInput8 �C���^�[�t�F�[�X
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mDevKeyboad; // �L�[�{�[�h�f�o�C�X
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mDevMouse;   // �}�E�X�f�o�C�X
	unsigned char mKeyBuffer[256] = {};                      // ���݂̃L�[�{�[�h�̓��͏�Ԃ�ێ�����o�b�t�@
	unsigned char mOldKeyBuffer[256] = {};                   // 1�t���[���O�̃L�[�{�[�h�̓��͏�Ԃ�ێ�����o�b�t�@
	DIMOUSESTATE2 mMouseState = {};                          // ���݂̃}�E�X�̓��͏�Ԃ�ێ�����\����
	DIMOUSESTATE2 mOldMouseState = {};                       // 1�t���[���O�̃}�E�X�̓��͏�Ԃ�ێ�����\����
	
public:
	static Input& Get();
	// DirectInput�̏��������s���֐�
	// �Q�[�����[�v���n�܂�O�ɌĂяo��
	void Initialize(HWND hWnd, HINSTANCE hInstance);
	// �����[�v�Ăяo���L�[�̉�����Ԃ��o�b�t�@�ɕۑ�����
	void Update();
	// ���݂̃L�[�{�[�h�̉�����Ԃ��擾����
	// key: DIK_�Ŏn�܂�L�[�萔���g��
	bool GetKeyPress(int key);
	// �������u�Ԃ̃L�[���擾����
	bool GetKeyTrigger(int key);

	bool GetMousePress(int button);


};