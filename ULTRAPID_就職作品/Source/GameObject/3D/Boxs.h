#pragma once
#include "BoxObject.h"
#include <vector>
#include <memory>

//===========================================//
// 3Dボックスを一括で管理するクラス
//===========================================//
class Boxs
{
public:
	// ボックスデータ
	struct BoxData
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 scaling;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT4 color;
	};
public:
	std::vector<std::unique_ptr<BoxObject>> boxs; // ボックスオブジェクトのコンテナ

private:
	std::vector<Renderer::ObjectBuffer> instanceDatas;		// Renderer送信用オブジェクトデータ
	Renderer::Model model;
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;         // 使用する頂点シェーダー
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;         // 使用するピクセルシェーダー
	Renderer::Material mat;                             // Renderer送信用マテリアル情報
#ifdef _DEBUG
	std::vector<Renderer::ObjectBuffer> debugObjDatas;
	Renderer::Model sphereModel;
#endif

public:
	Boxs(std::vector<BoxData>& _data);
	~Boxs() {};
	// インスタンシング描画
	void InstancingWithShadowsDraw();
	void InstancingWithoutShadowsDraw();
	void DrawInstancingShadowMap();

#ifdef _DEBUG
	void DebugSphereDraw();
	void DebugSingleDraw();
#endif

	// Set関数
	void SetModel(Renderer::Model& m) { model = m; }
	void SetTexture(ComPtr<ID3D11ShaderResourceView> tex) { model.texture = tex; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; };
	void SetMaterial(Renderer::Material _mat) { mat = _mat; }
	// Get関数
	Renderer::Model& GetModel() { return model; }

private:
	void SetObjectBuffer();

};