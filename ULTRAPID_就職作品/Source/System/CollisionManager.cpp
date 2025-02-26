#include "CollisionManager.h"
#include "Player.h"
#include "PlayerController.h"
#include "ItemObject.h"
#include "BoxObject.h"
#include "xa2.h"

using namespace DirectX;

// プレイヤーとアイテムの当たり判定
bool CollisionManager::SphereCollisionPlayerAndItem(Player& _player, ItemObject& _item)
{
	// 通常判定用のプレイヤーのバウンディングスフィア
	Collision::BoundingSphere playerBs1 = _player.GetBoundingSphere();

	// アイテムとの判定用のプレイヤーのバウンディングスフィア
	Collision::BoundingSphere playerBs2 = _player.GetItemBoundingSphere();

	// アイテムのバウンディングスフィア
	Collision::BoundingSphere itemBs = _item.GetBoundingSphere();

	// 球と球の当たり判定を実行
	if (Collision::IsHitSphereAndSphere(playerBs2, itemBs))
	{
		// 破壊可能の場合そのままtrueでreturn
		if (_item.GetIsBreakable())
		{
			return true;
		}
		// 破壊不可のときはプレイヤーを押し戻して
		// 衝突SEを再生する
		else
		{
			_player.SetPosition(Collision::AdjustPositionSphereAndSphere(playerBs1, itemBs));
			XA_Play(NO_BREAK_SE, NoBreakSEVol * volRatio);
			XA_SetSpeed(NO_BREAK_SE, NoBreakSESpeed);
		}
	}

	return false;
}

// プレイヤーの移動可能かどうかの判定を行う
void CollisionManager::MovePlayerCollisionCheck(
	std::unique_ptr<PlayerController>& _player, 
	std::vector<Collision::AABB>& grounds, 
	std::vector<Collision::AABB>& walls, 
	std::vector<Collision::Triangle>& triangles)
{
	// 次に移動するプレイヤーの座標
	XMFLOAT3 movePosition = _player->GetNewPosition();

	// 移動前の座標
	XMFLOAT3 oldPosition = _player->GetOldPosition();

	// 方向ベクトルを取得
	Vec3 moveVec = Vec3::SetVector(movePosition) - Vec3::SetVector(oldPosition);
	moveVec = moveVec.Normalize(); // 正規化

	// プレイヤーのバウンディングスフィア
	Collision::BoundingSphere bs = _player->GetBoundingSphere();

	// 中心座標からさらに半径分を加算する
	movePosition.x += bs.radius * moveVec.x;
	movePosition.z += bs.radius * moveVec.z;

	// 移動距離分の線分
	Collision::Segment seg = { oldPosition, movePosition };

	// 衝突した座標を保持する変数
	XMFLOAT3 hitPosition{};

	// 壁との判定
	for (auto& wall : walls)
	{
		// 線分と直方体の当たり判定実行
		if (Collision::IsHitLineAndBox(seg, wall, hitPosition))
		{
			// 衝突座標からプレイヤーの半径分を戻す
			if (moveVec.x != 0.0f)
			{
				moveVec.x *= -1;
				hitPosition.x += bs.radius * moveVec.x;
			}

			if (moveVec.z != 0.0f)
			{
				moveVec.z *= -1;
				hitPosition.z += bs.radius * moveVec.z;
			}

			
			// プレイヤー座標を設定
			_player->SetNewPosition(hitPosition);
			return;
		}
	}
	
	// 地面との判定
	for (auto& ground : grounds)
	{
		// 線分と直方体の当たり判定実行
		if (Collision::IsHitLineAndBox(seg, ground, hitPosition))
		{
			// 衝突座標からプレイヤーの半径分を戻す
			if (moveVec.x != 0.0f)
			{
				moveVec.x *= -1;
				hitPosition.x += bs.radius * moveVec.x;
			}

			if (moveVec.z != 0.0f)
			{
				moveVec.z *= -1;
				hitPosition.z += bs.radius * moveVec.z;
			}

			if (moveVec.y != 0.0f)
			{
				moveVec.y *= -1;
				hitPosition.y += bs.radius * moveVec.y;
			}

			// プレイヤー座標を設定
			_player->SetNewPosition(hitPosition);
			return;
		}
	}

	// 坂道との判定
	for (auto& triangle : triangles)
	{
		// 線分と平面の当たり判定実行
		if (Collision::IsHitSegmentAndPlane(seg, triangle, hitPosition))
		{
			// プレイヤー座標を設定
			_player->SetNewPosition(hitPosition);
			return;
		}
	}

	// 衝突が確認できない場合は正式な位置に移動させる
	_player->SetPosition(_player->GetNewPosition());

}

// ジャンプ可否の判定処理
bool CollisionManager::JumpPlayerCollisionCheck(
	std::unique_ptr<PlayerController>& _player, 
	std::vector<Collision::AABB>& grounds, 
	std::vector<Collision::AABB>& walls, 
	std::vector<Collision::Triangle>& triangles)
{
	// プレイヤーのバウンディングスフィア
	Collision::BoundingSphere bs = _player->GetBoundingSphere();

	// プレイヤーの座標を取得
	XMFLOAT3 downPoint = _player->GetPosition();

	// プレイヤーの半径分下の座標を取得
	// このポイントで衝突が確認できれば地面と判断する
	downPoint.y -= bs.radius;

	// 地面との判定
	for (auto& ground : grounds)
	{
		// 点と直方体の当たり判定実行
		if (Collision::IsHitPointAndBox(downPoint, ground))
		{
			return true;
		}
	}

	// 壁との判定
	for (auto& wall : walls)
	{
		// 点と直方体の当たり判定実行
		if (Collision::IsHitPointAndBox(downPoint, wall))
		{
			return true;
		}
	}

	XMFLOAT3 playerPos = _player->GetPosition();
	XMFLOAT3 collisionNormal = _player->GetCollisionNormal();

	collisionNormal.x *= -1;
	collisionNormal.y *= -1;
	collisionNormal.z *= -1;

	// 坂道との判定
	for (auto& triangle : triangles)
	{
		// 衝突した座標を保持する
		XMFLOAT3 hitPosition{};

		// 線分の方向を collisionNormal に基づいて決定
		XMFLOAT3 segmentEnd = {
			playerPos.x + collisionNormal.x * bs.radius,
			playerPos.y + collisionNormal.y * bs.radius,
			playerPos.z + collisionNormal.z * bs.radius
		};

		// 中心から法線方向に伸ばした線分
		Collision::Segment seg = { playerPos, segmentEnd };

		// 線分と平面の当たり判定を実行
		if (Collision::IsHitSegmentAndPlane(seg, triangle, hitPosition))
		{
			return true;
		}
	}

	return false;
}

// 衝突時の押し戻し処理
void CollisionManager::ExecuteCollisionResponse(
	std::unique_ptr<PlayerController>& _player,
	std::vector<Collision::AABB>& grounds,
	std::vector<std::reference_wrapper<BoxObject>> walls,
	std::vector<Collision::Triangle>& triangles
)
{
	// プレイヤーのバウンディングスフィア
	Collision::BoundingSphere& bs1 = _player->GetBoundingSphere();

	// アイテムとの判定用バウンディングスフィア
	Collision::BoundingSphere& bs2 = _player->GetItemBoundingSphere();

	// 各バウンディングスフィアの中心座標を更新する
	bs1.center = _player->GetNewPosition();
	bs2.center = bs1.center;

	// 衝突した面の法線
	XMFLOAT3 normal{};

	// 調整した座標
	XMFLOAT3 adjustPos{};

	bool isWallHit = false;

	// 壁との判定
	for (auto& wall : walls)
	{
		// AABB取得
		Collision::AABB aabb = wall.get().GetAABB();

		// 壁のデフォルトカラーを取得
		XMFLOAT3 tempColor = wall.get().GetRGBColor();

		// 球と直方体の当たり判定実行
		if (Collision::IsHitSphereAndBox(bs1, aabb, adjustPos, normal))
		{
			// 衝突して押し戻した位置を設定する
			_player->SetPosition(adjustPos);
			_player->SetNewPosition(adjustPos);
			_player->SetCollisionNormal(normal);

			// プレイヤーと衝突していれば壁のカラーを変える
			wall.get().SetRGBColor(XMFLOAT3(1, 0, 0));
			isWallHit = true;
		}
	}

	// 壁にヒットしていれば、SEを再生する
	if (isWallHit)
	{
		if (seTimerCount >= WallHitSeTimerCount)
		{
			XA_Play(WALL_SE, WallSEVol * volRatio);
			XA_SetSpeed(WALL_SE, WallSESpeed);
			seTimerCount = 0;
		}

		seTimerCount++;
		
		// プレイヤーに壁に衝突していることを伝える
		_player->SetIsWallHit(true);
	}
	else
	{
		// プレイヤーに壁に衝突していないことを伝える
		_player->SetIsWallHit(false);

		seTimerCount = WallHitSeTimerCount;
	}

	// 地面との判定
	for (auto& ground : grounds)
	{
		// 球と直方体の当たり判定実行
		if (Collision::IsHitSphereAndBox(bs1, ground, adjustPos, normal))
		{
			// 衝突して押し戻した位置を設定する
			_player->SetPosition(adjustPos);
			_player->SetNewPosition(adjustPos);
			_player->SetCollisionNormal(normal);
		}
	}

	// 坂道との判定
	for (auto& triangle : triangles)
	{
		// 球と平面の当たり判定実行
		if (Collision::CheckSpherePlaneCollision(triangle, bs1, adjustPos, normal))
		{
			// 衝突して押し戻した位置を設定する
			_player->SetPosition(adjustPos);
			_player->SetNewPosition(adjustPos);
			_player->SetCollisionNormal(normal);
		}
	}
}