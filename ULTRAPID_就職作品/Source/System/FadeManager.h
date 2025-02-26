#pragma once
#include "GameObject.h"

//===========================================//
// �Q�[�����t�F�[�h���Ǘ�����N���X
//===========================================//
class FadeManager : public GameObject
{
private:
	float fadeAlpha = 1.0f;                       // ���l
	float fadeSpeed = 1.0f;                       // �t�F�[�h���x
	bool isFade = false;                          // �t�F�[�h��
	bool isFadeIn = true;                         // �t�F�[�h�C����
	Renderer::Model model; 
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;    // �g�p���钸�_�V�F�[�_�[
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;    // �g�p����s�N�Z���V�F�[�_�[

public:
	FadeManager(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
	void Update(float _deltaTime) override;
	void Draw() override;
	// �t�F�[�h�C���J�n
	void StartFadeIn();
	// �t�F�[�h�A�E�g�J�n
	void StartFadeOut();
	// Set�֐�
	void SetModel(Renderer::Model& m) { model = m; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; }
	void SetIMGUI();
	void SetSpeed(float _speed) { fadeSpeed = _speed; }
	// Get�֐�
	bool GetIsFade() { return isFade; }
	Renderer::Model& GetModel() { return model; }
	float GetSpeed() { return fadeSpeed; }

};