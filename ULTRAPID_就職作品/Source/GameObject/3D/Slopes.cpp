#include "Slopes.h"
#include "Geometry.h"

using namespace DirectX;

// コンストラクタ
Slopes::Slopes(std::vector<SlopeData>& _data)
{
	slopes.resize(_data.size());
	instanceDetas.reserve(_data.size());  // 事前に容量を確保

#ifdef _DEBUG
	debugObjDatas.reserve(_data.size());
	debugWorldMatrixs.resize(_data.size());
#endif

	for (int i = 0; i < slopes.size(); i++)
	{
		// オブジェクト生成
		slopes[i] = std::make_unique<SlopeObject>(
			_data[i].position,
			_data[i].rotation,
			_data[i].scaling,
			_data[i].color);

		// 転置したワールド変換行列設定
		slopes[i]->SetMatrixWorld(XMMatrixTranspose(slopes[i]->GetMatrixWorld()));

#ifdef _DEBUG
		float sphereRadius = slopes[i]->GetBoundingSphere().radius * 2.0f;
		XMFLOAT3 sphereScale = XMFLOAT3(sphereRadius, sphereRadius, sphereRadius);
		debugWorldMatrixs[i] = XMMatrixTranspose(GameObject::CalculateMatrixWorld(
			_data[i].position,
			sphereScale,
			_data[i].rotation));
#endif
	}

	// 頂点情報を設定
	// 坂道メッシュで設定
	Geometry::VertexData vd = Geometry::GetSlopeVertexData();
	model.indexBuffer = vd.indexBuffer;
	model.vertexBuffer = vd.vertexBuffer;
	model.numIndex = vd.indexCount;
	model.texture = nullptr;
	model.matrixUV = XMMatrixIdentity();

#ifdef _DEBUG
	vd = Geometry::GetSphereVertexData();
	sphereModel.indexBuffer = vd.indexBuffer;
	sphereModel.vertexBuffer = vd.vertexBuffer;
	sphereModel.numIndex = vd.indexCount;
	sphereModel.texture = nullptr;
	sphereModel.matrixUV = XMMatrixIdentity();
#endif
}

// インスタンシング描画
void Slopes::InstancingDraw()
{
	instanceDetas.clear();

	for (auto& slope : slopes)
	{
		if (slope->GetIsActive()) // 描画可能かどうか
		{
			instanceDetas.push_back({ slope->GetMatrixWorld(), slope->GetColor() });
		}
	}

	// インスタンシングで描画
	Renderer::DrawObjectInstancing(model, instanceDetas, mat, useVS, usePS, false, false, true);
}

#ifdef _DEBUG
void Slopes::DebugSphereDraw()
{
	debugObjDatas.clear();

	for (int i = 0; i < slopes.size(); i++)
	{
		if (slopes[i]->GetIsCollisionCheck()) 
		{
			debugObjDatas.push_back({ debugWorldMatrixs[i] ,slopes[i]->GetDebugSphereColor() });
		}
	}

	Renderer::DrawObjectInstancing(sphereModel, debugObjDatas, std::nullopt, Shader::VS_AMBIENT_INSTANCING, Shader::PS_COLOR, false, false, false);
}
#endif
