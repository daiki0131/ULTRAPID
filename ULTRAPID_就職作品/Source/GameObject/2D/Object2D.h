#pragma once
#include "GameObject.h"

//===========================================//
// 2D�I�u�W�F�N�g�p�N���X
//===========================================//
class Object2D : public GameObject
{
public:
	static constexpr float UiLowAlpha = 0.2f;   // �������̃��l
	static constexpr float MaxTexPos = 2.0f;    // �ő�̍��W�����l
	static constexpr float MaxTexScale = 3.0f;  // �ő�X�P�[���̒����l

protected:
	Renderer::Model model;
	Shader::VS_NAME useVS = Shader::VS_TEXTURE; // �g�p���钸�_�V�F�[�_�[
	Shader::PS_NAME usePS = Shader::PS_TEXTURE; // �g�p����s�N�Z���V�F�[�_�[
	DirectX::XMFLOAT2 uv = { 0,0 };            // UV���W

public:
	Object2D(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
	void Draw() override;
	// �摜�������ꍇ�̕`�揈��
	void MoveDraw();
	// ���[���h�ϊ��s��̍Čv�Z
	void ReCalculateWorldMatrix();
	// Set�֐�
	void SetModel(Renderer::Model& m) { model = m; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps);
	void SetUV(DirectX::XMFLOAT2 _uv) { uv = _uv; }
	// Get�֐�
	// UV���W�擾
	DirectX::XMMATRIX GetUvMatrix() override;
	Renderer::Model& GetModel() { return model; }
	
};