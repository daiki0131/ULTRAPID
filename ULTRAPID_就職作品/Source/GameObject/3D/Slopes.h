#pragma once
#include "SlopeObject.h"
#include <vector>
#include <memory>

//===========================================//
// 坂道オブジェクトを一括で扱うクラス
//===========================================//
class Slopes
{
public:
	// 坂道データ
	struct SlopeData
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 scaling;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT4 color;
	};

public:
	std::vector<std::unique_ptr<SlopeObject>> slopes; // 坂道オブジェクトコンテナ

private:
	std::vector<Renderer::ObjectBuffer> instanceDetas;			  // Renderer送信用オブジェクトデータ
	Renderer::Model model;
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;	      // 使用する頂点シェーダー
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;        // 使用するピクセルシェーダー
	Renderer::Material mat {};                        // Renderer送信用マテリアル情報

#ifdef _DEBUG
	std::vector<Renderer::ObjectBuffer> debugObjDatas;
	std::vector<DirectX::XMMATRIX> debugWorldMatrixs;
	Renderer::Model sphereModel;
#endif

public:
	Slopes(std::vector<SlopeData>& _data);
	// インスタンシング描画
	void InstancingDraw();

#ifdef _DEBUG
	void DebugSphereDraw();
#endif

	// Set関数
	void SetModel(Renderer::Model& m) { model = m; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; }
	void SetMaterial(Renderer::Material _mat) { mat = _mat; }
	// Get関数
	Renderer::Model& GetModel() { return model; }

};