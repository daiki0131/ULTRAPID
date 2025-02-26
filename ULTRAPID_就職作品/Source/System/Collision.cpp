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

// ����������
void Collision::Init()
{
	HRESULT hr;

	// �Փ˔���Ώۗp�o�b�t�@���쐬
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Collision::BoundingSphere) * GameManager::Max3DobjectCount;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = sizeof(Collision::BoundingSphere);
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, sphereBoundsBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("sphereBoundsBuffer �쐬���s�FCollision::Init���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// �Փˌ��ʂ�ۑ�����o�b�t�@���쐬
	bufferDesc.ByteWidth = sizeof(uint32_t) * GameManager::Max3DobjectCount;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	hr = Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, visibilityBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("visibilityBuffer �쐬���s�FCollision::Init���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// sphereBoundsBuffer��GPU���ł̓ǂݎ���p�̃o�b�t�@�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = GameManager::Max3DobjectCount;

	hr = Renderer::GetDevice()->CreateShaderResourceView(sphereBoundsBuffer.Get(), &srvDesc, sphereBoundsSRV.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("sphereBoundsBuffer �쐬���s�FCollision::Init���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// �J�����̃t���X�^���o�b�t�@�쐬
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
		DebugConsole::DebugLog("frustumPlaneBuffer �쐬���s�FCollision::Init���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// �v���C���[�̃X�t�B�A�o�b�t�@�쐬
	cbDesc.ByteWidth = sizeof(Collision::BoundingSphere);
	hr = Renderer::GetDevice()->CreateBuffer(&cbDesc, nullptr, playerSphereBoundsBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("playerSphereBoundsBuffer �쐬���s�FCollision::Init���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// visibilityBuffer��CPU���ł̓ǂݎ���p�o�b�t�@�쐬
	D3D11_BUFFER_DESC stagingDesc = {};
	stagingDesc.ByteWidth = sizeof(uint32_t) * GameManager::Max3DobjectCount;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = Renderer::GetDevice()->CreateBuffer(&stagingDesc, nullptr, visibilityStagingBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("visibilityStagingBuffer �쐬���s�FCollision::Init���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// visibilityBuffer�̏������݂��s�����߂̃o�b�t�@�쐬
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = GameManager::Max3DobjectCount;
	if (visibilityBuffer != nullptr)
	{
		hr = Renderer::GetDevice()->CreateUnorderedAccessView(visibilityBuffer.Get(), &uavDesc, visibilityUAV.GetAddressOf());
		if (FAILED(hr))
		{
			DebugConsole::DebugLog("visibilityUAV �쐬���s�FCollision::Init���Ŕ���", DebugConsole::ERROR_LOG);
		}
	}
}

// AABB���v�Z����
Collision::AABB Collision::CalculateAABB(BoundingBox _bb)
{
	AABB aabb;
	// �ŏ��l�͉\�Ȍ���傫�Ȓl�ɁA�ő�l�͉\�Ȍ��菬���Ȓl�ɐݒ肷��
	aabb.min = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	aabb.max = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// �o�E���f�B���O�{�b�N�X��8�̒��_���v�Z
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

	// �e���_�𑖍����AAABB�̍ŏ��E�ő���W������
	for (int i = 0; i < 8; i++)
	{
		// �ŏ��l�̍X�V
		if (aabb.min.x > vertex[i].x) aabb.min.x = vertex[i].x;
		if (aabb.min.y > vertex[i].y) aabb.min.y = vertex[i].y;
		if (aabb.min.z > vertex[i].z) aabb.min.z = vertex[i].z;

		// �ő�l�̍X�V
		if (aabb.max.x < vertex[i].x) aabb.max.x = vertex[i].x;
		if (aabb.max.y < vertex[i].y) aabb.max.y = vertex[i].y;
		if (aabb.max.z < vertex[i].z) aabb.max.z = vertex[i].z;
	}

	return aabb;
}

// ���Ƌ��̓����蔻��
bool Collision::IsHitSphereAndSphere(BoundingSphere& _bs1, BoundingSphere& _bs2)
{
	// 2�̋��̒��S�Ԃ̋������e�����ƂɌv�Z
	float x = _bs2.center.x - _bs1.center.x; // x��������
	float y = _bs2.center.y - _bs1.center.y; // y��������
	float z = _bs2.center.z - _bs1.center.z; // z��������

	// 2�̋��̔��a�̍��v���v�Z
	float r2 = _bs1.radius + _bs2.radius;    // �~1�̔��a�{�~2�̔��a

	// 2�̋��̒��S�Ԃ̋�����2�悪�A���a�̍��v��2��ȉ��ł���ΏՓ˂��Ă���
	if (x * x + y * y + z * z <= r2 * r2)
	{
		return true;
	}

	return false;
}

bool Collision::IsHitSphereAndBox(BoundingSphere& _bs, AABB& _aabb, XMFLOAT3& adjustPos, DirectX::XMFLOAT3& _CollisionNormal)
{
	// AABB�Ƃ̐ڐG�_���v�Z�i���̒��S���N�����v�j
	float contactX = std::clamp(_bs.center.x, _aabb.min.x, _aabb.max.x);
	float contactY = std::clamp(_bs.center.y, _aabb.min.y, _aabb.max.y);
	float contactZ = std::clamp(_bs.center.z, _aabb.min.z, _aabb.max.z);


	// �ڐG�_���狅�̒��S�ւ̃x�N�g���i�����K���@���j
	XMFLOAT3 collisionNormal =
	{
		_bs.center.x - contactX,
		_bs.center.y - contactY,
		_bs.center.z - contactZ
	};

	// �����̓����v�Z
	float distanceSquared = collisionNormal.x * collisionNormal.x +
		collisionNormal.y * collisionNormal.y +
		collisionNormal.z * collisionNormal.z;

	// �Փ˂��Ă��Ȃ��ꍇ
	if (distanceSquared > _bs.radius * _bs.radius) {
		return false; // �Փ˂��Ă��Ȃ�
	}

	// �@���𐳋K��
	float length = sqrt(distanceSquared);
	if (length > 0.0f)
	{
		collisionNormal.x /= length;
		collisionNormal.y /= length;
		collisionNormal.z /= length;
	}

	_CollisionNormal = collisionNormal;

	// �߂荞�ݐ[�����v�Z
	float penetrationDepth = _bs.radius - length;

	// ���������߂��ʒu���v�Z
	adjustPos.x = _bs.center.x + collisionNormal.x * penetrationDepth;
	adjustPos.y = _bs.center.y + collisionNormal.y * penetrationDepth;
	adjustPos.z = _bs.center.z + collisionNormal.z * penetrationDepth;

	_bs.center = adjustPos;

	return true;
}

// �_�ƒ����̂̓����蔻��
bool Collision::IsHitPointAndBox(DirectX::XMFLOAT3& _point, AABB& _aabb)
{
	// �_�������̂̒��ɂ��邩����
	return (_point.x >= _aabb.min.x && _point.x <= _aabb.max.x &&
		    _point.y >= _aabb.min.y && _point.y <= _aabb.max.y &&
		    _point.z >= _aabb.min.z && _point.z <= _aabb.max.z);
}

// �_�Ƌ��̓����蔻��
bool Collision::IsHitPointAndSphere(DirectX::XMFLOAT3& _point, BoundingSphere& _bs)
{
	// �����v�Z
	Vec3 v = Vec3::SetVector(_point) - Vec3::SetVector(_bs.center);
	float distance = v.Length();

	// ���̔��a��菬�����ꍇ�A������������Ɣ���
	return distance < _bs.radius;
}

bool Collision::IsHitLineAndBox(Segment& _segment, AABB& _aabb, DirectX::XMFLOAT3& _hitPoint)
{
	// �����̎n�_�ƏI�_��XMVECTOR�ɕϊ�
	XMVECTOR startP = XMLoadFloat3(&_segment.start);  // �����̊J�n�_
	XMVECTOR endP = XMLoadFloat3(&_segment.end);      // �����̏I�_
	XMVECTOR dirV = XMVectorSubtract(endP, startP);   // �����̕����x�N�g��

	XMFLOAT3 dir;
	XMStoreFloat3(&dir, dirV); // �����x�N�g����XMFLOAT3�ɕϊ�

	// �����������邽�߂ɔ��ɏ������l�Ń[������������u��������
	const float epsilon = 1e-6f;
	XMFLOAT3 invDir;
	invDir.x = (fabsf(dir.x) > epsilon) ? 1.0f / dir.x : FLT_MAX; // x�����̋t��
	invDir.y = (fabsf(dir.y) > epsilon) ? 1.0f / dir.y : FLT_MAX; // y�����̋t��
	invDir.z = (fabsf(dir.z) > epsilon) ? 1.0f / dir.z : FLT_MAX; // z�����̋t��

	// �e�����̌�_���v�Z���A�ŏ��E�ő��_�����߂�
	float t1 = (_aabb.min.x - _segment.start.x) * invDir.x;
	float t2 = (_aabb.max.x - _segment.start.x) * invDir.x;
	float t3 = (_aabb.min.y - _segment.start.y) * invDir.y;
	float t4 = (_aabb.max.y - _segment.start.y) * invDir.y;
	float t5 = (_aabb.min.z - _segment.start.z) * invDir.z;
	float t6 = (_aabb.max.z - _segment.start.z) * invDir.z;

	// �e���̍ŏ��E�ő��_�����߂�
	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// ��������
	if (tmax < 0 || tmin > tmax || tmin > 1.0f)
	{
		return false;
	}

	// ������AABB���ɂ���ꍇ�ɑΉ�
	tmin = std::max(0.0f, tmin);

	// �ł��߂��ړ_���v�Z
	XMVECTOR contactVec = XMVectorAdd(startP, XMVectorScale(dirV, tmin));

	// �Փ˓_���i�[
	XMStoreFloat3(&_hitPoint, contactVec);

	return true;
}


// ���ƕ��ʂ̓����蔻��
bool Collision::IsHitSegmentAndPlane(const Segment& seg, const Triangle& plane, DirectX::XMFLOAT3& intersectionPoint)
{
	const float EPSILON = 1e-6f; // ���e�덷

	// �@�������K������Ă���O��
	XMFLOAT3 normal = plane.normal;

	// �����̕����x�N�g�����v�Z
	XMFLOAT3 segmentDir = 
	{
		seg.end.x - seg.start.x,
		seg.end.y - seg.start.y,
		seg.end.z - seg.start.z
	};

	// �@���ƕ����x�N�g���̓��ς��v�Z
	float dotNormalDir = segmentDir.x * normal.x +
		segmentDir.y * normal.y +
		segmentDir.z * normal.z;

	// ���s�`�F�b�N
	if (fabs(dotNormalDir) < EPSILON) 
	{
		return false; // ���s�Ȃ̂Ō����Ȃ�
	}

	// �����n�_���畽�ʂ܂ł̋������v�Z
	float dotNormalStart = normal.x * seg.start.x +
		normal.y * seg.start.y +
		normal.z * seg.start.z + plane.d;

	float t = -dotNormalStart / dotNormalDir;

	// t �� [0, 1] �͈̔͊O�Ȃ�������Ȃ�
	if (t < 0.0f || t > 1.0f) 
	{
		return false; // �����͈͊O�Ȃ̂Ō����Ȃ�
	}

	// ��_���v�Z
	intersectionPoint = 
	{
		seg.start.x + t * segmentDir.x,
		seg.start.y + t * segmentDir.y,
		seg.start.z + t * segmentDir.z
	};

	// �O�p�`������ (�����ʖ@)
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

	// �e�ӂ̃x�N�g���ƌ�_�Ƃ̃x�N�g�����v�Z
	XMFLOAT3 edge0 = Subtract(v1, v0);
	XMFLOAT3 edge1 = Subtract(v2, v1);
	XMFLOAT3 edge2 = Subtract(v0, v2);

	// ��_����e���_�ւ̃x�N�g�����v�Z
	XMFLOAT3 c0 = Subtract(intersectionPoint, v0);
	XMFLOAT3 c1 = Subtract(intersectionPoint, v1);
	XMFLOAT3 c2 = Subtract(intersectionPoint, v2);

	// �O�p�`�̕ӂɑ΂���O��
	XMFLOAT3 cross0 = Cross(edge0, c0);
	XMFLOAT3 cross1 = Cross(edge1, c1);
	XMFLOAT3 cross2 = Cross(edge2, c2);

	// ��_���O�p�`���ɂ��邩�`�F�b�N (�@���Ɠ��������ł��邱��)
	float dot0 = Dot(cross0, normal);
	float dot1 = Dot(cross1, normal);
	float dot2 = Dot(cross2, normal);

	if (dot0 >= -EPSILON && dot1 >= -EPSILON && dot2 >= -EPSILON) 
	{
		return true;
	}

	return false;
}

// ���ƕ��ʂ̓����蔻��
bool Collision::CheckSpherePlaneCollision(const Collision::Triangle& plane, const Collision::BoundingSphere& ps, DirectX::XMFLOAT3& _adjustPos, DirectX::XMFLOAT3& _CollisionNormal)
{
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&plane.vertices[0]);
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&plane.vertices[1]);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&plane.vertices[2]);

	// �O�p�`�̖@���x�N�g�����v�Z
	DirectX::XMVECTOR edge1 = v1 - v0;
	DirectX::XMVECTOR edge2 = v2 - v0;
	DirectX::XMVECTOR normal = DirectX::XMVector3Cross(edge1, edge2);
	normal = DirectX::XMVector3Normalize(normal);

	// ���̒��S�ƎO�p�`�̕��ʂƂ̋������v�Z
	DirectX::XMVECTOR sphereCenter = DirectX::XMLoadFloat3(&ps.center);
	DirectX::XMVECTOR pointOnPlane = v0;  // �O�p�`�̒��_v0�𕽖ʏ�̔C�ӂ̓_�Ƃ��Ďg�p

	// ���ʂ�d�l���v�Z
	float d = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal, pointOnPlane));

	// ���̒��S���畽�ʂ܂ł̋���
	float distance = std::fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(sphereCenter, normal)) + d);

	// ���������̔��a�ȓ��ł���ΏՓ˂��Ă���\������
	if (distance <= ps.radius)
	{
		// ���̒��S���O�p�`�̓����ɂ��邩�𔻒�
		if (IsPointInTriangle(sphereCenter, v0, v1, v2))
		{
			// ���̒��S�������߂��x�N�g��
			DirectX::XMVECTOR pushBackVector = normal * (ps.radius - std::fabs(distance));

			// ���̒��S���X�V�i�����߂��j
			sphereCenter += pushBackVector;

			DirectX::XMStoreFloat3(&_adjustPos, sphereCenter);
			DirectX::XMStoreFloat3(&_CollisionNormal, normal);


			return true;
		}

		// �����O�p�`�̕ӂɐڐG���Ă��邩�m�F
		if (CheckSphereEdgeCollision(v0, v1, ps) || CheckSphereEdgeCollision(v1, v2, ps) || CheckSphereEdgeCollision(v2, v0, ps))
		{
			// ���̒��S�������߂��x�N�g��
			DirectX::XMVECTOR pushBackVector = normal * (ps.radius - std::fabs(distance));

			// ���̒��S���X�V�i�����߂��j
			sphereCenter += pushBackVector;

			DirectX::XMStoreFloat3(&_adjustPos, sphereCenter);
			DirectX::XMStoreFloat3(&_CollisionNormal, normal);
			return true; // �Փ˂��Ă���
		}

		// �����O�p�`�̒��_�ɐڐG���Ă��邩�m�F
		if (CheckSphereVertexCollision(v0, ps) || CheckSphereVertexCollision(v1, ps) || CheckSphereVertexCollision(v2, ps))
		{
			// ���̒��S�������߂��x�N�g��
			DirectX::XMVECTOR pushBackVector = normal * (ps.radius - std::fabs(distance));

			// ���̒��S���X�V�i�����߂��j
			sphereCenter += pushBackVector;

			DirectX::XMStoreFloat3(&_adjustPos, sphereCenter);
			DirectX::XMStoreFloat3(&_CollisionNormal, normal);
			return true;
		}
	}

	return false;
}

// ���ƗŐ��̓����蔻��
bool Collision::CheckSphereEdgeCollision(const DirectX::XMVECTOR& v0, const DirectX::XMVECTOR& v1, const Collision::BoundingSphere& ps)
{
	// �ӂ̍ŒZ�������v�Z
	DirectX::XMVECTOR edge = v1 - v0;

	// ���̒��S���W��XMVECTOR�Ƀ��[�h
	DirectX::XMVECTOR sphereCenter = DirectX::XMLoadFloat3(&ps.center);

	// ���̒��S����v0�ւ̃x�N�g�����v�Z
	DirectX::XMVECTOR sphereToV0 = sphereCenter - v0;

	// �Ӄx�N�g���Ƌ��̒��S����v0�ւ̃x�N�g���̓��ς����߁A�ӂ̒�����2��Ŋ���
	float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(sphereToV0, edge)) / DirectX::XMVectorGetX(DirectX::XMVector3Dot(edge, edge));

	// t��0�`1�͈͓̔��ł���΁A�ŒZ�����͕ӏ�ɑ��݂���
	if (t >= 0 && t <= 1)
	{
		// ��_�ƂȂ�_���v�Z
		DirectX::XMVECTOR closestPoint = v0 + t * edge;

		// ���̒��S���炻�̌�_�܂ł̃x�N�g�������߂�
		DirectX::XMVECTOR sphereToClosestPoint = sphereCenter - closestPoint;

		// ��_�܂ł̋�����2����v�Z���A���̔��a��2��Ɣ�r
		float distanceSquared = DirectX::XMVectorGetX(DirectX::XMVector3Dot(sphereToClosestPoint, sphereToClosestPoint));

		// �ŒZ���������̔��a�ȉ��ł���ΏՓ˂��Ă���
		return distanceSquared <= ps.radius * ps.radius;
	}

	// t��0�`1�͈̔͊O�̏ꍇ�A�ӂ̒[�_�Ƃ̋������`�F�b�N
	return CheckSphereVertexCollision(v0, ps) || CheckSphereVertexCollision(v1, ps);
}

// ���ƒ��_�̓����蔻��
bool Collision::CheckSphereVertexCollision(const DirectX::XMVECTOR& vertex, const Collision::BoundingSphere& ps)
{
	// ���̒��S���W��XMLoadFloat3��XMVECTOR�ɕϊ�
	DirectX::XMVECTOR sphereCenter = DirectX::XMLoadFloat3(&ps.center);

	// ���̒��S�ƒ��_�Ԃ̃x�N�g�����v�Z
	DirectX::XMVECTOR sphereToVertex = sphereCenter - vertex;

	// ������2�����ςŋ��߁A���������߂���A����������邱�ƂȂ���r���邽�߂ɋ�����2����g��
	float distanceSquared = DirectX::XMVectorGetX(DirectX::XMVector3Dot(sphereToVertex, sphereToVertex));

	// �v�Z���ꂽ������2��Ƌ��̔��a��2����r���A�Փ˂��Ă��邩�𔻒�
	return distanceSquared <= ps.radius * ps.radius;
}

// �_���O�p�`�̓����ɂ��邩���肷��
bool Collision::IsPointInTriangle(DirectX::XMVECTOR point, DirectX::XMVECTOR v0, DirectX::XMVECTOR v1, DirectX::XMVECTOR v2)
{
	// �O�p�`�̒��_ v0, v1, v2 ���g���āA�Ӄx�N�g�� v0v1 �� v0v2 ���v�Z
	DirectX::XMVECTOR v0v1 = v1 - v0;
	DirectX::XMVECTOR v0v2 = v2 - v0;
	DirectX::XMVECTOR v0p = point - v0;

	// �_�ƎO�p�`�̕ӂƂ̊֌W���v�Z���邽�߂ɁA�e�ӂ̓��ς��v�Z
	float dot00 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v1, v0v1));
	float dot01 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v1, v0v2));
	float dot02 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v1, v0p));
	float dot11 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v2, v0v2));
	float dot12 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0v2, v0p));

	// �O�p�`������̂��߁A�t�s���p���ďd�S���W (u, v) ���v�Z
	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// u, v �� 0 �ȏ�ł���A���� u + v �� 1 �ȉ��ł���΁A�_�͎O�p�`�̓����ɂ���
	return (u >= 0) && (v >= 0) && (u + v <= 1);
}

// �t���X�^���J�����O��p�����蔻��
void Collision::FrustumCheck(const DirectX::XMFLOAT4* planes, std::vector<ActiveBoundsData>& activeBounds)
{
	ComPtr<ID3D11DeviceContext> context = Renderer::GetContext();

	// �t���X�^�����ʂ̏����i�[
	FrustumPlane fp;

	memcpy(fp.planes, planes, sizeof(XMFLOAT4) * 6);
	
	// �o�b�t�@�X�V
	Renderer::UpdateBuffer(frustumPlaneBuffer.Get(), &fp, sizeof(FrustumPlane));

	// �A�N�e�B�u�ȃI�u�W�F�N�g�̃o�E���f�B���O�X�t�B�A�������o��
	std::vector<Collision::BoundingSphere> boundingSpheres;
	boundingSpheres.reserve(activeBounds.size());

	std::transform(activeBounds.begin(), activeBounds.end(), std::back_inserter(boundingSpheres),
		[](const ActiveBoundsData& data) { return data.boundingSphere; });

	// �X�V����̈��ݒ肵�A�f�[�^��GPU�ɓ]��
	D3D11_BOX updateBox;
	updateBox.left = 0;
	updateBox.top = 0;
	updateBox.front = 0;
	updateBox.right = static_cast<UINT>(sizeof(BoundingSphere) * boundingSpheres.size());
	updateBox.bottom = 1;
	updateBox.back = 1;
	context->UpdateSubresource(sphereBoundsBuffer.Get(), 0, &updateBox, boundingSpheres.data(), 0, 0);

	// �V�F�[�_�[���\�[�X��萔�o�b�t�@���Z�b�g���A�R���s���[�g�V�F�[�_�[�����s
	context->CSSetShaderResources(0, 1, sphereBoundsSRV.GetAddressOf());
	context->CSSetConstantBuffers(0, 1, frustumPlaneBuffer.GetAddressOf());
	context->CSSetShader(Shader::GetCS(Shader::CS_FRUSTUM), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, visibilityUAV.GetAddressOf(), nullptr);
	
	// �e�X���b�h�O���[�v�̐����v�Z���A�t���X�^���`�F�b�N�����s
	const int threadsPerGroup = MaxThreadDataNum;
	const size_t groupCount = (activeBounds.size() + threadsPerGroup - 1) / threadsPerGroup;
	context->Dispatch(static_cast<UINT>(groupCount), 1, 1);

	// �����f�[�^��]�����邽�߂̃R�s�[
	context->CopyResource(visibilityStagingBuffer.Get(), visibilityBuffer.Get());

	// �����f�[�^��CPU�Ƀ}�b�s���O
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(visibilityStagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		auto* rawData = static_cast<uint32_t*>(mappedResource.pData);
		
		// �e�A�N�e�B�u��Ԃ��X�V
		for (int i = 0; i < activeBounds.size(); i++)
		{
			activeBounds[i].activeState.get() = rawData[i];
		}
		context->Unmap(visibilityStagingBuffer.Get(), 0);
	}

	// UAV ������
	ComPtr<ID3D11UnorderedAccessView> nullUAV = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, nullUAV.GetAddressOf(), nullptr);
}

// �u���[�h�t�F�[�Y��p���Ƌ��̓����蔻�������
void Collision::BroadPhaseCheck(const BoundingSphere& playerBounds, std::vector<ActiveBoundsData>& activeBounds)
{
	ComPtr<ID3D11DeviceContext> context = Renderer::GetContext();

	// �o�b�t�@�X�V
	Renderer::UpdateBuffer(playerSphereBoundsBuffer.Get(), &playerBounds, sizeof(BoundingSphere));

	// �A�N�e�B�u�ȃI�u�W�F�N�g�̃o�E���f�B���O�X�t�B�A�������o��
	std::vector<Collision::BoundingSphere> boundingSpheres;
	boundingSpheres.reserve(activeBounds.size());

	std::transform(activeBounds.begin(), activeBounds.end(), std::back_inserter(boundingSpheres),
		[](const ActiveBoundsData& data) { return data.boundingSphere; });

	// �X�V����̈��ݒ肵�A�f�[�^��GPU�ɓ]��
	D3D11_BOX updateBox;
	updateBox.left = 0;
	updateBox.top = 0;
	updateBox.front = 0;
	updateBox.right = static_cast<UINT>(sizeof(BoundingSphere) * boundingSpheres.size());
	updateBox.bottom = 1;
	updateBox.back = 1;
	context->UpdateSubresource(sphereBoundsBuffer.Get(), 0, &updateBox, boundingSpheres.data(), 0, 0);

	// �V�F�[�_�[���\�[�X��萔�o�b�t�@���Z�b�g���A�R���s���[�g�V�F�[�_�[�����s
	context->CSSetShaderResources(0, 1, sphereBoundsSRV.GetAddressOf());
	context->CSSetConstantBuffers(0, 1, playerSphereBoundsBuffer.GetAddressOf());
	context->CSSetShader(Shader::GetCS(Shader::CS_BROAD_PHASE), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, visibilityUAV.GetAddressOf(), nullptr);

	// �e�X���b�h�O���[�v�̐����v�Z���A�t���X�^���`�F�b�N�����s
	const int threadsPerGroup = MaxThreadDataNum;
	const size_t groupCount = (activeBounds.size() + threadsPerGroup - 1) / threadsPerGroup;
	context->Dispatch(static_cast<UINT>(groupCount), 1, 1);

	// �����f�[�^��]�����邽�߂̃R�s�[
	context->CopyResource(visibilityStagingBuffer.Get(), visibilityBuffer.Get());

	// �����f�[�^��CPU�Ƀ}�b�s���O
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(visibilityStagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		auto* rawData = static_cast<uint32_t*>(mappedResource.pData);

		for (int i = 0; i < activeBounds.size(); i++)
		{
			// �e�A�N�e�B�u��Ԃ��X�V
			activeBounds[i].activeState.get() = rawData[i];
		}
		context->Unmap(visibilityStagingBuffer.Get(), 0);
	}

	// UAV ������
	ComPtr<ID3D11UnorderedAccessView> nullUAV = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, nullUAV.GetAddressOf(), nullptr);
}

// �����蔻���̂߂荞�񂾕��̍��W�����֐��i���̓��m�j
DirectX::XMFLOAT3 Collision::AdjustPositionSphereAndSphere(BoundingSphere& _adjustBs, BoundingSphere& _bs)
{
	XMFLOAT3 result;

	// �x�N�g���̐ݒ�
	Vec3 adjustVec = Vec3::SetVector(_adjustBs.center);
	Vec3 bsVec = Vec3::SetVector(_bs.center);

	// �ʒu�x�N�g���̍����v�Z
	Vec3 newVec = adjustVec - bsVec;

	// 2�̋��̒��S�Ԃ̋������v�Z
	float length = newVec.Length();

	// 2�̋��̔��a�̍��v���v�Z
	float radiusSum = _adjustBs.radius + _bs.radius;

	// �����d�Ȃ��Ă���ꍇ�̂ݒ���
	if (length < radiusSum)
	{
		// ���K�����ꂽ�����x�N�g�����v�Z
		newVec = newVec.Normalize();

		// �d�Ȃ�ʂ��v�Z���A���������߂�
		float overlap = radiusSum - length;
		adjustVec += newVec * overlap;
		// Y��������
		adjustVec.y = _adjustBs.center.y;

		// �V�����ʒu�����ʂɐݒ�
		result = Vec3::SetXMFLOAT3(adjustVec);
	}
	else
	{
		// �d�Ȃ��Ă��Ȃ��ꍇ�͌��݂̈ʒu�����̂܂ܕԂ�
		result = _adjustBs.center;
	}

	return result;
}