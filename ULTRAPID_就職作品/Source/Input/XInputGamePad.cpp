#include <iostream>
#include "XInputGamePad.h"

std::unique_ptr<XInputGamePad> XInputGamePad::instance = nullptr;

// �V���O���g���ŃC���X�^���V���O��Ԃ�
XInputGamePad& XInputGamePad::Get()
{
	// �C���X�^���X���܂���������Ă��Ȃ��ꍇ�A�C���X�^���X�𐶐�
	if (!instance)
	{
		// �C���X�^���X����
		instance = std::make_unique<XInputGamePad>();
	}
	// �C���X�^���X��Ԃ�
	return *instance;
}

// �X�V����
void XInputGamePad::Update()
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	DWORD dwResult = XInputGetState(0, &state);

	// �R���g���[���[��PC�ݐڑ�����Ă���ꍇ
	if (dwResult == ERROR_SUCCESS)
	{
		isGamePad = true;
		// �O�̃{�^���̃g���K�[��Ԃ�ۑ�
		oldButtons = nowButtons;
		// ���X�e�B�b�N�̌X�����擾
		float LX = state.Gamepad.sThumbLX;
		float LY = state.Gamepad.sThumbLY;
	
		const int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		if (fabsf(LX) < deadZone) LX = 0;
		if (fabsf(LY) < deadZone) LY = 0;

		// �O�̍��X�e�B�b�N�̃g���K�[��Ԃ�ۑ�
		oldLX = analogStick.LX;
		oldLY = analogStick.LY;

		// �X�e�B�b�N��Normalize
		analogStick.LX = LX / 32767.0f;
		analogStick.LY = LY / 32767.0f;

		// �E�X�e�B�b�N�̌X�����擾
		float RX = state.Gamepad.sThumbRX;
		float RY = state.Gamepad.sThumbRY;

		const int rightStickDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		if (fabsf(RX) < rightStickDeadZone) RX = 0;
		if (fabsf(RY) < rightStickDeadZone) RY = 0;

		// �E�X�e�B�b�N��Normalize
		analogStick.RX = RX / 32767.0f;
		analogStick.RY = RY / 32767.0f;

		// ���݂̃{�^����ۑ�
		nowButtons = state.Gamepad.wButtons;
		rightTriggerValue = state.Gamepad.bRightTrigger;
		leftTriggerValue = state.Gamepad.bLeftTrigger;
	}
	else
	{
		isGamePad = false;
	}
}

// ������Ă����Ԃ�Ԃ�
bool XInputGamePad::GetButtonPress(WORD _button)
{
	return (nowButtons & _button) != 0;
}

// �����ꂽ�u�Ԃ̏�Ԃ�Ԃ�
bool XInputGamePad::GetButtonTrigger(WORD _button)
{
	return (oldButtons & _button) == 0 && (nowButtons & _button) != 0;
}

// �X�e�B�b�N�̌X�����擾
XInputGamePad::AnalogStick XInputGamePad::GetAnalogStick()
{
	return analogStick;
}

// �����ꂽ�u�Ԃ̏�Ԃ�Ԃ��i���X�e�B�b�NX���j
bool XInputGamePad::GetStickTriggerLX()
{
	return analogStick.LX != 0.0f && oldLX == 0.0f;
}

// �����ꂽ�u�Ԃ̏�Ԃ�Ԃ��i���X�e�B�b�NY���j
bool XInputGamePad::GetStickTriggerLY()
{
	return analogStick.LY != 0.0f && oldLY == 0.0f;
}

// ������Ă����Ԃ�Ԃ��iL2�{�^���j
bool XInputGamePad::GetL2ButtonPress()
{
	return leftTriggerValue > 0;
}

// ������Ă����Ԃ�Ԃ��iR2�{�^���j
bool XInputGamePad::GetR2ButtonPress()
{
	return rightTriggerValue > 0;
}