#pragma once
#include <Windows.h>
#include <XInput.h>
#include <memory>
#pragma comment(lib, "Xinput.lib")

//===========================================//
// XInput�̃Q�[���p�b�h�̓��͑�����擾����N���X
//===========================================//
class XInputGamePad
{
public:
	// �Q�[���p�b�h���X�e�B�b�N��X�EY���̒l���i�[����\����
	struct AnalogStick
	{
		float LX;
		float LY;
		float RX;
		float RY;
	};

private:
	static std::unique_ptr<XInputGamePad> instance;  //	�V���O���g���C���X�^���X
	XINPUT_STATE state;					             // ���̓{�^���̏�Ԃ��i�[����ϐ�
	WORD oldButtons;					             // 1�t���[���O�̃{�^�����i�[����ϐ�
	WORD nowButtons;					             // ���݉����Ă���{�^�����i�[����ϐ�
	float oldLX = 0.0f;                              // �O�̃t���[���̍��X�e�B�b�N��X����
	float oldLY = 0.0f;                              // �O�̃t���[���̍��X�e�B�b�N��Y����
	AnalogStick analogStick = { 0,0 };              // �A�i���O�X�e�B�b�N��X�EY���̒l���i�[����ϐ�
	bool isGamePad = false;                          // �Q�[���p�b�h�ڑ���
	BYTE leftTriggerValue;                           // ���g���K�[���l�̒l
	BYTE rightTriggerValue;                          // �E�g���K�[���l�̒l

public:
	static XInputGamePad& Get();
	// �X�V����
	void Update();
	// �����Ă���{�^�����擾����֐�
	bool GetButtonPress(WORD _button);
	// �������u�Ԃ̃{�^�����擾����ϐ�
	bool GetButtonTrigger(WORD _button);
	// �X�e�B�b�N�̏����擾����ϐ�
	AnalogStick GetAnalogStick();
	// ���X�e�B�b�N�̃g���K�[��Ԃ��擾
	bool GetStickTriggerLX();
	bool GetStickTriggerLY();
	// �Q�[���p�b�h�ڑ��ۂ�Ԃ�
	bool GetIsGamePad() { return isGamePad; }
	// L2�ER2��������Ă邩��Ԃ�
	bool GetL2ButtonPress();
	bool GetR2ButtonPress();

};