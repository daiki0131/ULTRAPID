#include "UVAnimation2D.h"
#include "DebugConsole.h"

// コンストラクタ
UVAnimation2D::UVAnimation2D(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:Object2D(_pos, _rot, _scale, _color)
{
}

// 更新処理
void UVAnimation2D::Update(float _deltaTime)
{
	if (!isAnimation) return; // アニメーションしなければreturn

	// 範囲のチェック
	if ((int)animCounter > animTable.size())
	{
		animCounter = 0;
	}
	// アニメーションIDをanimCounterに基づいて設定
	animID = animTable[(int)animCounter];

	// 横分割数が2以上
	if (divSide > 1)
	{
		// 横軸のUV値を計算
		uv.x = animID % divSide * (1.0f / (float)divSide);
	}
	else
	{
		uv.x = 1.0f;
	}

	// 縦分割数が2以上
	if (divVertical > 1)
	{
		// 縦軸のUV値を計算
		uv.y = animID / divSide * (1.0f / (float)divVertical);
	}
	else
	{
		uv.y = 1.0f;
	}

	animCounter += animSpeed * _deltaTime;   // アニメーションカウンターを進める

	// カウンターがテーブルサイズを超えたら最初に戻る
	if ((int)animCounter >= animTable.size())
	{
		animCounter = 0.0f;
		// ループしないならアニメーション終了
		if (!isLoopAnimation)
		{
			isAnimation = false;
		}
	}
}

// アニメーションのコマ数を設定
void UVAnimation2D::SetAnimationTable(int tableNum)
{
	animTable.resize(tableNum);

	for (int i = 0; i < animTable.size(); i++)
	{
		animTable[i] = i;
	}
}

// アニメーション再生
void UVAnimation2D::Play()
{
	isAnimation = true;
	animCounter = 0;
}

// アニメーション停止
void UVAnimation2D::Stop()
{
	isAnimation = false;
}