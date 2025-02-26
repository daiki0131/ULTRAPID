#pragma once
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include "SceneBase.h"
#include "CollisionManager.h"
#include "Particle.h"
#include "Camera.h"
#include "Object2D.h"
#include "Player.h"
#include "Boxs.h"
#include "Items.h"
#include "Light.h"
#include "Frustum.h"

//===========================================//
// タイトルシーンクラス
//===========================================//
class TitleScene : public SceneBase
{
public:
	const float MinLimitPos = 100.0f;                  // 描画するプレイヤーの最小の限界座標
	const float MaxLimitPos = 200.0f;                  // 描画するプレイヤーの最大の限界座標
	const float MinLimitSpeed = 0.1f;                  // 描画するプレイヤーの最小速度
	const float MaxLimitSpeed = 1.5f;                  // 描画するプレイヤーの最大速度
	const float MaxTexPos = 2.0f;                      // 2Dテクスチャの座標調整値
	const float MaxTexScale = 3.0f;                    // 2Dテクスチャのスケール調整値

private:
	std::unique_ptr<Object2D> titleTexture;            // タイトル名テクスチャ
	std::unique_ptr<Object2D> pressEnterKeyTexture;    // PressEnterKeyテクスチャ
	std::unique_ptr<Boxs> ground;                      // 地面オブジェクト
	std::unique_ptr<Items> item;                       // アイテムオブジェクト
	std::unique_ptr<Player> player;                    // プレイヤーオブジェクト
	std::unique_ptr<TitleSceneCamera> camera3D;        // カメラ3D
	std::unique_ptr<Light> sceneLight;                 // ライト
	std::unique_ptr<ItemEffect> itemFx;                // アイテムエフェクト
	CollisionManager collisionManager;                 // CollisionManagerオブジェクト
	Frustum frustum;

	float limitPlayerPositionX = MinLimitPos;          // 描画するプレイヤーの限界座標
	float playerSpeed = MinLimitSpeed;                 // 描画するプレイヤーの速度

#ifdef _DEBUG
	std::unique_ptr<DebugCamera> debugCamera3D;        // デバッグ専用カメラ
	bool isDebugCameraChange = false;                  // デバッグカメラと通常カメラの切り替え
	bool isDebugDrawUI = true;                         // UIの表示・非表示
#endif

	// モデル情報
	typedef std::unordered_map<std::wstring, Renderer::Model> ModelHash;
	std::unique_ptr<ModelHash> modelManager;
	Renderer::Model model;

public:
	void Init() override;
	void UnInit() override;
	void Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm) override;
	void Draw(Camera2D& _camera2D) override;
	void DrawShadowMap() override;
	// パラメータ保存
	void SaveParameters() override;
	// パラメータ読み込み
	void LoadParameters()  override;
	// 当たり判定実行関数
	void CheckObjectCollision();
	// Set関数
	void SetLimitPlayerPositionX(float value) { limitPlayerPositionX = value; }
	void SetPlayerSpeed(float value) { playerSpeed = value; }
	void SetIMGUI() override;
	// Get関数
	Player& GetPlayer() { return *player; }
	float GetLimitPlayerPositionX() const { return limitPlayerPositionX; }
	float GetPlayerSpeed() const { return playerSpeed; }
};