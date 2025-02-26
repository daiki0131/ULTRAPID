#undef UNICODE
#include <Windows.h>
#include "Input.h"
#include "XInputGamePad.h"
#include "SceneManager.h"
#include "Fps.h"
#include "DebugUI.h"
#include "TitleScene.h"
#include "SelectStageScene.h"
#include "GameScene.h"
#include "Renderer.h"
#include "Geometry.h"
#include "Shader.h"
#include "Particle.h"
#include "GameManager.h"
#include "Collision.h"
#include "DebugConsole.h"

constexpr const char* ClassName = "HAL_AT13D432_13";// ウインドウクラス名
constexpr const char* WindowName = "ULTRAPID";      // ウインドウの名前
constexpr int ScreenWidth = 1920;                    // ウインドウの幅
constexpr int ScreenHeight = 1080;                   // ウインドウの高さ

// プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//=================================
// エントリポイント
//=================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// デバッグウインドウ生成
	DebugConsole::CreateDebugConsole();

	// ウインドウクラス WNDCLASSEX
	WNDCLASSEX wc;

	// 構造体のサイズ
	wc.cbSize = sizeof(WNDCLASSEX);
	// クラスのスタイル
	wc.style = CS_CLASSDC;
	// ウインドウプロシージャへのポインタ
	wc.lpfnWndProc = WndProc;
	// ウインドウクラス構造体の後に割り当てる余分なバイト数
	wc.cbClsExtra = 0;
	// ウインドウインスタンスの後に割り当てる余分なバイト数
	wc.cbWndExtra = 0;
	// クラスのウィンドウプロシージャを含むインスタンスへのハンドル
	wc.hInstance = hInstance;
	// クラスアイコンへのハンドル
	wc.hIcon = NULL;
	// クラスカーソルへのハンドル
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// クラスの背景ブラシへのハンドル
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	// クラスメニューのリソース名を指定するnullで終わる文字列へのポインタ
	wc.lpszMenuName = NULL;
	// nullで終わる文字列へのポインタ
	wc.lpszClassName = ClassName;
	// ウインドウクラスに関連付けられている小さなアイコンへのハンドル
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);// 引数:WNDCLASSEX構造体

	// ウインドハンドル HWND
	HWND hWnd;

	hWnd = CreateWindowEx(0,   // 拡張ウインドウスタイル
		ClassName,             // ウインドウクラスの名前
		WindowName,            // ウインドウの名前
		WS_OVERLAPPEDWINDOW,   // ウインドウスタイル
		CW_USEDEFAULT,         // ウインドウの左上Ｘ座標
		CW_USEDEFAULT,         // ウインドウの左上Ｙ座標 
		ScreenWidth,           // ウインドウの幅
		ScreenHeight,          // ウインドウの高さ
		NULL,                  // 親ウインドウのハンドル
		NULL,                  // メニューハンドルまたは子ウインドウID
		hInstance,             // インスタンスハンドル
		NULL);                 // ウインドウ作成データ

	// ウインドウを表示
	ShowWindow(hWnd, nCmdShow);
	// ウインドウのクライアント領域を更新
	UpdateWindow(hWnd);

	//==========初期化処理=============//

	// レンダラークラス初期化
	Renderer::Init(hWnd);
	// シェーダークラス初期化
	Shader::CreateShader();
	// メッシュ作成
	Geometry::Init();
	// パーティクルシステム初期化
	Particle::Init();
	// 当たり判定クラス初期化
	Collision::Init();
	// IMGUI初期化
	DebugUI::Init(Renderer::GetDevice().Get(), Renderer::GetContext().Get(), hWnd);
	
	// ゲームマネージャークラス
	GameManager gameManager;
	gameManager.Init();

	// シーンマネージャークラス
	SceneManager sceneManager;
	sceneManager.SetInitScene(std::make_unique<TitleScene>());
	//sceneManager.SetInitScene(std::make_unique<SelectStageScene>());
	//sceneManager.SetInitScene(std::make_unique<GameScene>());

	sceneManager.Init();

	// 入力クラスの初期化
	Input::Get().Initialize(hWnd, hInstance);

	// FPS初期化（FPS60で設定）
	FPS::Get()->Init(60.0f);

	// デバッグ時のIMGUI表示・非表示切り替え
#ifdef _DEBUG
	bool isRendererIMGUI = true;
#endif

	MSG msg;

	//===========ゲームループ==============//
	for (;;)
	{
		BOOL isAnyMessage = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

		// 何かメッセージがあるか判定
		if (isAnyMessage)           
		{
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) 
			{
				break;
			}
		}
		else
		{
			// Escapeキーが押されたらメッセージボックスを出す
			if (Input::Get().GetKeyTrigger(DIK_ESCAPE))
			{
				// アプリケーション終了ボックス表示
				if (MessageBoxA(NULL, "アプリケーションを終了しますか？", "ULTRAPID", MB_YESNO) == IDYES)
				{
					PostQuitMessage(0); // アプリ終了
				}
			}

			// FPS更新
			FPS::Get()->Update();
			// 入力更新処理
			Input::Get().Update();
			XInputGamePad::Get().Update();
			// シーン更新処理
			sceneManager.Update(FPS::Get()->GetFixedDeltaTime(), gameManager);
			// シャドウマップ初期化
			Renderer::ClearShadowMapScreen();
			// シャドウマップ描画
			sceneManager.DrawShadowMap();
			// 画面初期化
			Renderer::ClearScreen();
			// シーン描画
			sceneManager.Draw();

#ifdef _DEBUG
			// キーボードのGキーでIMGUI表示・非表示切り替え
			if (Input::Get().GetKeyTrigger(DIK_G))
			{
				if (isRendererIMGUI)
				{
					isRendererIMGUI = false;
				}
				else
				{
					isRendererIMGUI =  true;
				}
			}
			if (isRendererIMGUI)
			{
				DebugUI::Render(hWnd); // IMGUI更新
			}
			
#endif
			// 画面更新
			Renderer::UpdateScreen();
		}
	}
	// 解放処理
	gameManager.UnInit();
	sceneManager.UnInit();
	DebugUI::DisposeUI();

#ifdef _DEBUG
	DebugConsole::SaveLogMesseage();
#endif

	UnregisterClass(ClassName, hInstance);

	return (int)msg.wParam; // プログラム終了
}

//========================================
//コールバック関数
//========================================
LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:         // ウインドウ破棄のメッセージ
		
		PostQuitMessage(0);  // アプリ終了
		break;

	case WM_CLOSE:           // ウインドウXボタンが押されたら
		DestroyWindow(hWnd); //“WM_DESTROY”メッセージを送る
		break;

	case WM_KEYDOWN:         // キーが押されたイベント
		break;

	case WM_KEYUP:           // キーが離されたイベント
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}