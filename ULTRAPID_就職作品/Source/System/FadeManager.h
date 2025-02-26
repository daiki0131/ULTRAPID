#pragma once
#include "GameObject.h"

//===========================================//
// ゲーム内フェードを管理するクラス
//===========================================//
class FadeManager : public GameObject
{
private:
	float fadeAlpha = 1.0f;                       // α値
	float fadeSpeed = 1.0f;                       // フェード速度
	bool isFade = false;                          // フェード可否
	bool isFadeIn = true;                         // フェードイン可否
	Renderer::Model model; 
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;    // 使用する頂点シェーダー
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;    // 使用するピクセルシェーダー

public:
	FadeManager(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
	void Update(float _deltaTime) override;
	void Draw() override;
	// フェードイン開始
	void StartFadeIn();
	// フェードアウト開始
	void StartFadeOut();
	// Set関数
	void SetModel(Renderer::Model& m) { model = m; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; }
	void SetIMGUI();
	void SetSpeed(float _speed) { fadeSpeed = _speed; }
	// Get関数
	bool GetIsFade() { return isFade; }
	Renderer::Model& GetModel() { return model; }
	float GetSpeed() { return fadeSpeed; }

};