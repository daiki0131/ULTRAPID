#include "Boxs.h"
#include "Geometry.h"

using namespace DirectX;

// �R���X�g���N�^
Boxs::Boxs(std::vector<BoxData>& _data)
{
	boxs.resize(_data.size());
	instanceDatas.reserve(_data.size()); // ���O�ɗe�ʂ��m��

	// _data�̃T�C�Y��_data�̓��e��box�Ɋi�[����
	for (int i = 0; i < boxs.size(); i++)
	{
		// �I�u�W�F�N�g����
		boxs[i] = std::make_unique<BoxObject>(
			_data[i].position,
			_data[i].rotation,
			_data[i].scaling,
			_data[i].color);

		// �o�E���f�B���O�{�b�N�X�EAABB�ݒ�
		boxs[i]->SetBoundingBox(_data[i].position, _data[i].scaling);
		boxs[i]->SetAABB(boxs[i]->GetBoundingBox());

		// �]�u�������[���h�ϊ��s��ݒ�
		boxs[i]->SetMatrixWorld(XMMatrixTranspose(boxs[i]->GetMatrixWorld()));

#ifdef _DEBUG
		float sphereRadius = boxs[i]->GetBoundingSphere().radius * 2.0f;
		XMFLOAT3 sphereScale = XMFLOAT3(sphereRadius, sphereRadius, sphereRadius);
		
		boxs[i]->SetDebugSphereWorldMatrix(XMMatrixTranspose(GameObject::CalculateMatrixWorld(
			_data[i].position,
			sphereScale,
			_data[i].rotation)));
#endif
	}

	// ���_����ݒ�
	// �{�b�N�X���b�V���Őݒ�
	Geometry::VertexData vd = Geometry::GetBoxVertexData();
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

// �C���X�^���V���O�`��i�V���h�E�}�b�v�K�p�j
void Boxs::InstancingWithShadowsDraw()
{
	SetObjectBuffer();
	
	// �C���X�^���V���O�ŕ`��
	Renderer::DrawObjectInstancing(model, instanceDatas, mat, useVS, usePS, false, false, true);
}

// �C���X�^���V���O�`��i�V���h�E�}�b�v�Ȃ��E�����̂݁j
void Boxs::InstancingWithoutShadowsDraw()
{
	SetObjectBuffer();

	// �C���X�^���V���O�ŕ`��
	Renderer::DrawObjectInstancing(model, instanceDatas, std::nullopt, useVS, usePS, false, false, false);
}

void Boxs::DrawInstancingShadowMap()
{
	SetObjectBuffer();

	// �C���X�^���V���O�ŕ`��
	Renderer::DrawObjectInstancing(model, instanceDatas, std::nullopt, Shader::VS_INSTANCING_SHADOW_MAP, Shader::PS_NULLPTR, false, true, false);
}

#ifdef _DEBUG
void Boxs::DebugSphereDraw()
{
	debugObjDatas.clear();
	
	for (int i = 0; i < boxs.size(); i++)
	{
		if (boxs[i]->GetIsCollisionCheck())
		{
			debugObjDatas.push_back({ boxs[i]->GetDebugSphereWorldMatrix() ,boxs[i]->GetDebugSphereColor()});
			XMFLOAT4 pos = boxs[i]->GetDebugSphereColor();
		}
	}

	Renderer::DrawObjectInstancing(sphereModel, debugObjDatas, std::nullopt, Shader::VS_AMBIENT_INSTANCING, Shader::PS_COLOR, false, false, false);
}
void Boxs::DebugSingleDraw()
{
	SetObjectBuffer();

	for (auto& obj : instanceDatas)
	{
		Renderer::ObjectBuffer ob;
		ob.matrixWorld = obj.matrixWorld;
		ob.color = obj.color;

		Renderer::DrawObject(model, ob, mat, Shader::VS_TEXTURE_3D, Shader::PS_SPECULER_TEXTURE, false, false, true);
	}
}
#endif

void Boxs::SetObjectBuffer()
{
	instanceDatas.clear();

	for (auto& box : boxs)
	{
		if (box->GetIsActive()) // �`��\���ǂ���
		{
			instanceDatas.push_back({ box->GetMatrixWorld() ,box->GetColor() });
		}
	}
}