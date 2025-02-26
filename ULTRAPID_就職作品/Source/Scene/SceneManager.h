#pragma once
#include <memory>
#include <unordered_map>
#include <map>
#include "FadeManager.h"
#include "Renderer.h"
#include "GameManager.h"

class SceneBase;
class Camera2D;

//===========================================//
// シーン全体を管理するクラス
//===========================================//
class SceneManager
{
private:
	static std::unique_ptr<SceneBase> nowScene;     // 現在のシーン
	std::unique_ptr<SceneBase> nextScene;			// 前のシーン
	static std::unique_ptr<FadeManager> sceneFade;  // シーン遷移専用フェード
	std::unique_ptr<Camera2D> camera2D;             // すべてのシーンを通して使うカメラ
	bool isChangingScene = false;                   // シーン遷移中か
	// モデル読み込み専用
	typedef std::unordered_map<std::wstring, Renderer::Model> ModelHash;
	std::unique_ptr<ModelHash> modelManager;
	Renderer::Model model;

public:
	void Update(float _deltaTime, GameManager& gm);
	void Draw();
	void DrawShadowMap();
	void Init();
	void UnInit();
	// 最初のシーンの初期化（Init関数より前に行う）
	void SetInitScene(std::unique_ptr<SceneBase> _initScene);
	// 新しいシーンをセット
	void SetScene(std::unique_ptr<SceneBase> _nextScene);
	// Set関数
	static FadeManager& GetFade() { return *sceneFade; }
	static void SetSceneIMGUI();
	// Get関数
	bool GetIsChangeScene() { return isChangingScene; };
	bool GetIsSceneFade() { return sceneFade->GetIsFade(); }
	
private:
	// シーン切り替え
	void ChangeScene();
};