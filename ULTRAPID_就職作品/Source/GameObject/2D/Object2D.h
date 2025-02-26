#pragma once
#include "GameObject.h"

//===========================================//
// 2Dオブジェクト用クラス
//===========================================//
class Object2D : public GameObject
{
public:
	static constexpr float UiLowAlpha = 0.2f;   // 透明時のα値
	static constexpr float MaxTexPos = 2.0f;    // 最大の座標調整値
	static constexpr float MaxTexScale = 3.0f;  // 最大スケールの調整値

protected:
	Renderer::Model model;
	Shader::VS_NAME useVS = Shader::VS_TEXTURE; // 使用する頂点シェーダー
	Shader::PS_NAME usePS = Shader::PS_TEXTURE; // 使用するピクセルシェーダー
	DirectX::XMFLOAT2 uv = { 0,0 };            // UV座標

public:
	Object2D(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
	void Draw() override;
	// 画像が動く場合の描画処理
	void MoveDraw();
	// ワールド変換行列の再計算
	void ReCalculateWorldMatrix();
	// Set関数
	void SetModel(Renderer::Model& m) { model = m; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps);
	void SetUV(DirectX::XMFLOAT2 _uv) { uv = _uv; }
	// Get関数
	// UV座標取得
	DirectX::XMMATRIX GetUvMatrix() override;
	Renderer::Model& GetModel() { return model; }
	
};