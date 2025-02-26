#include "Items.h"
#include "Geometry.h"

using namespace DirectX;

// �R���X�g���N�^
Items::Items(std::vector<ItemData>& _data)
{
	items.resize(_data.size());
	instanceDatas.reserve(_data.size());

	for (int i = 0; i < items.size(); i++)
	{
		// �I�u�W�F�N�g����
		items[i] = std::make_unique<ItemObject>(
			_data[i].position,
			_data[i].rotation,
			_data[i].scaling,
			_data[i].color);

		// ���[���h�ϊ��s����ŏ��Ɍv�Z�i�����Ȃ��I�u�W�F�N�g�j
		items[i]->SetMatrixWorld(XMMatrixTranspose(items[i]->GetMatrixWorld()));

		// �o�E���f�B���O�X�t�B�A�ݒ�
		// radius�͔���̗̈�{�����|����
		items[i]->SetBoundingSphere(_data[i].position, (_data[i].scaling.x * 0.5f) * collisionMultiplier);

		// ���̃I�u�W�F�N�g�̐F��ێ�����i�j��s�̂Ƃ��F��ς��邽�߁j
		items[i]->SetDefaultColor(_data[i].color);

		// �^�C�v�ƌ�̒l��ݒ�
		// SetBreakRatio�i�j��\�ȑ��x�����j
		// SetGettableTime�i�擾��Đ����܂ł̎��ԁj
		// SetScore�i���̃A�C�e���̎�ނ̃X�R�A���j
		switch (_data[i].type)
		{
		case ItemObject::SMALL:     // �A�C�e����
			items[i]->SetBreakRatio(smallBreakRatio);
			items[i]->SetGettableTime(smallAgainGettableTime);
			items[i]->SetScore(smallBaseScore);
			break;
		case ItemObject::MIDDLE:    // �A�C�e����
			items[i]->SetBreakRatio(middleBreakRatio);
			items[i]->SetGettableTime(middleAgainGettableTime);
			items[i]->SetScore(middleBaseScore);
			break;
		case ItemObject::BIG:       // �A�C�e����
			items[i]->SetBreakRatio(bigBreakRatio);
			items[i]->SetGettableTime(bigAgainGettableTime);
			items[i]->SetScore(bigBaseScore);
			break;
		case ItemObject::MEGA_BIG:  // �A�C�e������
			items[i]->SetBreakRatio(megabigBreakRatio);
			items[i]->SetGettableTime(megabigAgainGettableTime);
			items[i]->SetScore(megabigBaseScore);
			break;

		default:
			break;
		}

		// �^�C�v��ݒ�
		items[i]->SetType(_data[i].type);
	}

	// ���_����ݒ�
	// �X�t�B�A���b�V���Őݒ�
	Geometry::VertexData vd = Geometry::GetSphereVertexData();
	model.indexBuffer = vd.indexBuffer;
	model.vertexBuffer = vd.vertexBuffer;
	model.numIndex = vd.indexCount;
	model.texture = nullptr;
	model.matrixUV = XMMatrixIdentity();
}

// �X�V����
void Items::Update(float _deltaTime)
{
	for (auto& item : items)
	{
		item->Update(_deltaTime, realRatio, alphaDecayRate);
	}
}

// �C���X�^���V���O�`��
void Items::InstancingDraw()
{
	instanceDatas.clear();

	for (auto& item : items)
	{
		// �A�N�e�B�u��ԂƓ����蔻��\��Ԃ̏ꍇ�̂�
		if (item->GetIsActive() && item->GetIsTouched())
		{
			instanceDatas.push_back({ item->GetMatrixWorld() ,item->GetColor() });
		}
	}

	// �C���X�^���V���O�ŕ`��
	Renderer::DrawObjectInstancing(model, instanceDatas, mat, useVS, usePS, false, false, true);
}

void Items::DrawInstancingShadowMap()
{
	instanceDatas.clear();

	for (auto& item : items)
	{
		// �A�N�e�B�u��ԂƓ����蔻��\��Ԃ̏ꍇ�̂�
		if (item->GetIsActive() && item->GetIsTouched())
		{
			instanceDatas.push_back({ item->GetMatrixWorld() ,item->GetColor() });
		}
	}

	// �C���X�^���V���O�ŕ`��
	Renderer::DrawObjectInstancing(model, instanceDatas, std::nullopt, Shader::VS_INSTANCING_SHADOW_MAP, Shader::PS_NULLPTR, false, true, false);
}

#ifdef _DEBUG
void Items::DebugSingleDraw()
{
	
	for (auto& item : items)
	{
		// �A�N�e�B�u��ԂƓ����蔻��\��Ԃ̏ꍇ�̂�
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

// ���U�A�j���[�V����
void Items::ExplosionAnimation()
{
	for (auto& item : items)
	{
		// ���U��Ԃ̂Ƃ�
		if (item->GetState() == ItemObject::EXPLOSION)
		{
			Renderer::ObjectBuffer ob;
			// ���[���h�ϊ��s���ݒ�
			ob.matrixWorld = item->GetMatrixWorld();

			// �J���[�ݒ�i���l�̂ݎ��Ԍo�߂ɂ�铧���x��ݒ�j
			ob.color = item->GetColor();
			ob.color.w = item->GetFragmentAlpha();

			// GPU�ɑ��M����p�ɐݒ肵���o�b�t�@���擾
			Renderer::ShaderAnimationGSBuffer gsb = item->GetGSBuffer();

			// �ʏ�`��
			Renderer::DrawShaderAnimation(model, ob, gsb, std::nullopt, useAnimationVS, useAnimationPS, useAnimationGS, true);
		}
	}
}

// ���U�A�j���[�V�����Ŏg�p����V�F�[�_�[�Z�b�g
// ���_�E�s�N�Z���E�W�I���g���V�F�[�_�[
void Items::SetAnimationShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps, Shader::GS_NAME _gs)
{
	useAnimationVS = _vs;
	useAnimationPS = _ps;
	useAnimationGS = _gs;
}

// �e�A�C�e���̃^�C�v�ɔj��\�ȑ��x������ݒ�
void Items::SetBreakAllItemRatio(float smallRatio, float middleRatio, float bigRatio, float megabigRatio)
{
	for (auto& item : items)
	{
		switch (item->GetType())
		{
		case ItemObject::SMALL:     // �A�C�e����
			item->SetBreakRatio(smallRatio);
			break;
		case ItemObject::MIDDLE:    // �A�C�e����
			item->SetBreakRatio(middleRatio);
			break;
		case ItemObject::BIG:       // �A�C�e����
			item->SetBreakRatio(bigRatio);
			break;
		case ItemObject::MEGA_BIG:  // �A�C�e������
			item->SetBreakRatio(megabigRatio);
			break;
		default:
			break;
		}
	}
}