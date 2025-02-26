#include "Input.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

std::unique_ptr<Input> Input::instance = nullptr;

// �V���O���g���ŃC���X�^���V���O��Ԃ�
Input& Input::Get()
{
	// �C���X�^���X���܂���������Ă��Ȃ��ꍇ�A�C���X�^���X�𐶐�
	if (!instance)
	{
		// �C���X�^���X����
		instance = std::make_unique<Input>();
	}

	// �C���X�^���X��Ԃ�
    return *instance;
}

// ����������
void Input::Initialize(HWND hWnd, HINSTANCE hInstance)
{
	HRESULT	hr;
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(mDInput.GetAddressOf()), nullptr);
	if (FAILED(hr)) return;
	
	// �L�[�{�[�h�f�o�C�X�̐���
	hr = mDInput->CreateDevice(GUID_SysKeyboard, mDevKeyboad.GetAddressOf(), nullptr);
	if (FAILED(hr))return;
	
	// �L�[�{�[�h�f�[�^�t�H�[�}�b�g�̐ݒ�
	hr = mDevKeyboad->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return;
	
	// �L�[�{�[�h�̋������x���ݒ�
	hr = mDevKeyboad->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return;
	
	// �}�E�X�f�o�C�X�̐���
	hr = mDInput->CreateDevice(GUID_SysMouse, mDevMouse.GetAddressOf(), nullptr);
	if (FAILED(hr)) return;

	// �}�E�X�f�[�^�t�H�[�}�b�g�̐ݒ�
	hr = mDevMouse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr)) return;
		
	// �}�E�X�̋������x���ݒ�
	hr = mDevMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return;

	// �f�o�C�X�̃A�N�G�A
	mDevKeyboad->Acquire();
	mDevMouse->Acquire();
}

// �X�V����
void Input::Update()
{
	// �P�t���[���O�̃f�[�^��ۑ�
	memcpy_s(mOldKeyBuffer, sizeof(mOldKeyBuffer), mKeyBuffer, sizeof(mKeyBuffer));

	HRESULT hr;

	// �L�[�{�[�h�̏�Ԃ��擾����
	hr = mDevKeyboad->GetDeviceState(sizeof(mKeyBuffer), &mKeyBuffer);

	// ���s�����ꍇ�́A�f�o�C�X���Ď擾���ď�Ԃ��Ď擾
	if (FAILED(hr))
	{
		mDevKeyboad->Acquire();
		mDevKeyboad->GetDeviceState(sizeof(mKeyBuffer), &mKeyBuffer);
	}

	// �O�̃}�E�X��Ԃ�ۑ�
	mOldMouseState = mMouseState;

	// �}�E�X�̏�Ԃ��擾����
	hr = mDevMouse->GetDeviceState(sizeof(mMouseState), &mMouseState);

	// ���s�����ꍇ�́A�f�o�C�X���Ď擾���ď�Ԃ��Ď擾
	if (FAILED(hr))
	{
		mDevMouse->Acquire();
		mDevMouse->GetDeviceState(sizeof(mMouseState), &mMouseState);
	}
}

// �����Ă����Ԃ�Ԃ�
bool Input::GetKeyPress(int key)
{
	return (mKeyBuffer[key] & 0x80) != 0;
}

// �����ꂽ�u�Ԃ̏�Ԃ�Ԃ�
bool Input::GetKeyTrigger(int key)
{
	return (mOldKeyBuffer[key] & 0x80) == 0 && (mKeyBuffer[key] & 0x80) != 0;
}

// ������Ă����Ԃ�Ԃ��i�}�E�X�j
bool Input::GetMousePress(int button)
{
	return (mMouseState.rgbButtons[button] & 0x80) != 0;
}