#include "Player.h"
#include "Vector.h"
#include "DebugUI.h"
#include "GameManager.h"

using namespace DirectX;

// コンストラクタ
Player::Player(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:GameObject(_pos, _rot, _scale, _color)
{   
	// モーションブラーの過去座標の取得数
	oldPositionsMotionBlur.resize(4);
	// 初期値設定（プレイヤーの初期値に設定）
	OldPositionsMotionBlurUpdate(position, true);
	oldPosition = position;
	startPosition = position;
	defaultScale = scaling.x;

	// マテリアル設定
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.5f);
	mat.other = XMFLOAT4(0.02f, 0.0f, 0.0f, 0.0f);

	instanceDatas.reserve(MaxMotionBlurImprintNum);
}

// 更新処理（主に操作しない用）
void Player::Update(float _deltatime)
{
	// 速度割合を常に最大に
	speedRatio = 1.0f;

	// 座標の記録
	oldPosition = position;
	OldPositionsMotionBlurUpdate(position, false);

	// 回転処理
	RotationPlayer();

	// 当たり判定の中心点を更新
	boundingSphere.center = position;
	itemBounds.center = position;
}

// 描画処理
void Player::Draw()
{
	// ワールド変換行列計算
	matrixWorld = CalculateMatrixWorld(position, scaling, rotation);

	// Rendererに送るデータを格納
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose(matrixWorld);
	ob.color = color;
	
	// 通常描画
	Renderer::DrawObject(model, ob, mat, useVS, usePS, false, false, true);

}

void Player::DrawShadowMap()
{
	// ワールド変換行列計算
	matrixWorld = CalculateMatrixWorld(position, scaling, rotation);

	// Rendererに送るデータを格納
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose(matrixWorld);
	ob.color = color;

	// 通常描画
	Renderer::DrawObject(model, ob, std::nullopt, Shader::VS_SHADOW_MAP, Shader::PS_NULLPTR, false, true, false);
}

// モーションブラーのエフェクト動画
void Player::MortionBlurDraw()
{
	// ワールド変換行列を退避
	XMMATRIX tempMatrix = matrixWorld;

	// 過去の座標が正しい順番になるようにソートする
	std::vector<XMFLOAT3> sortedPositions(oldPositionsMotionBlur.size());

	for (size_t i = 0; i < oldPositionsMotionBlur.size(); i++)
	{
		// 最新のデータが先頭に来るようにコピー
		int index = (oldPositionsCountMotionBlur - 1 - i + static_cast<int>(oldPositionsMotionBlur.size())) % oldPositionsMotionBlur.size();
		sortedPositions[i] = oldPositionsMotionBlur[index];
	}

	instanceDatas.clear();

	// 過去の座標同士の間に残像エフェクトを置いていく
	for (int i = 0; i < motionBlurImprintNum; i++)
	{
		// 現在の残像の進行度（0.0～1.0）を計算
		float t = static_cast<float>(i / static_cast<float>(motionBlurImprintNum - 1));

		XMFLOAT3 drawPos; // 描画する残像の位置

		int segmentCount = static_cast<int>(sortedPositions.size());     // 保持している過去の位置の数
		float segmentSize = 1.0f / (segmentCount + 1); // 各区間のサイズ
		// t の進行度に基づいて適切な過去の位置区間を計算
		int segmentIndex = std::min(static_cast<int>(t / segmentSize), segmentCount - 1);
		float localT = (t - segmentIndex * segmentSize) / segmentSize;

		// 選択した区間で線形補間を行い、残像の位置を決定
		if (segmentIndex == 0)
		{
			// 最初の区間は現在位置と最初の過去位置の補間
			drawPos = Vec3::LerpXMFLOAT3(position, sortedPositions[0], localT);
		}
		else
		{
			// その他の区間は過去の2つの位置を補間
			drawPos = Vec3::LerpXMFLOAT3(sortedPositions[segmentIndex - 1], sortedPositions[segmentIndex], localT);
		}
		// ワールド変換行列計算
		matrixWorld = CalculateMatrixWorld(drawPos, scaling, rotation);

		// Rendererに送るデータを格納
		XMFLOAT4 blurColor = color; // 残像の色の透明度を計算
		blurColor.w = std::clamp(((motionBlurAlphaValue * speedRatio) - t) * 0.1f, 0.0f, 1.0f);
		instanceDatas.push_back({ XMMatrixTranspose(matrixWorld), blurColor });
		
	}
	// もし残像を描画するためのデータがあれば、インスタンシングで描画する
	if (motionBlurImprintNum > 0)
	{
		// インスタンシングで残像エフェクトを描画
		Renderer::DrawObjectInstancing(model, instanceDatas,mat, useMotionBlurVS, useMotionBlurPS, false, false, true);
	}

	// 退避した行列を元に戻す
	matrixWorld = tempMatrix;
}

// 座標を更新（TitleSceneでのみ使用）
void Player::AddPosition(DirectX::XMFLOAT3 _addPos)
{
	position.x += _addPos.x;
	position.y += _addPos.y;
	position.z += _addPos.z;

	newPosition = position;
}

// バウンディングスフィア設定
void Player::SetBoundingSphere(DirectX::XMFLOAT3 _center, float _radius)
{
	boundingSphere.center = _center;
	boundingSphere.radius = _radius;
}

// アイテムとの判定用のバウンディングスフィア
void Player::SetItemBoundingSphere(DirectX::XMFLOAT3 _center, float _radius)
{
	itemBounds.center = _center;
	itemBounds.radius = _radius;
}

// モーションブラーで使用する過去の座標を配列に格納する関数
void Player::OldPositionsMotionBlurUpdate(const DirectX::XMFLOAT3 _pos, const bool isAllSet)
{
	// 一つずつ格納する場合
	if (!isAllSet)
	{
		oldPositionsMotionBlur[oldPositionsCountMotionBlur] = _pos;

		oldPositionsCountMotionBlur++;

		// 配列のサイズに満たしたらカウントを0にする
		if (oldPositionsCountMotionBlur >= oldPositionsMotionBlur.size())
		{
			oldPositionsCountMotionBlur = 0;
		}
	}
	// 配列の要素すべてに格納する
	else
	{
		for (auto& oldPos : oldPositionsMotionBlur)
		{
			oldPos = _pos;
		}
		oldPositionsCountMotionBlur = 0;
	}
}

void Player::RotationPlayer()
{
	// ワールド変換行列から必要な情報を取得する
	XMFLOAT3 outScale; // スケール格納用
	XMFLOAT3 outPos;   // 座標格納用
	XMMATRIX matWrold = GetMatrixWorld(); // ワールド変換行列取得

	// 行列を分解して座標・回転・スケールを取得
	DirectX::XMVECTOR scale, rotationQuat, translation;
	DirectX::XMMatrixDecompose(&scale, &rotationQuat, &translation, matWrold);

	// 取得したスケール、位置をそれぞれ格納
	DirectX::XMStoreFloat3(&outScale, scale);
	DirectX::XMStoreFloat3(&outPos, translation);

	// 移動距離に応じて回転量を決める
	// 新しい位置と現在の位置のベクトル差を取得
	Vec3 moveV = Vec3::SetVector(newPosition) - Vec3::SetVector(outPos);
	moveV.y = 0.0f; // Y軸の回転は無視する（地面に沿った回転のみ）

	// 移動距離の長さを計算
	float rollDistance = moveV.Length();

	// 速度が出すぎて止まって見えないようにするため
	// ある程度の距離分に収める
	if (rollDistance >= MaxRollDistance * (scaling.x / defaultScale))
	{
		rollDistance = MaxRollDistance;
	}

	// 回転量の計算
	float rollRotation = rollDistance / (outScale.x * 0.5f);

	// 正規化
	moveV.Normalize();

	// 回転させる
	rotation.x += rollRotation;
	// 値が溢れないように自動的に調整する
	rotation.x = std::fmod(rotation.x, GameManager::FullRotationDegress);
}