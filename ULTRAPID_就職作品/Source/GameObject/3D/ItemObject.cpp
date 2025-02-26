#include "ItemObject.h"

using namespace DirectX;

// コンストラクタ
ItemObject::ItemObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:GameObject(_pos, _rot, _scale, _color)
{
	state = ItemObject::TOUCH_ITEM; // 状態を可視化状態で初期化

	// ジオメトリアニメーションのバッファ内容を設定
	gsb.center = _pos;
	gsb.elapsedTime = 0;
}

// 更新処理
void ItemObject::Update(float _deltaTime, float _realRatio, float _alphaDecayRate)
{
	switch (state)
	{
	case ItemObject::TOUCH_ITEM:   // 触れることが可能
		// 取得可能割合を上回っていると
		// 破壊可能にする
		if (breakRatio <= _realRatio)
		{
			color = defaultColor;
			isBreakable = true;
		}
		// 破壊負荷の場合は色を黒に近い色にする
		else
		{
			color = XMFLOAT4(0.1f, 0.1f, 0.1f, defaultColor.w);
			isBreakable = false;
		}
		break;
	case ItemObject::WAIT:      // 次に取得可能になるまでの待機時間

		returnCount += _deltaTime; // 取得可能までのカウントを進める

		// 復活時間を満たしたら
		if (returnCount > gettableTime)
		{
			returnCount = 0.0f;
			state = ItemObject::TOUCH_ITEM;
		}
		break;
	case ItemObject::EXPLOSION: // 爆散状態（アニメーション再生）

		// 時間経過で破片を透明にする
		fragmentAlpha = std::max(0.0f, fragmentAlpha - _deltaTime * _alphaDecayRate);

		gsb.elapsedTime += _deltaTime; // アニメーションのカウントを進める
		// α値が0になったら
		// 待機状態に切り替え
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

// バウンディングスフィア設定
void ItemObject::SetBoundingSphere(DirectX::XMFLOAT3 _center, float _radius)
{
	boundingSphere.center = _center;
	boundingSphere.radius = _radius;
}

// 状態を設定
void ItemObject::SetState(ITEM_STATE _state)
{
	state = _state;

	// 爆散時のみ経過時間を0に設定
	if (state == ItemObject::EXPLOSION)
	{
		gsb.elapsedTime = 0;
	}
}