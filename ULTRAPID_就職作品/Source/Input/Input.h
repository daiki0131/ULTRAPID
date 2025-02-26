#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <wrl/client.h>
#include <memory>
#include <dinput.h>

//===========================================//
// Direct Inputを使ってキーボードを取得するクラス
//===========================================//
class Input
{

private:
	static std::unique_ptr<Input> instance;                  // シングルトンインスタンス
	Microsoft::WRL::ComPtr<IDirectInput8> mDInput;           // DirectInput8 インターフェース
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mDevKeyboad; // キーボードデバイス
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mDevMouse;   // マウスデバイス
	unsigned char mKeyBuffer[256] = {};                      // 現在のキーボードの入力状態を保持するバッファ
	unsigned char mOldKeyBuffer[256] = {};                   // 1フレーム前のキーボードの入力状態を保持するバッファ
	DIMOUSESTATE2 mMouseState = {};                          // 現在のマウスの入力状態を保持する構造体
	DIMOUSESTATE2 mOldMouseState = {};                       // 1フレーム前のマウスの入力状態を保持する構造体
	
public:
	static Input& Get();
	// DirectInputの初期化を行う関数
	// ゲームループが始まる前に呼び出す
	void Initialize(HWND hWnd, HINSTANCE hInstance);
	// 毎ループ呼び出しキーの押下状態をバッファに保存する
	void Update();
	// 現在のキーボードの押下状態を取得する
	// key: DIK_で始まるキー定数を使う
	bool GetKeyPress(int key);
	// 押した瞬間のキーを取得する
	bool GetKeyTrigger(int key);

	bool GetMousePress(int button);


};