#include <iostream>
#include "XInputGamePad.h"

std::unique_ptr<XInputGamePad> XInputGamePad::instance = nullptr;

// シングルトンでインスタンシングを返す
XInputGamePad& XInputGamePad::Get()
{
	// インスタンスがまだ生成されていない場合、インスタンスを生成
	if (!instance)
	{
		// インスタンス生成
		instance = std::make_unique<XInputGamePad>();
	}
	// インスタンスを返す
	return *instance;
}

// 更新処理
void XInputGamePad::Update()
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	DWORD dwResult = XInputGetState(0, &state);

	// コントローラーがPCみ接続されている場合
	if (dwResult == ERROR_SUCCESS)
	{
		isGamePad = true;
		// 前のボタンのトリガー状態を保存
		oldButtons = nowButtons;
		// 左スティックの傾きを取得
		float LX = state.Gamepad.sThumbLX;
		float LY = state.Gamepad.sThumbLY;
	
		const int deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		if (fabsf(LX) < deadZone) LX = 0;
		if (fabsf(LY) < deadZone) LY = 0;

		// 前の左スティックのトリガー状態を保存
		oldLX = analogStick.LX;
		oldLY = analogStick.LY;

		// スティックのNormalize
		analogStick.LX = LX / 32767.0f;
		analogStick.LY = LY / 32767.0f;

		// 右スティックの傾きを取得
		float RX = state.Gamepad.sThumbRX;
		float RY = state.Gamepad.sThumbRY;

		const int rightStickDeadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		if (fabsf(RX) < rightStickDeadZone) RX = 0;
		if (fabsf(RY) < rightStickDeadZone) RY = 0;

		// 右スティックのNormalize
		analogStick.RX = RX / 32767.0f;
		analogStick.RY = RY / 32767.0f;

		// 現在のボタンを保存
		nowButtons = state.Gamepad.wButtons;
		rightTriggerValue = state.Gamepad.bRightTrigger;
		leftTriggerValue = state.Gamepad.bLeftTrigger;
	}
	else
	{
		isGamePad = false;
	}
}

// 押されている状態を返す
bool XInputGamePad::GetButtonPress(WORD _button)
{
	return (nowButtons & _button) != 0;
}

// 押された瞬間の状態を返す
bool XInputGamePad::GetButtonTrigger(WORD _button)
{
	return (oldButtons & _button) == 0 && (nowButtons & _button) != 0;
}

// スティックの傾きを取得
XInputGamePad::AnalogStick XInputGamePad::GetAnalogStick()
{
	return analogStick;
}

// 押された瞬間の状態を返す（左スティックX軸）
bool XInputGamePad::GetStickTriggerLX()
{
	return analogStick.LX != 0.0f && oldLX == 0.0f;
}

// 押された瞬間の状態を返す（左スティックY軸）
bool XInputGamePad::GetStickTriggerLY()
{
	return analogStick.LY != 0.0f && oldLY == 0.0f;
}

// 押されている状態を返す（L2ボタン）
bool XInputGamePad::GetL2ButtonPress()
{
	return leftTriggerValue > 0;
}

// 押されている状態を返す（R2ボタン）
bool XInputGamePad::GetR2ButtonPress()
{
	return rightTriggerValue > 0;
}