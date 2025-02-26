#include "SlopeObject.h"
#include "Geometry.h"

using namespace DirectX;

// �R���X�g���N�^
SlopeObject::SlopeObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:GameObject(_pos, _rot, _scale, _color)
{
	// �e���ʂ�ݒ�
	SetPlane();
}

// ���ʂ̐ݒ�
void SlopeObject::SetPlane()
{
	// �ړ��A�X�P�[���A��]�s����ʂɍ쐬
	XMMATRIX matrixMove = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX matrixScale = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
	XMMATRIX matrixRotateY = XMMatrixRotationY(XMConvertToRadians(rotation.y));
	XMMATRIX matrixRotateX = XMMatrixRotationX(XMConvertToRadians(rotation.x));
	XMMATRIX matrixRotateZ = XMMatrixRotationZ(XMConvertToRadians(rotation.z));

	// ��]�s���X, Y, Z�̏��ɍ���
	XMMATRIX matrixRotate = matrixRotateX * matrixRotateY * matrixRotateZ;

	// �ŏI�I�ȕϊ��s��
	XMMATRIX transform = matrixScale * matrixRotate * matrixMove;

	// �W�I���g���f�[�^���擾
	const Geometry::VertexData vd = Geometry::GetSlopeVertexData();

	// ���_�f�[�^�̈ʒu����z��Ɋi�[
	std::vector<XMFLOAT3> verticesPos;

	verticesPos.reserve(vd.vertices.size());

	for (const auto& vertex : vd.vertices)
	{
		verticesPos.push_back(vertex.position);
	}

	// ���b�V���̖ʏ����擾
	planes = GetPlanesFromMesh(verticesPos, vd.indices, transform);

	// �o�E���f�B���O�X�t�B�A�̐ݒ�
	boundingSphere.center = position;

	// �ő�X�P�[�����O�l�����߂Ĕ��a�ɓK�p
	float maxLength = sqrt(scaling.x * scaling.x + scaling.y * scaling.y + scaling.z * scaling.z);

	boundingSphere.radius = maxLength * 0.505f;
}

// �@���ƕ��ʕ������̌W�����v�Z
Collision::Triangle SlopeObject::CalculatePlane(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const DirectX::XMFLOAT3& v3)
{
	// 2�̃G�b�W�x�N�g�����v�Z
	XMVECTOR edge1 = XMVectorSet(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z, 0.0f);
	XMVECTOR edge2 = XMVectorSet(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z, 0.0f);

	// �O�ςŖ@�����v�Z
	XMVECTOR normalVec = XMVector3Cross(edge1, edge2);
	normalVec = XMVector3Normalize(normalVec);  // ���K��

	// �@����ۑ�
	XMFLOAT3 normal;
	XMStoreFloat3(&normal, normalVec);

	// ���ʂ�d�l���v�Z
	float d = -(normal.x * v1.x + normal.y * v1.y + normal.z * v1.z);

	// ���ʂ�Ԃ�
	return { normal, d, { v1, v2, v3 } }; // �ǉ��������_���܂�
}

// �O�p�`�̕��ʏ��𒊏o
std::vector<Collision::Triangle> SlopeObject::GetPlanesFromMesh(const std::vector<DirectX::XMFLOAT3>& vertices, const std::vector<UINT>& indices, const DirectX::XMMATRIX& transform)
{
	std::vector<Collision::Triangle> planes;

	// ���_��ϊ�����
	std::vector<XMFLOAT3> transformedVertices = TransformVertices(vertices, transform);

	// �C���f�b�N�X��3���Ƃɏ������ĎO�p�`���Ƃɕ��ʂ��v�Z
	for (size_t i = 0; i < indices.size(); i += 3) {
		UINT idx1 = indices[i];
		UINT idx2 = indices[i + 1];
		UINT idx3 = indices[i + 2];

		// 3�̒��_���擾
		const XMFLOAT3& v1 = transformedVertices[idx1];
		const XMFLOAT3& v2 = transformedVertices[idx2];
		const XMFLOAT3& v3 = transformedVertices[idx3];

		// ���ʂ��v�Z
		Collision::Triangle plane = CalculatePlane(v1, v2, v3);
		planes.push_back(plane);
	}

	return planes;
}

//���_���s����g���Čv�Z 
std::vector<DirectX::XMFLOAT3> SlopeObject::TransformVertices(const std::vector<DirectX::XMFLOAT3>& vertices, const DirectX::XMMATRIX& transform)
{
	// �ϊ���̒��_���i�[����z��
	std::vector<XMFLOAT3> transformedVertices;

	for (const auto& vertex : vertices)
	{
		// ���݂̒��_��XMVECTOR�ɕϊ�
		XMVECTOR v = XMVectorSet(vertex.x, vertex.y, vertex.z, 1.0f);

		// �ϊ��s���K�p���Ē��_��ϊ�
		v = XMVector3Transform(v, transform);

		// �ϊ����XMVECTOR��XMFLOAT3�Ɋi�[
		XMFLOAT3 transformed;
		XMStoreFloat3(&transformed, v);

		transformedVertices.push_back(transformed);
	}

	// �ϊ���̒��_���X�g��Ԃ�
	return transformedVertices;
}