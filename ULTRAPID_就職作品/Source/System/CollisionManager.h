#pragma once
#include "Collision.h"
#include <memory>

class Player;
class PlayerController;
class Items;
class ItemObject;
class BoxObject;

//===========================================//
// �Q�[�����̓����蔻����Ǘ�����N���X
//===========================================//
class CollisionManager
{
public:
	static constexpr int WallHitSeTimerCount = 60;

private:
	int seTimerCount = 60;

public:
	// �v���C���[�ƃA�C�e���Ƃ̓����蔻��
	bool SphereCollisionPlayerAndItem(Player& _player, ItemObject& _item);
	
	// �v���C���[�ړ�����̓����蔻����s��
	void MovePlayerCollisionCheck(
		std::unique_ptr<PlayerController>& _player,  // �v���C���[
		std::vector<Collision::AABB>& grounds,       // �n�ʃI�u�W�F�N�g
		std::vector<Collision::AABB>& walls,         // �ǃI�u�W�F�N�g
		std::vector<Collision::Triangle>& triangles  // �⓹�i�O�p�`���ʁj
	);

	// �v���C���[�W�����v����̓����蔻����s��
	bool JumpPlayerCollisionCheck(
		std::unique_ptr<PlayerController>& _player, // �v���C���[
		std::vector<Collision::AABB>& grounds,       // �n�ʃI�u�W�F�N�g
		std::vector<Collision::AABB>& walls,         // �ǃI�u�W�F�N�g
		std::vector<Collision::Triangle>& triangles  // �⓹�i�O�p�`���ʁj
	);

	// �Փ˂̉������s���i�߂荞�݉����߂������j
	void ExecuteCollisionResponse(
		std::unique_ptr<PlayerController>& _player, // �v���C���[
		std::vector<Collision::AABB>& grounds,       // �n�ʃI�u�W�F�N�g
		std::vector<std::reference_wrapper<BoxObject>> walls,         // �ǃI�u�W�F�N�g
		std::vector<Collision::Triangle>& triangles  // �⓹�i�O�p�`���ʁj
	);
};