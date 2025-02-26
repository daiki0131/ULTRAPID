#pragma once
#include "Collision.h"
#include <memory>

class Player;
class PlayerController;
class Items;
class ItemObject;
class BoxObject;

//===========================================//
// ゲーム内の当たり判定を管理するクラス
//===========================================//
class CollisionManager
{
public:
	static constexpr int WallHitSeTimerCount = 60;

private:
	int seTimerCount = 60;

public:
	// プレイヤーとアイテムとの当たり判定
	bool SphereCollisionPlayerAndItem(Player& _player, ItemObject& _item);
	
	// プレイヤー移動判定の当たり判定を行う
	void MovePlayerCollisionCheck(
		std::unique_ptr<PlayerController>& _player,  // プレイヤー
		std::vector<Collision::AABB>& grounds,       // 地面オブジェクト
		std::vector<Collision::AABB>& walls,         // 壁オブジェクト
		std::vector<Collision::Triangle>& triangles  // 坂道（三角形平面）
	);

	// プレイヤージャンプ判定の当たり判定を行う
	bool JumpPlayerCollisionCheck(
		std::unique_ptr<PlayerController>& _player, // プレイヤー
		std::vector<Collision::AABB>& grounds,       // 地面オブジェクト
		std::vector<Collision::AABB>& walls,         // 壁オブジェクト
		std::vector<Collision::Triangle>& triangles  // 坂道（三角形平面）
	);

	// 衝突の応答を行う（めり込み押し戻し処理）
	void ExecuteCollisionResponse(
		std::unique_ptr<PlayerController>& _player, // プレイヤー
		std::vector<Collision::AABB>& grounds,       // 地面オブジェクト
		std::vector<std::reference_wrapper<BoxObject>> walls,         // 壁オブジェクト
		std::vector<Collision::Triangle>& triangles  // 坂道（三角形平面）
	);
};