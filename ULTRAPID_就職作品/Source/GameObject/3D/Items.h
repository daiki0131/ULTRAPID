#pragma once
#include "ItemObject.h"
#include <vector>
#include <memory>

//===========================================//
// ゲーム内アイテムを一括で扱うクラス
//===========================================//
class Items
{
public:
	// アイテムデータ
	struct ItemData
	{
		DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 scaling{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 rotation{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 color{ 0.0f, 0.0f, 0.0f, 0.0f };
		ItemObject::ITEM_TYPE type;
	};

public:
	std::vector<std::unique_ptr<ItemObject>> items; // アイテムオブジェクトコンテナ

private:
	float smallBreakRatio = 0.0f;					// 小アイテム取得可能割合
	float middleBreakRatio = 0.3f;					// 中アイテム取得可能割合
	float bigBreakRatio = 0.7f;						// 大アイテム取得可能割合
	float megabigBreakRatio = 0.9f;					// 特大アイテム取得可能割合

	float smallAgainGettableTime = 5.0f;			// 小アイテム再取得可能時間
	float middleAgainGettableTime = 10.0f;			// 中アイテム再取得可能時間
	float bigAgainGettableTime = 15.0f;				// 大アイテム再取得可能時間
	float megabigAgainGettableTime = 20.0f;			// 特大アイテム再取得可能時間

	int smallBaseScore = 5;						    // 小アイテム獲得スコア数
	int middleBaseScore = 10;						// 中アイテム獲得スコア数
	int bigBaseScore = 30;							// 大アイテム獲得スコア数
	int megabigBaseScore = 300;						// 特大アイテム獲得スコア数

	float alphaDecayRate = 0.7f;					// 消滅時のα値減速割合

private:
	std::vector<Renderer::ObjectBuffer> instanceDatas;			            // Renderer送信用オブジェクトデータ
	Renderer::Model model;
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;			        // 使用する頂点シェーダー
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;			        // 使用するピクセルシェーダー
	Shader::VS_NAME useAnimationVS = Shader::VS_TEXTURE;      	// 消滅アニメーション時の頂点シェーダー
	Shader::PS_NAME useAnimationPS = Shader::PS_TEXTURE;         // 消滅アニメーション時のピクセルシェーダー
	Shader::GS_NAME useAnimationGS = Shader::GS_ITEM_ANIMATION;  // 消滅アニメーション時のジオメトリシェーダー
	Renderer::Material mat;							            // Renderer送信用マテリアル情報
	float collisionMultiplier = 1.25f;                          // プレイヤーとの当たり判定領域の倍率
	float realRatio = 1.0f;                                     // プレイヤーの速度割合

public:
	Items(std::vector<ItemData>& _data);
	~Items() {};
	void Update(float _deltaTime);
	// インスタンシング描画
	void InstancingDraw();
	void DrawInstancingShadowMap();
#ifdef _DEBUG
	void DebugSingleDraw();
#endif
	// 消滅アニメーション
	void ExplosionAnimation();
	// Set関数
	void SetModel(Renderer::Model& m) { model = m; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; };
	void SetAnimationShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps, Shader::GS_NAME _gs);
	void SetRealRatio(float _ratio) { realRatio = _ratio; }
	void SetMaterial(Renderer::Material _mat) { mat = _mat; }
	void SetBreakAllItemRatio(float smallRatio, float middleRatio, float bigRatio, float megabigRatio);
	// Get関数
	Renderer::Model& GetModel() { return model; }

};