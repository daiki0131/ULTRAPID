#pragma once
#include "BoxObject.h"
#include <vector>
#include <memory>

//===========================================//
// 3D�{�b�N�X���ꊇ�ŊǗ�����N���X
//===========================================//
class Boxs
{
public:
	// �{�b�N�X�f�[�^
	struct BoxData
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 scaling;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT4 color;
	};
public:
	std::vector<std::unique_ptr<BoxObject>> boxs; // �{�b�N�X�I�u�W�F�N�g�̃R���e�i

private:
	std::vector<Renderer::ObjectBuffer> instanceDatas;		// Renderer���M�p�I�u�W�F�N�g�f�[�^
	Renderer::Model model;
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;         // �g�p���钸�_�V�F�[�_�[
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;         // �g�p����s�N�Z���V�F�[�_�[
	Renderer::Material mat;                             // Renderer���M�p�}�e���A�����
#ifdef _DEBUG
	std::vector<Renderer::ObjectBuffer> debugObjDatas;
	Renderer::Model sphereModel;
#endif

public:
	Boxs(std::vector<BoxData>& _data);
	~Boxs() {};
	// �C���X�^���V���O�`��
	void InstancingWithShadowsDraw();
	void InstancingWithoutShadowsDraw();
	void DrawInstancingShadowMap();

#ifdef _DEBUG
	void DebugSphereDraw();
	void DebugSingleDraw();
#endif

	// Set�֐�
	void SetModel(Renderer::Model& m) { model = m; }
	void SetTexture(ComPtr<ID3D11ShaderResourceView> tex) { model.texture = tex; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; };
	void SetMaterial(Renderer::Material _mat) { mat = _mat; }
	// Get�֐�
	Renderer::Model& GetModel() { return model; }

private:
	void SetObjectBuffer();

};