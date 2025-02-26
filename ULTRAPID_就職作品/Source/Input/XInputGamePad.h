#pragma once
#include <Windows.h>
#include <XInput.h>
#include <memory>
#pragma comment(lib, "Xinput.lib")

//===========================================//
// XInputのゲームパッドの入力操作を取得するクラス
//===========================================//
class XInputGamePad
{
public:
	// ゲームパッド左スティックのX・Y軸の値を格納する構造体
	struct AnalogStick
	{
		float LX;
		float LY;
		float RX;
		float RY;
	};

private:
	static std::unique_ptr<XInputGamePad> instance;  //	シングルトンインスタンス
	XINPUT_STATE state;					             // 入力ボタンの状態を格納する変数
	WORD oldButtons;					             // 1フレーム前のボタンを格納する変数
	WORD nowButtons;					             // 現在押しているボタンを格納する変数
	float oldLX = 0.0f;                              // 前のフレームの左スティックのX成分
	float oldLY = 0.0f;                              // 前のフレームの左スティックのY成分
	AnalogStick analogStick = { 0,0 };              // アナログスティックのX・Y軸の値を格納する変数
	bool isGamePad = false;                          // ゲームパッド接続可否
	BYTE leftTriggerValue;                           // 左トリガー数値の値
	BYTE rightTriggerValue;                          // 右トリガー数値の値

public:
	static XInputGamePad& Get();
	// 更新処理
	void Update();
	// 押しているボタンを取得する関数
	bool GetButtonPress(WORD _button);
	// 押した瞬間のボタンを取得する変数
	bool GetButtonTrigger(WORD _button);
	// スティックの情報を取得する変数
	AnalogStick GetAnalogStick();
	// 左スティックのトリガー状態を取得
	bool GetStickTriggerLX();
	bool GetStickTriggerLY();
	// ゲームパッド接続可否を返す
	bool GetIsGamePad() { return isGamePad; }
	// L2・R2が押されてるかを返す
	bool GetL2ButtonPress();
	bool GetR2ButtonPress();

};