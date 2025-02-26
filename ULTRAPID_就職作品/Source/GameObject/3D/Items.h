#pragma once
#include "ItemObject.h"
#include <vector>
#include <memory>

//===========================================//
// �Q�[�����A�C�e�����ꊇ�ň����N���X
//===========================================//
class Items
{
public:
	// �A�C�e���f�[�^
	struct ItemData
	{
		DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 scaling{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 rotation{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 color{ 0.0f, 0.0f, 0.0f, 0.0f };
		ItemObject::ITEM_TYPE type;
	};

public:
	std::vector<std::unique_ptr<ItemObject>> items; // �A�C�e���I�u�W�F�N�g�R���e�i

private:
	float smallBreakRatio = 0.0f;					// ���A�C�e���擾�\����
	float middleBreakRatio = 0.3f;					// ���A�C�e���擾�\����
	float bigBreakRatio = 0.7f;						// ��A�C�e���擾�\����
	float megabigBreakRatio = 0.9f;					// ����A�C�e���擾�\����

	float smallAgainGettableTime = 5.0f;			// ���A�C�e���Ď擾�\����
	float middleAgainGettableTime = 10.0f;			// ���A�C�e���Ď擾�\����
	float bigAgainGettableTime = 15.0f;				// ��A�C�e���Ď擾�\����
	float megabigAgainGettableTime = 20.0f;			// ����A�C�e���Ď擾�\����

	int smallBaseScore = 5;						    // ���A�C�e���l���X�R�A��
	int middleBaseScore = 10;						// ���A�C�e���l���X�R�A��
	int bigBaseScore = 30;							// ��A�C�e���l���X�R�A��
	int megabigBaseScore = 300;						// ����A�C�e���l���X�R�A��

	float alphaDecayRate = 0.7f;					// ���Ŏ��̃��l��������

private:
	std::vector<Renderer::ObjectBuffer> instanceDatas;			            // Renderer���M�p�I�u�W�F�N�g�f�[�^
	Renderer::Model model;
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;			        // �g�p���钸�_�V�F�[�_�[
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;			        // �g�p����s�N�Z���V�F�[�_�[
	Shader::VS_NAME useAnimationVS = Shader::VS_TEXTURE;      	// ���ŃA�j���[�V�������̒��_�V�F�[�_�[
	Shader::PS_NAME useAnimationPS = Shader::PS_TEXTURE;         // ���ŃA�j���[�V�������̃s�N�Z���V�F�[�_�[
	Shader::GS_NAME useAnimationGS = Shader::GS_ITEM_ANIMATION;  // ���ŃA�j���[�V�������̃W�I���g���V�F�[�_�[
	Renderer::Material mat;							            // Renderer���M�p�}�e���A�����
	float collisionMultiplier = 1.25f;                          // �v���C���[�Ƃ̓����蔻��̈�̔{��
	float realRatio = 1.0f;                                     // �v���C���[�̑��x����

public:
	Items(std::vector<ItemData>& _data);
	~Items() {};
	void Update(float _deltaTime);
	// �C���X�^���V���O�`��
	void InstancingDraw();
	void DrawInstancingShadowMap();
#ifdef _DEBUG
	void DebugSingleDraw();
#endif
	// ���ŃA�j���[�V����
	void ExplosionAnimation();
	// Set�֐�
	void SetModel(Renderer::Model& m) { model = m; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; };
	void SetAnimationShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps, Shader::GS_NAME _gs);
	void SetRealRatio(float _ratio) { realRatio = _ratio; }
	void SetMaterial(Renderer::Material _mat) { mat = _mat; }
	void SetBreakAllItemRatio(float smallRatio, float middleRatio, float bigRatio, float megabigRatio);
	// Get�֐�
	Renderer::Model& GetModel() { return model; }

};