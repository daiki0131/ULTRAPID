#include "Items.h"
#include "Geometry.h"

using namespace DirectX;

// コンストラクタ
Items::Items(std::vector<ItemData>& _data)
{
	items.resize(_data.size());
	instanceDatas.reserve(_data.size());

	for (int i = 0; i < items.size(); i++)
	{
		// オブジェクト生成
		items[i] = std::make_unique<ItemObject>(
			_data[i].position,
			_data[i].rotation,
			_data[i].scaling,
			_data[i].color);

		// ワールド変換行列を最初に計算（動かないオブジェクト）
		items[i]->SetMatrixWorld(XMMatrixTranspose(items[i]->GetMatrixWorld()));

		// バウンディングスフィア設定
		// radiusは判定の領域倍率を掛ける
		items[i]->SetBoundingSphere(_data[i].position, (_data[i].scaling.x * 0.5f) * collisionMultiplier);

		// そのオブジェクトの色を保持する（破壊不可のとき色を変えるため）
		items[i]->SetDefaultColor(_data[i].color);

		// タイプと後の値を設定
		// SetBreakRatio（破壊可能な速度割合）
		// SetGettableTime（取得後再生成までの時間）
		// SetScore（そのアイテムの種類のスコア数）
		switch (_data[i].type)
		{
		case ItemObject::SMALL:     // アイテム小
			items[i]->SetBreakRatio(smallBreakRatio);
			items[i]->SetGettableTime(smallAgainGettableTime);
			items[i]->SetScore(smallBaseScore);
			break;
		case ItemObject::MIDDLE:    // アイテム中
			items[i]->SetBreakRatio(middleBreakRatio);
			items[i]->SetGettableTime(middleAgainGettableTime);
			items[i]->SetScore(middleBaseScore);
			break;
		case ItemObject::BIG:       // アイテム大
			items[i]->SetBreakRatio(bigBreakRatio);
			items[i]->SetGettableTime(bigAgainGettableTime);
			items[i]->SetScore(bigBaseScore);
			break;
		case ItemObject::MEGA_BIG:  // アイテム特大
			items[i]->SetBreakRatio(megabigBreakRatio);
			items[i]->SetGettableTime(megabigAgainGettableTime);
			items[i]->SetScore(megabigBaseScore);
			break;

		default:
			break;
		}

		// タイプを設定
		items[i]->SetType(_data[i].type);
	}

	// 頂点情報を設定
	// スフィアメッシュで設定
	Geometry::VertexData vd = Geometry::GetSphereVertexData();
	model.indexBuffer = vd.indexBuffer;
	model.vertexBuffer = vd.vertexBuffer;
	model.numIndex = vd.indexCount;
	model.texture = nullptr;
	model.matrixUV = XMMatrixIdentity();
}

// 更新処理
void Items::Update(float _deltaTime)
{
	for (auto& item : items)
	{
		item->Update(_deltaTime, realRatio, alphaDecayRate);
	}
}

// インスタンシング描画
void Items::InstancingDraw()
{
	instanceDatas.clear();

	for (auto& item : items)
	{
		// アクティブ状態と当たり判定可能状態の場合のみ
		if (item->GetIsActive() && item->GetIsTouched())
		{
			instanceDatas.push_back({ item->GetMatrixWorld() ,item->GetColor() });
		}
	}

	// インスタンシングで描画
	Renderer::DrawObjectInstancing(model, instanceDatas, mat, useVS, usePS, false, false, true);
}

void Items::DrawInstancingShadowMap()
{
	instanceDatas.clear();

	for (auto& item : items)
	{
		// アクティブ状態と当たり判定可能状態の場合のみ
		if (item->GetIsActive() && item->GetIsTouched())
		{
			instanceDatas.push_back({ item->GetMatrixWorld() ,item->GetColor() });
		}
	}

	// インスタンシングで描画
	Renderer::DrawObjectInstancing(model, instanceDatas, std::nullopt, Shader::VS_INSTANCING_SHADOW_MAP, Shader::PS_NULLPTR, false, true, false);
}

#ifdef _DEBUG
void Items::DebugSingleDraw()
{
	
	for (auto& item : items)
	{
		// アクティブ状態と当たり判定可能状態の場合のみ
		if (item->GetIsActive() && item->GetIsTouched())
		{
			Renderer::ObjectBuffer ob;
			ob.matrixWorld = item->GetMatrixWorld();
			ob.color = item->GetColor();

			Renderer::DrawObject(model, ob, mat, Shader::VS_OBJECT, Shader::PS_SPECULER, true, false, true);
		}
	}
}
#endif

// 爆散アニメーション
void Items::ExplosionAnimation()
{
	for (auto& item : items)
	{
		// 爆散状態のとき
		if (item->GetState() == ItemObject::EXPLOSION)
		{
			Renderer::ObjectBuffer ob;
			// ワールド変換行列を設定
			ob.matrixWorld = item->GetMatrixWorld();

			// カラー設定（α値のみ時間経過による透明度を設定）
			ob.color = item->GetColor();
			ob.color.w = item->GetFragmentAlpha();

			// GPUに送信する用に設定したバッファを取得
			Renderer::ShaderAnimationGSBuffer gsb = item->GetGSBuffer();

			// 通常描画
			Renderer::DrawShaderAnimation(model, ob, gsb, std::nullopt, useAnimationVS, useAnimationPS, useAnimationGS, true);
		}
	}
}

// 爆散アニメーションで使用するシェーダーセット
// 頂点・ピクセル・ジオメトリシェーダー
void Items::SetAnimationShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps, Shader::GS_NAME _gs)
{
	useAnimationVS = _vs;
	useAnimationPS = _ps;
	useAnimationGS = _gs;
}

// 各アイテムのタイプに破壊可能な速度割合を設定
void Items::SetBreakAllItemRatio(float smallRatio, float middleRatio, float bigRatio, float megabigRatio)
{
	for (auto& item : items)
	{
		switch (item->GetType())
		{
		case ItemObject::SMALL:     // アイテム小
			item->SetBreakRatio(smallRatio);
			break;
		case ItemObject::MIDDLE:    // アイテム中
			item->SetBreakRatio(middleRatio);
			break;
		case ItemObject::BIG:       // アイテム大
			item->SetBreakRatio(bigRatio);
			break;
		case ItemObject::MEGA_BIG:  // アイテム特大
			item->SetBreakRatio(megabigRatio);
			break;
		default:
			break;
		}
	}
}