#include "SlopeObject.h"
#include "Geometry.h"

using namespace DirectX;

// コンストラクタ
SlopeObject::SlopeObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:GameObject(_pos, _rot, _scale, _color)
{
	// 各平面を設定
	SetPlane();
}

// 平面の設定
void SlopeObject::SetPlane()
{
	// 移動、スケール、回転行列を個別に作成
	XMMATRIX matrixMove = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX matrixScale = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
	XMMATRIX matrixRotateY = XMMatrixRotationY(XMConvertToRadians(rotation.y));
	XMMATRIX matrixRotateX = XMMatrixRotationX(XMConvertToRadians(rotation.x));
	XMMATRIX matrixRotateZ = XMMatrixRotationZ(XMConvertToRadians(rotation.z));

	// 回転行列をX, Y, Zの順に合成
	XMMATRIX matrixRotate = matrixRotateX * matrixRotateY * matrixRotateZ;

	// 最終的な変換行列
	XMMATRIX transform = matrixScale * matrixRotate * matrixMove;

	// ジオメトリデータを取得
	const Geometry::VertexData vd = Geometry::GetSlopeVertexData();

	// 頂点データの位置情報を配列に格納
	std::vector<XMFLOAT3> verticesPos;

	verticesPos.reserve(vd.vertices.size());

	for (const auto& vertex : vd.vertices)
	{
		verticesPos.push_back(vertex.position);
	}

	// メッシュの面情報を取得
	planes = GetPlanesFromMesh(verticesPos, vd.indices, transform);

	// バウンディングスフィアの設定
	boundingSphere.center = position;

	// 最大スケーリング値を求めて半径に適用
	float maxLength = sqrt(scaling.x * scaling.x + scaling.y * scaling.y + scaling.z * scaling.z);

	boundingSphere.radius = maxLength * 0.505f;
}

// 法線と平面方程式の係数を計算
Collision::Triangle SlopeObject::CalculatePlane(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const DirectX::XMFLOAT3& v3)
{
	// 2つのエッジベクトルを計算
	XMVECTOR edge1 = XMVectorSet(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z, 0.0f);
	XMVECTOR edge2 = XMVectorSet(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z, 0.0f);

	// 外積で法線を計算
	XMVECTOR normalVec = XMVector3Cross(edge1, edge2);
	normalVec = XMVector3Normalize(normalVec);  // 正規化

	// 法線を保存
	XMFLOAT3 normal;
	XMStoreFloat3(&normal, normalVec);

	// 平面のd値を計算
	float d = -(normal.x * v1.x + normal.y * v1.y + normal.z * v1.z);

	// 平面を返す
	return { normal, d, { v1, v2, v3 } }; // 追加した頂点も含む
}

// 三角形の平面情報を抽出
std::vector<Collision::Triangle> SlopeObject::GetPlanesFromMesh(const std::vector<DirectX::XMFLOAT3>& vertices, const std::vector<UINT>& indices, const DirectX::XMMATRIX& transform)
{
	std::vector<Collision::Triangle> planes;

	// 頂点を変換する
	std::vector<XMFLOAT3> transformedVertices = TransformVertices(vertices, transform);

	// インデックスを3つごとに処理して三角形ごとに平面を計算
	for (size_t i = 0; i < indices.size(); i += 3) {
		UINT idx1 = indices[i];
		UINT idx2 = indices[i + 1];
		UINT idx3 = indices[i + 2];

		// 3つの頂点を取得
		const XMFLOAT3& v1 = transformedVertices[idx1];
		const XMFLOAT3& v2 = transformedVertices[idx2];
		const XMFLOAT3& v3 = transformedVertices[idx3];

		// 平面を計算
		Collision::Triangle plane = CalculatePlane(v1, v2, v3);
		planes.push_back(plane);
	}

	return planes;
}

//頂点を行列を使って計算 
std::vector<DirectX::XMFLOAT3> SlopeObject::TransformVertices(const std::vector<DirectX::XMFLOAT3>& vertices, const DirectX::XMMATRIX& transform)
{
	// 変換後の頂点を格納する配列
	std::vector<XMFLOAT3> transformedVertices;

	for (const auto& vertex : vertices)
	{
		// 現在の頂点をXMVECTORに変換
		XMVECTOR v = XMVectorSet(vertex.x, vertex.y, vertex.z, 1.0f);

		// 変換行列を適用して頂点を変換
		v = XMVector3Transform(v, transform);

		// 変換後のXMVECTORをXMFLOAT3に格納
		XMFLOAT3 transformed;
		XMStoreFloat3(&transformed, v);

		transformedVertices.push_back(transformed);
	}

	// 変換後の頂点リストを返す
	return transformedVertices;
}