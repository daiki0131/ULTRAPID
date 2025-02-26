#include "ItemObject.h"

using namespace DirectX;

// �R���X�g���N�^
ItemObject::ItemObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:GameObject(_pos, _rot, _scale, _color)
{
	state = ItemObject::TOUCH_ITEM; // ��Ԃ�������Ԃŏ�����

	// �W�I���g���A�j���[�V�����̃o�b�t�@���e��ݒ�
	gsb.center = _pos;
	gsb.elapsedTime = 0;
}

// �X�V����
void ItemObject::Update(float _deltaTime, float _realRatio, float _alphaDecayRate)
{
	switch (state)
	{
	case ItemObject::TOUCH_ITEM:   // �G��邱�Ƃ��\
		// �擾�\�����������Ă����
		// �j��\�ɂ���
		if (breakRatio <= _realRatio)
		{
			color = defaultColor;
			isBreakable = true;
		}
		// �j�󕉉ׂ̏ꍇ�͐F�����ɋ߂��F�ɂ���
		else
		{
			color = XMFLOAT4(0.1f, 0.1f, 0.1f, defaultColor.w);
			isBreakable = false;
		}
		break;
	case ItemObject::WAIT:      // ���Ɏ擾�\�ɂȂ�܂ł̑ҋ@����

		returnCount += _deltaTime; // �擾�\�܂ł̃J�E���g��i�߂�

		// �������Ԃ𖞂�������
		if (returnCount > gettableTime)
		{
			returnCount = 0.0f;
			state = ItemObject::TOUCH_ITEM;
		}
		break;
	case ItemObject::EXPLOSION: // ���U��ԁi�A�j���[�V�����Đ��j

		// ���Ԍo�߂Ŕj�Ђ𓧖��ɂ���
		fragmentAlpha = std::max(0.0f, fragmentAlpha - _deltaTime * _alphaDecayRate);

		gsb.elapsedTime += _deltaTime; // �A�j���[�V�����̃J�E���g��i�߂�
		// ���l��0�ɂȂ�����
		// �ҋ@��Ԃɐ؂�ւ�
		if (fragmentAlpha <= 0.0f)
		{
			state = ItemObject::WAIT;
			gsb.elapsedTime = 0;
		}
		break;
	default:
		break;
	}
}

// �o�E���f�B���O�X�t�B�A�ݒ�
void ItemObject::SetBoundingSphere(DirectX::XMFLOAT3 _center, float _radius)
{
	boundingSphere.center = _center;
	boundingSphere.radius = _radius;
}

// ��Ԃ�ݒ�
void ItemObject::SetState(ITEM_STATE _state)
{
	state = _state;

	// ���U���̂݌o�ߎ��Ԃ�0�ɐݒ�
	if (state == ItemObject::EXPLOSION)
	{
		gsb.elapsedTime = 0;
	}
}