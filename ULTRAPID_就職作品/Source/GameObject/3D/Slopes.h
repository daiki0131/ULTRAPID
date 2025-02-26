#pragma once
#include "SlopeObject.h"
#include <vector>
#include <memory>

//===========================================//
// �⓹�I�u�W�F�N�g���ꊇ�ň����N���X
//===========================================//
class Slopes
{
public:
	// �⓹�f�[�^
	struct SlopeData
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 scaling;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT4 color;
	};

public:
	std::vector<std::unique_ptr<SlopeObject>> slopes; // �⓹�I�u�W�F�N�g�R���e�i

private:
	std::vector<Renderer::ObjectBuffer> instanceDetas;			  // Renderer���M�p�I�u�W�F�N�g�f�[�^
	Renderer::Model model;
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;	      // �g�p���钸�_�V�F�[�_�[
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;        // �g�p����s�N�Z���V�F�[�_�[
	Renderer::Material mat {};                        // Renderer���M�p�}�e���A�����

#ifdef _DEBUG
	std::vector<Renderer::ObjectBuffer> debugObjDatas;
	std::vector<DirectX::XMMATRIX> debugWorldMatrixs;
	Renderer::Model sphereModel;
#endif

public:
	Slopes(std::vector<SlopeData>& _data);
	// �C���X�^���V���O�`��
	void InstancingDraw();

#ifdef _DEBUG
	void DebugSphereDraw();
#endif

	// Set�֐�
	void SetModel(Renderer::Model& m) { model = m; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; }
	void SetMaterial(Renderer::Material _mat) { mat = _mat; }
	// Get�֐�
	Renderer::Model& GetModel() { return model; }

};