#include "Input.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

std::unique_ptr<Input> Input::instance = nullptr;

// シングルトンでインスタンシングを返す
Input& Input::Get()
{
	// インスタンスがまだ生成されていない場合、インスタンスを生成
	if (!instance)
	{
		// インスタンス生成
		instance = std::make_unique<Input>();
	}

	// インスタンスを返す
    return *instance;
}

// 初期化処理
void Input::Initialize(HWND hWnd, HINSTANCE hInstance)
{
	HRESULT	hr;
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(mDInput.GetAddressOf()), nullptr);
	if (FAILED(hr)) return;
	
	// キーボードデバイスの生成
	hr = mDInput->CreateDevice(GUID_SysKeyboard, mDevKeyboad.GetAddressOf(), nullptr);
	if (FAILED(hr))return;
	
	// キーボードデータフォーマットの設定
	hr = mDevKeyboad->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return;
	
	// キーボードの協調レベル設定
	hr = mDevKeyboad->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return;
	
	// マウスデバイスの生成
	hr = mDInput->CreateDevice(GUID_SysMouse, mDevMouse.GetAddressOf(), nullptr);
	if (FAILED(hr)) return;

	// マウスデータフォーマットの設定
	hr = mDevMouse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr)) return;
		
	// マウスの協調レベル設定
	hr = mDevMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return;

	// デバイスのアクエア
	mDevKeyboad->Acquire();
	mDevMouse->Acquire();
}

// 更新処理
void Input::Update()
{
	// １フレーム前のデータを保存
	memcpy_s(mOldKeyBuffer, sizeof(mOldKeyBuffer), mKeyBuffer, sizeof(mKeyBuffer));

	HRESULT hr;

	// キーボードの状態を取得する
	hr = mDevKeyboad->GetDeviceState(sizeof(mKeyBuffer), &mKeyBuffer);

	// 失敗した場合は、デバイスを再取得して状態を再取得
	if (FAILED(hr))
	{
		mDevKeyboad->Acquire();
		mDevKeyboad->GetDeviceState(sizeof(mKeyBuffer), &mKeyBuffer);
	}

	// 前のマウス状態を保存
	mOldMouseState = mMouseState;

	// マウスの状態を取得する
	hr = mDevMouse->GetDeviceState(sizeof(mMouseState), &mMouseState);

	// 失敗した場合は、デバイスを再取得して状態を再取得
	if (FAILED(hr))
	{
		mDevMouse->Acquire();
		mDevMouse->GetDeviceState(sizeof(mMouseState), &mMouseState);
	}
}

// 押している状態を返す
bool Input::GetKeyPress(int key)
{
	return (mKeyBuffer[key] & 0x80) != 0;
}

// 押された瞬間の状態を返す
bool Input::GetKeyTrigger(int key)
{
	return (mOldKeyBuffer[key] & 0x80) == 0 && (mKeyBuffer[key] & 0x80) != 0;
}

// 押されている状態を返す（マウス）
bool Input::GetMousePress(int button)
{
	return (mMouseState.rgbButtons[button] & 0x80) != 0;
}