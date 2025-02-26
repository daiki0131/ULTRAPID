#pragma once
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include "SceneBase.h"
#include "Object2D.h"
#include "ScoreUI.h"

class Camera2D;

//===========================================//
// セレクトステージシーンクラス
//===========================================//
class SelectStageScene : public SceneBase
{

private:
	std::unique_ptr<Object2D> selectStageTexture;                                    // SelectStageテクスチャ
	std::unique_ptr<Object2D> frame01;                                               // フレーム1
	std::unique_ptr<Object2D> frame02;                                               // フレーム2
	std::unique_ptr<Object2D> bestScoreTexture;                                      // BestScoreテクスチャ
	std::vector<std::unique_ptr<Object2D>> stageNames;                               // ステージ名テクスチャ
	std::vector<std::unique_ptr<Object2D>> stageMapPhoto;                            // ステージの写真
	std::unique_ptr<ScoreUI> scoreUI;                                                // スコア数字UI
	
	int selectStageCount = 0;                                                        // 選択中ステージのカウント
	int selectStageScore = 0;                                                        // 選択中ステージのスコア
	DirectX::XMFLOAT3 nameUpPosition = DirectX::XMFLOAT3(-1.4f, 0.5f, -0.2f);        // ステージ名上の座標
	DirectX::XMFLOAT3 nameMiddlePosition = DirectX::XMFLOAT3(-1.4f, 0.0f, -0.2f);    // ステージ名中の座標
	DirectX::XMFLOAT3 nameDownPosition = DirectX::XMFLOAT3(-1.4f, -0.5f, -0.2f);     // ステージ名下の座標
	DirectX::XMFLOAT3 nameUpDownScale = DirectX::XMFLOAT3(1.0f, 0.233f, 1.0f);       // ステージ名上下のスケール
	DirectX::XMFLOAT3 nameMiddleScale = DirectX::XMFLOAT3(1.5f, 0.35f, 1.0f);        // ステージ名中のスケール

	// モデル情報
	typedef std::unordered_map<std::wstring, Renderer::Model> ModelHash;
	std::unique_ptr<ModelHash> modelManager;
	Renderer::Model model;


public:
	void Init() override;
	void UnInit() override;
	void Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm) override;
	void Draw(Camera2D& _camera2D) override;
	void DrawShadowMap() override {};
	// パラメータ保存
	void SaveParameters() override;
	// パラメータ読み込み
	void LoadParameters()  override;
	// Set関数
	void SetIMGUI() override;
};