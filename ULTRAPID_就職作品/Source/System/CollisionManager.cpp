#include "CollisionManager.h"
#include "Player.h"
#include "PlayerController.h"
#include "ItemObject.h"
#include "BoxObject.h"
#include "xa2.h"

using namespace DirectX;

// �v���C���[�ƃA�C�e���̓����蔻��
bool CollisionManager::SphereCollisionPlayerAndItem(Player& _player, ItemObject& _item)
{
	// �ʏ픻��p�̃v���C���[�̃o�E���f�B���O�X�t�B�A
	Collision::BoundingSphere playerBs1 = _player.GetBoundingSphere();

	// �A�C�e���Ƃ̔���p�̃v���C���[�̃o�E���f�B���O�X�t�B�A
	Collision::BoundingSphere playerBs2 = _player.GetItemBoundingSphere();

	// �A�C�e���̃o�E���f�B���O�X�t�B�A
	Collision::BoundingSphere itemBs = _item.GetBoundingSphere();

	// ���Ƌ��̓����蔻������s
	if (Collision::IsHitSphereAndSphere(playerBs2, itemBs))
	{
		// �j��\�̏ꍇ���̂܂�true��return
		if (_item.GetIsBreakable())
		{
			return true;
		}
		// �j��s�̂Ƃ��̓v���C���[�������߂���
		// �Փ�SE���Đ�����
		else
		{
			_player.SetPosition(Collision::AdjustPositionSphereAndSphere(playerBs1, itemBs));
			XA_Play(NO_BREAK_SE, NoBreakSEVol * volRatio);
			XA_SetSpeed(NO_BREAK_SE, NoBreakSESpeed);
		}
	}

	return false;
}

// �v���C���[�̈ړ��\���ǂ����̔�����s��
void CollisionManager::MovePlayerCollisionCheck(
	std::unique_ptr<PlayerController>& _player, 
	std::vector<Collision::AABB>& grounds, 
	std::vector<Collision::AABB>& walls, 
	std::vector<Collision::Triangle>& triangles)
{
	// ���Ɉړ�����v���C���[�̍��W
	XMFLOAT3 movePosition = _player->GetNewPosition();

	// �ړ��O�̍��W
	XMFLOAT3 oldPosition = _player->GetOldPosition();

	// �����x�N�g�����擾
	Vec3 moveVec = Vec3::SetVector(movePosition) - Vec3::SetVector(oldPosition);
	moveVec = moveVec.Normalize(); // ���K��

	// �v���C���[�̃o�E���f�B���O�X�t�B�A
	Collision::BoundingSphere bs = _player->GetBoundingSphere();

	// ���S���W���炳��ɔ��a�������Z����
	movePosition.x += bs.radius * moveVec.x;
	movePosition.z += bs.radius * moveVec.z;

	// �ړ��������̐���
	Collision::Segment seg = { oldPosition, movePosition };

	// �Փ˂������W��ێ�����ϐ�
	XMFLOAT3 hitPosition{};

	// �ǂƂ̔���
	for (auto& wall : walls)
	{
		// �����ƒ����̂̓����蔻����s
		if (Collision::IsHitLineAndBox(seg, wall, hitPosition))
		{
			// �Փˍ��W����v���C���[�̔��a����߂�
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

			
			// �v���C���[���W��ݒ�
			_player->SetNewPosition(hitPosition);
			return;
		}
	}
	
	// �n�ʂƂ̔���
	for (auto& ground : grounds)
	{
		// �����ƒ����̂̓����蔻����s
		if (Collision::IsHitLineAndBox(seg, ground, hitPosition))
		{
			// �Փˍ��W����v���C���[�̔��a����߂�
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

			// �v���C���[���W��ݒ�
			_player->SetNewPosition(hitPosition);
			return;
		}
	}

	// �⓹�Ƃ̔���
	for (auto& triangle : triangles)
	{
		// �����ƕ��ʂ̓����蔻����s
		if (Collision::IsHitSegmentAndPlane(seg, triangle, hitPosition))
		{
			// �v���C���[���W��ݒ�
			_player->SetNewPosition(hitPosition);
			return;
		}
	}

	// �Փ˂��m�F�ł��Ȃ��ꍇ�͐����Ȉʒu�Ɉړ�������
	_player->SetPosition(_player->GetNewPosition());

}

// �W�����v�ۂ̔��菈��
bool CollisionManager::JumpPlayerCollisionCheck(
	std::unique_ptr<PlayerController>& _player, 
	std::vector<Collision::AABB>& grounds, 
	std::vector<Collision::AABB>& walls, 
	std::vector<Collision::Triangle>& triangles)
{
	// �v���C���[�̃o�E���f�B���O�X�t�B�A
	Collision::BoundingSphere bs = _player->GetBoundingSphere();

	// �v���C���[�̍��W���擾
	XMFLOAT3 downPoint = _player->GetPosition();

	// �v���C���[�̔��a�����̍��W���擾
	// ���̃|�C���g�ŏՓ˂��m�F�ł���Βn�ʂƔ��f����
	downPoint.y -= bs.radius;

	// �n�ʂƂ̔���
	for (auto& ground : grounds)
	{
		// �_�ƒ����̂̓����蔻����s
		if (Collision::IsHitPointAndBox(downPoint, ground))
		{
			return true;
		}
	}

	// �ǂƂ̔���
	for (auto& wall : walls)
	{
		// �_�ƒ����̂̓����蔻����s
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

	// �⓹�Ƃ̔���
	for (auto& triangle : triangles)
	{
		// �Փ˂������W��ێ�����
		XMFLOAT3 hitPosition{};

		// �����̕����� collisionNormal �Ɋ�Â��Č���
		XMFLOAT3 segmentEnd = {
			playerPos.x + collisionNormal.x * bs.radius,
			playerPos.y + collisionNormal.y * bs.radius,
			playerPos.z + collisionNormal.z * bs.radius
		};

		// ���S����@�������ɐL�΂�������
		Collision::Segment seg = { playerPos, segmentEnd };

		// �����ƕ��ʂ̓����蔻������s
		if (Collision::IsHitSegmentAndPlane(seg, triangle, hitPosition))
		{
			return true;
		}
	}

	return false;
}

// �Փˎ��̉����߂�����
void CollisionManager::ExecuteCollisionResponse(
	std::unique_ptr<PlayerController>& _player,
	std::vector<Collision::AABB>& grounds,
	std::vector<std::reference_wrapper<BoxObject>> walls,
	std::vector<Collision::Triangle>& triangles
)
{
	// �v���C���[�̃o�E���f�B���O�X�t�B�A
	Collision::BoundingSphere& bs1 = _player->GetBoundingSphere();

	// �A�C�e���Ƃ̔���p�o�E���f�B���O�X�t�B�A
	Collision::BoundingSphere& bs2 = _player->GetItemBoundingSphere();

	// �e�o�E���f�B���O�X�t�B�A�̒��S���W���X�V����
	bs1.center = _player->GetNewPosition();
	bs2.center = bs1.center;

	// �Փ˂����ʂ̖@��
	XMFLOAT3 normal{};

	// �����������W
	XMFLOAT3 adjustPos{};

	bool isWallHit = false;

	// �ǂƂ̔���
	for (auto& wall : walls)
	{
		// AABB�擾
		Collision::AABB aabb = wall.get().GetAABB();

		// �ǂ̃f�t�H���g�J���[���擾
		XMFLOAT3 tempColor = wall.get().GetRGBColor();

		// ���ƒ����̂̓����蔻����s
		if (Collision::IsHitSphereAndBox(bs1, aabb, adjustPos, normal))
		{
			// �Փ˂��ĉ����߂����ʒu��ݒ肷��
			_player->SetPosition(adjustPos);
			_player->SetNewPosition(adjustPos);
			_player->SetCollisionNormal(normal);

			// �v���C���[�ƏՓ˂��Ă���Εǂ̃J���[��ς���
			wall.get().SetRGBColor(XMFLOAT3(1, 0, 0));
			isWallHit = true;
		}
	}

	// �ǂɃq�b�g���Ă���΁ASE���Đ�����
	if (isWallHit)
	{
		if (seTimerCount >= WallHitSeTimerCount)
		{
			XA_Play(WALL_SE, WallSEVol * volRatio);
			XA_SetSpeed(WALL_SE, WallSESpeed);
			seTimerCount = 0;
		}

		seTimerCount++;
		
		// �v���C���[�ɕǂɏՓ˂��Ă��邱�Ƃ�`����
		_player->SetIsWallHit(true);
	}
	else
	{
		// �v���C���[�ɕǂɏՓ˂��Ă��Ȃ����Ƃ�`����
		_player->SetIsWallHit(false);

		seTimerCount = WallHitSeTimerCount;
	}

	// �n�ʂƂ̔���
	for (auto& ground : grounds)
	{
		// ���ƒ����̂̓����蔻����s
		if (Collision::IsHitSphereAndBox(bs1, ground, adjustPos, normal))
		{
			// �Փ˂��ĉ����߂����ʒu��ݒ肷��
			_player->SetPosition(adjustPos);
			_player->SetNewPosition(adjustPos);
			_player->SetCollisionNormal(normal);
		}
	}

	// �⓹�Ƃ̔���
	for (auto& triangle : triangles)
	{
		// ���ƕ��ʂ̓����蔻����s
		if (Collision::CheckSpherePlaneCollision(triangle, bs1, adjustPos, normal))
		{
			// �Փ˂��ĉ����߂����ʒu��ݒ肷��
			_player->SetPosition(adjustPos);
			_player->SetNewPosition(adjustPos);
			_player->SetCollisionNormal(normal);
		}
	}
}