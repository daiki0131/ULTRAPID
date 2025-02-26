#include <algorithm>
#include "Collision.h"
#include "Vector.h"
#include "GameManager.h"
#include "DebugConsole.h"

using namespace DirectX;

ComPtr<ID3D11Buffer> Collision::sphereBoundsBuffer;
ComPtr<ID3D11Buffer> Collision::frustumPlaneBuffer;
ComPtr<ID3D11Buffer> Collision::playerSphereBoundsBuffer;
ComPtr<ID3D11Buffer> Collision::visibilityBuffer;
ComPtr<ID3D11Buffer> Collision::visibilityStagingBuffer;
ComPtr<ID3D11UnorderedAccessView> Collision::visibilityUAV;
ComPtr<ID3D11ShaderResourceView> Collision::sphereBoundsSRV;

// 初期化処理
void Collision::Init()
{
	HRESULT hr;

	// 衝突判定対象用バッファを作成
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Collision::BoundingSphere) * GameManager::Max3DobjectCount;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = sizeof(Collision::BoundingSphere);
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, sphereBoundsBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("sphereBoundsBuffer 作成失敗：Collision::Init内で発生", DebugConsole::ERROR_LOG);
	}

	// 衝突結果を保存するバッファを作成
	bufferDesc.ByteWidth = sizeof(uint32_t) * GameManager::Max3DobjectCount;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	hr = Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, visibilityBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("visibilityBuffer 作成失敗：Collision::Init内で発生", DebugConsole::ERROR_LOG);
	}

	// sphereBoundsBufferのGPU側での読み取り専用のバッファ作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = GameManager::Max3DobjectCount;

	hr = Renderer::GetDevice()->CreateShaderResourceView(sphereBoundsBuffer.Get(), &srvDesc, sphereBoundsSRV.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("sphereBoundsBuffer 作成失敗：Collision::Init内で発生", DebugConsole::ERROR_LOG);
	}

	// カメラのフラスタムバッファ作成
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.ByteWidth = sizeof(Collision::FrustumPlane);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	hr = Renderer::GetDevice()->CreateBuffer(&cbDesc, nullptr, frustumPlaneBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("frustumPlaneBuffer 作成失敗：Collision::Init内で発生", DebugConsole::ERROR_LOG);
	}

	// プレイヤーのスフィアバッファ作成
	cbDesc.ByteWidth = sizeof(Collision::BoundingSphere);
	hr = Renderer::GetDevice()->CreateBuffer(&cbDesc, nullptr, playerSphereBoundsBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("playerSphereBoundsBuffer 作成失敗：Collision::Init内で発生", DebugConsole::ERROR_LOG);
	}

	// visibilityBufferのCPU側での読み取り専用バッファ作成
	D3D11_BUFFER_DESC stagingDesc = {};
	stagingDesc.ByteWidth = sizeof(uint32_t) * GameManager::Max3DobjectCount;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = Renderer::GetDevice()->CreateBuffer(&stagingDesc, nullptr, visibilityStagingBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("visibilityStagingBuffer 作成失敗：Collision::Init内で発生", DebugConsole::ERROR_LOG);
	}

	// visibilityBufferの書き込みを行うためのバッファ作成
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = GameManager::Max3DobjectCount;
	if (visibilityBuffer != nullptr)
	{
		hr = Renderer::GetDevice()->CreateUnorderedAccessView(visibilityBuffer.Get(), &uavDesc, visibilityUAV.GetAddressOf());
		if (FAILED(hr))
		{
			DebugConsole::DebugLog("visibilityUAV 作成失敗：Collision::Init内で発生", DebugConsole::ERROR_LOG);
		}
	}
}

// AABBを計算する
Collision::AABB Collision::CalculateAABB(BoundingBox _bb)
{
	AABB aabb;
	// 最小値は可能な限り大きな値に、最大値は可能な限り小さな値に設定する
	aabb.min = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	aabb.max = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// バウンディングボックスの8つの頂点を計算
	XMFLOAT3 vertex[8] = {
	vertex[0] = { _bb.center.x - _bb.extents.x,_bb.center.y + _bb.extents.y,_bb.center.z + _bb.extents.z },
	vertex[1] = { _bb.center.x + _bb.extents.x,_bb.center.y + _bb.extents.y,_bb.center.z + _bb.extents.z },
	vertex[2] = { _bb.center.x + _bb.extents.x,_bb.center.y + _bb.extents.y,_bb.center.z - _bb.extents.z },
	vertex[3] = { _bb.center.x - _bb.extents.x,_bb.center.y + _bb.extents.y,_bb.center.z - _bb.extents.z },
	vertex[4] = { _bb.center.x - _bb.extents.x,_bb.center.y - _bb.extents.y,_bb.center.z + _bb.extents.z },
	vertex[5] = { _bb.center.x + _bb.extents.x,_bb.center.y - _bb.extents.y,_bb.center.z + _bb.extents.z },
	vertex[6] = { _bb.center.x + _bb.extents.x,_bb.center.y - _bb.extents.y,_bb.center.z - _bb.extents.z },
	vertex[7] = { _bb.center.x - _bb.extents.x,_bb.center.y - _bb.extents.y,_bb.center.z - _bb.extents.z }
	};

	// 各頂点を走査し、AABBの最小・最大座標を決定
	for (int i = 0; i < 8; i++)
	{
		// 最小値の更新
		if (aabb.min.x > vertex[i].x) aabb.min.x = vertex[i].x;
		if (aabb.min.y > vertex[i].y) aabb.min.y = vertex[i].y;
		if (aabb.min.z > vertex[i].z) aabb.min.z = vertex[i].z;

		// 最大値の更新
		if (aabb.max.x < vertex[i].x) aabb.max.x = vertex[i].x;
		if (aabb.max.y < vertex[i].y) aabb.max.y = vertex[i].y;
		if (aabb.max.z < vertex[i].z) aabb.max.z = vertex[i].z;
	}

	return aabb;
}

// 球と球の当たり判定
bool Collision::IsHitSphereAndSphere(BoundingSphere& _bs1, BoundingSphere& _bs2)
{
	// 2つの球の中心間の距離を各軸ごとに計算
	float x = _bs2.center.x - _bs1.center.x; // x方向長さ
	float y = _bs2.center.y - _bs1.center.y; // y方向長さ
	float z = _bs2.center.z - _bs1.center.z; // z方向長さ

	// 2つの球の半径の合計を計算
	float r2 = _bs1.radius + _bs2.radius;    // 円1の半径＋円2の半径

	// 2つの球の中心間の距離の2乗が、半径の合計の2乗以下であれば衝突している
	if (x * x + y * y + z * z <= r2 * r2)
	{
		return true;
	}

	return false;
}

bool Collision::IsHitSphereAndBox(BoundingSphere& _bs, AABB& _aabb, XMFLOAT3& adjustPos, DirectX::XMFLOAT3& _CollisionNormal)
{
	// AABBとの接触点を計算（球の中心をクランプ）
	float contactX = std::clamp(_bs.center.x, _aabb.min.x, _aabb.max.x);
	float contactY = std::clamp(_bs.center.y, _aabb.min.y, _aabb.max.y);
	float contactZ = std::clamp(_bs.center.z, _aabb.min.z, _aabb.max.z);


	// 接触点から球の中心へのベクトル（未正規化法線）
	XMFLOAT3 collisionNormal =
	{
		_bs.center.x - contactX,
		_bs.center.y - contactY,
		_bs.center.z - contactZ
	};

	// 距離の二乗を計算
	float distanceSquared = collisionNormal.x * collisionNormal.x +
		collisionNormal.y * collisionNormal.y +
		collisionNormal.z * collisionNormal.z;

	// 衝突していない場合
	if (distanceSquared > _bs.radius * _bs.radius) {
		return false; // 衝突していない
	}

	// 法線を正規化
	float length = sqrt(distanceSquared);
	if (length > 0.0f)
	{
		collisionNormal.x /= length;
		collisionNormal.y /= length;
		collisionNormal.z /= length;
	}

	_CollisionNormal = collisionNormal;

	// めり込み深さを計算
	float penetrationDepth = _bs.radius - length;

	// 球を押し戻す位置を計算
	adjustPos.x = _bs.center.x + collisionNormal.x * penetrationDepth;
	adjustPos.y = _bs.center.y + collisionNormal.y * penetrationDepth;
	adjustPos.z = _bs.center.z + collisionNormal.z * penetrationDepth;

	_bs.center = adjustPos;

	return true;
}

// 点と直方体の当たり判定
bool Collision::IsHitPointAndBox(DirectX::XMFLOAT3& _point, AABB& _aabb)
{
	// 点が直方体の中にあるか判定
	return (_point.x >= _aabb.min.x && _point.x <= _aabb.max.x &&
		    _point.y >= _aabb.min.y && _point.y <= _aabb.max.y &&
		    _point.z >= _aabb.min.z && _point.z <= _aabb.max.z);
}

// 点と球の当たり判定
bool Collision::IsHitPointAndSphere(DirectX::XMFLOAT3& _point, BoundingSphere& _bs)
{
	// 距離計算
	Vec3 v = Vec3::SetVector(_point) - Vec3::SetVector(_bs.center);
	float distance = v.Length();

	// 球の半径より小さい場合、当たったいると判定
	return distance < _bs.radius;
}

bool Collision::IsHitLineAndBox(Segment& _segment, AABB& _aabb, DirectX::XMFLOAT3& _hitPoint)
{
	// 線分の始点と終点をXMVECTORに変換
	XMVECTOR startP = XMLoadFloat3(&_segment.start);  // 線分の開始点
	XMVECTOR endP = XMLoadFloat3(&_segment.end);      // 線分の終点
	XMVECTOR dirV = XMVectorSubtract(endP, startP);   // 線分の方向ベクトル

	XMFLOAT3 dir;
	XMStoreFloat3(&dir, dirV); // 方向ベクトルをXMFLOAT3に変換

	// 無限大を避けるために非常に小さい値でゼロ方向成分を置き換える
	const float epsilon = 1e-6f;
	XMFLOAT3 invDir;
	invDir.x = (fabsf(dir.x) > epsilon) ? 1.0f / dir.x : FLT_MAX; // x成分の逆数
	invDir.y = (fabsf(dir.y) > epsilon) ? 1.0f / dir.y : FLT_MAX; // y成分の逆数
	invDir.z = (fabsf(dir.z) > epsilon) ? 1.0f / dir.z : FLT_MAX; // z成分の逆数

	// 各成分の交点を計算し、最小・最大交点を求める
	float t1 = (_aabb.min.x - _segment.start.x) * invDir.x;
	float t2 = (_aabb.max.x - _segment.start.x) * invDir.x;
	float t3 = (_aabb.min.y - _segment.start.y) * invDir.y;
	float t4 = (_aabb.max.y - _segment.start.y) * invDir.y;
	float t5 = (_aabb.min.z - _segment.start.z) * invDir.z;
	float t6 = (_aabb.max.z - _segment.start.z) * invDir.z;

	// 各軸の最小・最大交点を求める
	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// 交差判定
	if (tmax < 0 || tmin > tmax || tmin > 1.0f)
	{
		return false;
	}

	// 線分がAABB内にある場合に対応
	tmin = std::max(0.0f, tmin);

	// 最も近い接点を計算
	XMVECTOR contactVec = XMVectorAdd(startP, XMVectorScale(dirV, tmin));

	// 衝突点を格納
	XMStoreFloat3(&_hitPoint, contactVec);

	return true;
}


// 線と平面の当たり判定
bool Collision::IsHitSegmentAndPlane(const Segment& seg, const Triangle& plane, DirectX::XMFLOAT3& intersectionPoint)
{
	const float EPSILON = 1e-6f; // 許容誤差

	// 法線が正規化されている前提
	XMFLOAT3 normal = plane.normal;

	// 線分の方向ベクトルを計算
	XMFLOAT3 segmentDir = 
	{
		seg.end.x - seg.start.x,
		seg.end.y - seg.start.y,
		seg.end.z - seg.start.z
	};

	// 法線と方向ベクトルの内積を計算
	float dotNormalDir = segmentDir.x * normal.x +
		segmentDir.y * normal.y +
		segmentDir.z * normal.z;

	// 平行チェック
	if (fabs(dotNormalDir) < EPSILON) 
	{
		return false; // 平行なので交差なし
	}

	// 線分始点から平面までの距離を計算
	float dotNormalStart = normal.x * seg.start.x +
		normal.y * seg.start.y +
		normal.z * seg.start.z + plane.d;

	float t = -dotNormalStart / dotNormalDir;

	// t が [0, 1] の範囲外なら交差しない
	if (t < 0.0f || t > 1.0f) 
	{
		return false; // 線分範囲外なので交差なし
	}

	// 交点を計算
	intersectionPoint = 
	{
		seg.start.x + t * segmentDir.x,
		seg.start.y + t * segmentDir.y,
		seg.start.z + t * segmentDir.z
	};

	// 三角形内判定 (半平面法)
	XMFLOAT3 v0 = plane.vertices[0];
	XMFLOAT3 v1 = plane.vertices[1];
	XMFLOAT3 v2 = plane.vertices[2];

	auto Cross = [](const XMFLOAT3& a, const XMFLOAT3& b) -> XMFLOAT3 
	{
		return {
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	};

	auto Dot = [](const XMFLOAT3& a, const XMFLOAT3& b) -> float
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	};

	auto Subtract = [](const XMFLOAT3& a, const XMFLOAT3& b) -> XMFLOAT3 
	{
		return { a.x - b.x, a.y - b.y, a.z - b.z };
	};

	// 各辺のベクトルと交点とのベクトルを計算
	XMFLOAT3 edge0 = Subtract(v1, v0);
	XMFLOAT3 edge1 = Subtract(v2, v1);
	XMFLOAT3 edge2 = Subtract(v0, v2);

	// 交点から各頂点へのベクトルを計算
	XMFLOAT3 c0 = Subtract(intersectionPoint, v0);
	XMFLOAT3 c1 = Subtract(intersectionPoint, v1);
	XMFLOAT3 c2 = Subtract(intersectionPoint, v2);

	// 三角形の辺に対する外積
	XMFLOAT3 cross0 = Cross(edge0, c0);
	XMFLOAT3 cross1 = Cross(edge1, c1);
	XMFLOAT3 cross2 = Cross(edge2, c2);

	// 交点が三角形内にあるかチェック (法線と同じ向きであること)
	float dot0 = Dot(cross0, normal);
	float dot1 = Dot(cross1, normal);
	float dot2 = Dot(cross2, normal);

	if (dot0 >= -EPSILON && dot1 >= -EPSILON && dot2 >= -EPSILON) 
	{
		return true;
	}

	return false;
}

// 球と平面の当たり判定
bool Collision::CheckSpherePlaneCollision(const Collision::Triangle& plane, const Collision::BoundingSphere& ps, DirectX::XMFLOAT3& _adjustPos, DirectX::XMFLOAT3& _CollisionNormal)
{
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&plane.vertices[0]);
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&plane.vertices[1]);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&plane.vertices[2]);

	// 三角形の法線ベクトルを計算
	DirectX::XMVECTOR edge1 = v1 - v0;
	DirectX::XMVECTOR edge2 = v2 - v0;
	DirectX::XMVECTOR normal = DirectX::XMVector3Cross(edge1, edge2);
	normal = DirectX::XMVector3Normalize(normal);

	// 球の中心と三角形の平面との距離を計算
	DirectX::XMVECTOR sphereCenter = DirectX::XMLoadFloat3(&ps.center);
	DirectX::XMVECTOR pointOnPlane = v0;  // 三角形の頂点v0を平面上の任意の点として使用

	// 平面のd値を計算
	float d = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal, pointOnPlane));

	// 球の中心から平面までの距離
	float distance = std::fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(sphereCenter, normal)) + d);

	// 距離が球の半径以内であれば衝突している可能性あり
	if (distance <= ps.radius)
	{
		// 球の中心が三角形の内部にあるかを判定
		if (IsPointInTriangle(sphereCenter, v0, v1, v2))
		{
			// 球の中心を押し戻すベクトル
			DirectX::XMVECTOR pushBackVector = normal * (ps.radius - std::fabs(distance));

			// 球の中心を更新（押し戻す）
			sphereCenter += pushBackVector;

			DirectX::XMStoreFloat3(&_adjustPos, sphereCenter);
			DirectX::XMStoreFloat3(&_CollisionNormal, normal);


			return true;
		}

		// 球が三角形の辺に接触しているか確認
		if (CheckSphereEdgeCollision(v0, v1, ps) || CheckSphereEdgeCollision(v1, v2, ps) || CheckSphereEdgeCollision(v2, v0, ps))
		{
			// 球の中心を押し戻すベクトル
			DirectX::XMVECTOR pushBackVector = normal * (ps.radius - std::fabs(distance));

			// 球の中心を更新（押し戻す）
			sphereCenter += pushBackVector;

			DirectX::XMStoreFloat3(&_adjustPos, sphereCenter);
			DirectX::XMStoreFloat3(&_CollisionNormal, normal);
			return true; // 衝突している
		}

		// 球が三角形の頂点に接触しているか確認
		if (CheckSphereVertexCollision(v0, ps) || CheckSphereVertexCollision(v1, ps) || CheckSphereVertexCollision(v2, ps))
		{
			// 球の中心を押し戻すベクトル
			DirectX::XMVECTOR pushBackVector = normal * (ps.radius - std::fabs(distance));

			// 球の中心を更新（押し戻す）
			sphereCenter += pushBackVector;

			DirectX::XMStoreFloat3(&_adjustPos, sphereCenter);
			DirectX::XMStoreFloat3(&_CollisionNormal, normal);
			return true;
		}
	}

	return false;
}

// 球と稜線の当たり判定
bool Collision::CheckSphereEdgeCollision(const DirectX::XMVECTOR& v0, const DirectX::XMVECTOR& v1, const Collision::BoundingSphere& ps)
{
	// 辺の最短距離を計算
	DirectX::XMVECTOR edge = v1 - v0;

	// 球の中心座標をXMVECTORにロード
	DirectX::XMVECTOR sphereCenter = DirectX::XMLoadFloat3(&ps.center);

	// 球の中心からv0へのベクトルを計算
	DirectX::XMVECTOR sphereToV0 = sphereCenter - v0;

	// 辺ベクトルと球の中心からv0へのベクトルの内積を求め、辺の長さの2乗で割る
	float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(sphereToV0, edge)) / DirectX::XMVectorGetX(DirectX::XMVector3Dot(edge, edge));

	// tが0～1の範囲内であれば、最短距離は辺上に存在する
	if (t >= 0 && t <= 1)
	{
		// 交点となる点を計算
		DirectX::XMVECTOR closestPoint = v0 + t * edge;

		// 球の中心からその交点までのベクトルを求める
		DirectX::XMVECTOR sphereToClosestPoint = sphereCenter - closestPoint;

		// 交点までの距離の2乗を計算し、球の半径の2乗と比較
		float distanceSquared = DirectX::XMVectorGetX(DirectX::XMVector3Dot(sphereToClosestPoint, sphereToClosestPoint));

		// 最短距離が球の半径以下であれば衝突している
		return distanceSquared <= ps.radius * ps.radius;
	}

	// tが0～1の範囲外の場合、辺の端点との距離をチェック
	return CheckSphereVertexCollision(v0, ps) || CheckSphereVertexCollision(v1, ps);
}

// 球と頂点の当たり判定
bool Collision::CheckSphereVertexCollision(const DirectX::XMVECTOR& vertex, const Collision::BoundingSphere& ps)
{
	// 球の中心座標をXMLoadFloat3でXMVECTORに変換
	DirectX::XMVECTOR sphereCenter = DirectX::XMLoadFloat3(&ps.center);

	// 球の中心と頂点間のベクトルを計算
	DirectX::XMVECTOR sphereToVertex = sphereCenter - vertex;

	// 距離の2乗を内積で求め、距離を求めた後、平方根を取ることなく比較するために距離の2乗を使う
	float distanceSquared = DirectX::XMVectorGetX(DirectX::XMVector3Dot(sphereToVertex, sphereToVertex));

	// 計算された距離の2乗と球の半径の2乗を比較し、衝突しているかを判定
	return distanceSquared <= ps.radius * ps.radius;
}

// 点が三角形の内部にあるか判定する
bool Collision::IsPointInTriangle(DirectX::XMVECTOR point, DirectX::XMVECTOR v0, DirectX::XMVECTOR v1, DirectX::XMVECTOR v2)
{
	// 三角形の頂点 v0, v1, v2 を使って、辺ベクトル v0v1 と v0v2 を計算
	DirectX::XMVECTOR v0v1 = v1 - v0;
	DirectX::XMVECTOR v0v2 = v2 - v0;
	DirectX::XMVECTOR v0p = point - v0;

	// 点と三角形の辺との関係を計算するために、各辺の内積を計算
	float dot00 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v1, v0v1));
	float dot01 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v1, v0v2));
	float dot02 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v1, v0p));
	float dot11 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v2, v0v2));
	float dot12 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v2, v0p));

	// 三角形内判定のため、逆行列を用いて重心座標 (u, v) を計算
	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// u, v が 0 以上であり、かつ u + v が 1 以下であれば、点は三角形の内部にある
	return (u >= 0) && (v >= 0) && (u + v <= 1);
}

// フラスタムカリング専用当たり判定
void Collision::FrustumCheck(const DirectX::XMFLOAT4* planes, std::vector<ActiveBoundsData>& activeBounds)
{
	ComPtr<ID3D11DeviceContext> context = Renderer::GetContext();

	// フラスタム平面の情報を格納
	FrustumPlane fp;

	memcpy(fp.planes, planes, sizeof(XMFLOAT4) * 6);
	
	// バッファ更新
	Renderer::UpdateBuffer(frustumPlaneBuffer.Get(), &fp, sizeof(FrustumPlane));

	// アクティブなオブジェクトのバウンディングスフィア情報を取り出す
	std::vector<Collision::BoundingSphere> boundingSpheres;
	boundingSpheres.reserve(activeBounds.size());

	std::transform(activeBounds.begin(), activeBounds.end(), std::back_inserter(boundingSpheres),
		[](const ActiveBoundsData& data) { return data.boundingSphere; });

	// 更新する領域を設定し、データをGPUに転送
	D3D11_BOX updateBox;
	updateBox.left = 0;
	updateBox.top = 0;
	updateBox.front = 0;
	updateBox.right = static_cast<UINT>(sizeof(BoundingSphere) * boundingSpheres.size());
	updateBox.bottom = 1;
	updateBox.back = 1;
	context->UpdateSubresource(sphereBoundsBuffer.Get(), 0, &updateBox, boundingSpheres.data(), 0, 0);

	// シェーダーリソースや定数バッファをセットし、コンピュートシェーダーを実行
	context->CSSetShaderResources(0, 1, sphereBoundsSRV.GetAddressOf());
	context->CSSetConstantBuffers(0, 1, frustumPlaneBuffer.GetAddressOf());
	context->CSSetShader(Shader::GetCS(Shader::CS_FRUSTUM), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, visibilityUAV.GetAddressOf(), nullptr);
	
	// 各スレッドグループの数を計算し、フラスタムチェックを実行
	const int threadsPerGroup = MaxThreadDataNum;
	const size_t groupCount = (activeBounds.size() + threadsPerGroup - 1) / threadsPerGroup;
	context->Dispatch(static_cast<UINT>(groupCount), 1, 1);

	// 可視性データを転送するためのコピー
	context->CopyResource(visibilityStagingBuffer.Get(), visibilityBuffer.Get());

	// 可視性データをCPUにマッピング
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(visibilityStagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		auto* rawData = static_cast<uint32_t*>(mappedResource.pData);
		
		// 各アクティブ状態を更新
		for (int i = 0; i < activeBounds.size(); i++)
		{
			activeBounds[i].activeState.get() = rawData[i];
		}
		context->Unmap(visibilityStagingBuffer.Get(), 0);
	}

	// UAV を解除
	ComPtr<ID3D11UnorderedAccessView> nullUAV = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, nullUAV.GetAddressOf(), nullptr);
}

// ブロードフェーズ専用球と球の当たり判定をする
void Collision::BroadPhaseCheck(const BoundingSphere& playerBounds, std::vector<ActiveBoundsData>& activeBounds)
{
	ComPtr<ID3D11DeviceContext> context = Renderer::GetContext();

	// バッファ更新
	Renderer::UpdateBuffer(playerSphereBoundsBuffer.Get(), &playerBounds, sizeof(BoundingSphere));

	// アクティブなオブジェクトのバウンディングスフィア情報を取り出す
	std::vector<Collision::BoundingSphere> boundingSpheres;
	boundingSpheres.reserve(activeBounds.size());

	std::transform(activeBounds.begin(), activeBounds.end(), std::back_inserter(boundingSpheres),
		[](const ActiveBoundsData& data) { return data.boundingSphere; });

	// 更新する領域を設定し、データをGPUに転送
	D3D11_BOX updateBox;
	updateBox.left = 0;
	updateBox.top = 0;
	updateBox.front = 0;
	updateBox.right = static_cast<UINT>(sizeof(BoundingSphere) * boundingSpheres.size());
	updateBox.bottom = 1;
	updateBox.back = 1;
	context->UpdateSubresource(sphereBoundsBuffer.Get(), 0, &updateBox, boundingSpheres.data(), 0, 0);

	// シェーダーリソースや定数バッファをセットし、コンピュートシェーダーを実行
	context->CSSetShaderResources(0, 1, sphereBoundsSRV.GetAddressOf());
	context->CSSetConstantBuffers(0, 1, playerSphereBoundsBuffer.GetAddressOf());
	context->CSSetShader(Shader::GetCS(Shader::CS_BROAD_PHASE), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, visibilityUAV.GetAddressOf(), nullptr);

	// 各スレッドグループの数を計算し、フラスタムチェックを実行
	const int threadsPerGroup = MaxThreadDataNum;
	const size_t groupCount = (activeBounds.size() + threadsPerGroup - 1) / threadsPerGroup;
	context->Dispatch(static_cast<UINT>(groupCount), 1, 1);

	// 可視性データを転送するためのコピー
	context->CopyResource(visibilityStagingBuffer.Get(), visibilityBuffer.Get());

	// 可視性データをCPUにマッピング
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(visibilityStagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		auto* rawData = static_cast<uint32_t*>(mappedResource.pData);

		for (int i = 0; i < activeBounds.size(); i++)
		{
			// 各アクティブ状態を更新
			activeBounds[i].activeState.get() = rawData[i];
		}
		context->Unmap(visibilityStagingBuffer.Get(), 0);
	}

	// UAV を解除
	ComPtr<ID3D11UnorderedAccessView> nullUAV = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, nullUAV.GetAddressOf(), nullptr);
}

// 当たり判定後のめり込んだ分の座標調整関数（球体同士）
DirectX::XMFLOAT3 Collision::AdjustPositionSphereAndSphere(BoundingSphere& _adjustBs, BoundingSphere& _bs)
{
	XMFLOAT3 result;

	// ベクトルの設定
	Vec3 adjustVec = Vec3::SetVector(_adjustBs.center);
	Vec3 bsVec = Vec3::SetVector(_bs.center);

	// 位置ベクトルの差を計算
	Vec3 newVec = adjustVec - bsVec;

	// 2つの球の中心間の距離を計算
	float length = newVec.Length();

	// 2つの球の半径の合計を計算
	float radiusSum = _adjustBs.radius + _bs.radius;

	// 球が重なっている場合のみ調整
	if (length < radiusSum)
	{
		// 正規化された方向ベクトルを計算
		newVec = newVec.Normalize();

		// 重なり量を計算し、球を押し戻す
		float overlap = radiusSum - length;
		adjustVec += newVec * overlap;
		// Y方向無視
		adjustVec.y = _adjustBs.center.y;

		// 新しい位置を結果に設定
		result = Vec3::SetXMFLOAT3(adjustVec);
	}
	else
	{
		// 重なっていない場合は現在の位置をそのまま返す
		result = _adjustBs.center;
	}

	return result;
}