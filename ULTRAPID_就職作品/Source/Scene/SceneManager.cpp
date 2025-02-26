#define _CRT_SECURE_NO_WARNINGS
#include "xa2.h"
#include <vector>
#include "Geometry.h"
#include "Input.h"
#include "SceneManager.h"
#include "SceneBase.h"
#include "DebugUI.h"
#include "DebugConsole.h"
#include "GameObject.h"
#include "Camera.h"

using namespace DirectX;

std::unique_ptr<SceneBase> SceneManager::nowScene;
std::unique_ptr<FadeManager> SceneManager::sceneFade;

// 初期化処理（プログラム実行時）
void SceneManager::SetInitScene(std::unique_ptr<SceneBase> _initScene)
{
	// BGM・SE初期化
	HRESULT hr = XA_Initialize();
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("サウンド初期化失敗しました：SceneManager::SetInitScene内で発生", DebugConsole::ERROR_LOG);
	}

	// 今から読み込むシーンを入れる
	nowScene = std::move(_initScene);
	// シーン初期化
	nowScene->Init();

	// IMGUI関数配列初期化
	DebugUI::ClearDebugFunction();

	// IMGUI関数の登録
	DebugUI::RedistDebugFunction(SetSceneIMGUI);

	// フェードオブジェクト生成
	sceneFade = std::make_unique<FadeManager>(
		XMFLOAT3(0.0f, 0.0f, -0.25f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(16.0f, 9.0f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));
	// フェードイン開始
	sceneFade->StartFadeIn();

	// シーンのIMGUIパラメータを読み込み
	nowScene->LoadParameters();
}

// 遷移前のシーンを設定
void SceneManager::SetScene(std::unique_ptr<SceneBase> _nextScene)
{
	// 次のシーンを入れておく
	nextScene = std::move(_nextScene);

	// フェードアウト開始
	sceneFade->StartFadeOut();
	isChangingScene = true;
}

// シーンの切り替え
void SceneManager::ChangeScene()
{
	// 前シーンの解放処理
	if (nowScene)
	{
		nowScene->UnInit();
	}

	// オブジェクトを移譲
	nowScene = std::move(nextScene);

	// フェードイン開始
	sceneFade->StartFadeIn();
	isChangingScene = false;

	// 次のシーンの初期化処理
	if (nowScene)
	{
		nowScene->Init();
		nowScene->LoadParameters();
		DebugUI::ClearDebugFunction();
		DebugUI::RedistDebugFunction(SetSceneIMGUI);
	}
}

// 更新処理
void SceneManager::Update(float _deltaTime, GameManager& gm)
{
	// カメラ2D更新
	camera2D->Update(_deltaTime);

	// シーンをチェンジする場合のみ
	if (isChangingScene)
	{
		// フェードが終わったら処理を通す
		if (!sceneFade->GetIsFade())
		{
			ChangeScene(); // 新しいシーンに切り替え
		}
	}

	if (nowScene)
	{
		// 現在のシーンの更新処理
		nowScene->Update(_deltaTime, *this, gm);
	}

	// フェード更新
	sceneFade->Update(_deltaTime);
}

// 描画処理
void SceneManager::Draw()
{
	if (nowScene)
	{
		// 現在のシーンの描画処理
		nowScene->Draw(*camera2D);
	}

	// 2Dカメラのビュー・プロジェクション行列をセット
	Renderer::SetCameraViewProjection(camera2D->GetViewMatrix(), camera2D->GetProjectionMatrix());

	// フェード描画
	sceneFade->Draw();
}

void SceneManager::DrawShadowMap()
{
	if (nowScene)
	{
		nowScene->DrawShadowMap();
	}
}

// シーンマネージャーの初期化処理
void SceneManager::Init()
{
	//=================================================
	//  オブジェクト生成
	//=================================================
	modelManager = std::make_unique<ModelHash>();
	camera2D = std::make_unique<Camera2D>();
	
	//=================================================
	//  テクスチャ読み込み
	//=================================================
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1.0f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/fade.png");
	modelManager->emplace(L"Assets/Texture/fade.png", model);

	//=================================================
	//  モデルセット
	//=================================================
	model = modelManager->at(L"Assets/Texture/fade.png");
	sceneFade->SetModel(model);

	//=================================================
	//  初期設定
	//=================================================

	// 2Dカメラ
	camera2D->SetEye({ 0,0,-1 });
	camera2D->SetFocus({ 0,0,1 });
}

// 解放処理
void SceneManager::UnInit()
{
	//サウンドの解放処理
	XA_Release();

#ifdef _DEBUG
	// デバッグ時のみIMGUIで操作したパラメータをCSVに保存する
	nowScene->SaveParameters();
#endif

}

// シーンごとのIMGUI関数を実行
void SceneManager::SetSceneIMGUI()
{
#ifdef _DEBUG
	nowScene->SetIMGUI();
#endif
	
}