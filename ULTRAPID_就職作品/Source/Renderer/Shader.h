#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include <unordered_map>

using Microsoft::WRL::ComPtr;

//===========================================//
// シェーダーに関するクラス
//===========================================//
class Shader
{
public:
	// 使用する頂点シェーダー
	enum VS_NAME
	{
		VS_TEXTURE,                // テクスチャ付き描画
		VS_INSTANCING_TEXTURE,     // インスタンシング（テクスチャ付き）
		VS_OBJECT,                 // テクスチャなし描画
		VS_INSTANCING,             // インスタンシング（テクスチャなし）
		VS_ITEM_ANIMATION,         // アイテムアニメーション専用
		VS_SPEED_GAUGE,            // 速度ゲージ専用
		VS_SHADOW_MAP,             // シャドウマップ専用
		VS_INSTANCING_SHADOW_MAP,  // シャドウマップ専用（インスタンシング）
		VS_AMBIENT_INSTANCING,     // 環境光用（インスタンシング）
		VS_TEXTURE_3D,
		VS_NULLPTR,
		VS_MAX,
	};
	// 使用するピクセルシェーダー
	enum PS_NAME
	{
		PS_TEXTURE,                // テクスチャ付き描画
		PS_SPECULER_TEXTURE,       // 光沢付き描画（テクスチャ付き）
		PS_SPECULER,               // 光沢付き描画（テクスチャなし）
		PS_COLOR,                  // 渡された色をそのまま出力
		PS_SPEED_GAUGE,            // 速度ゲージ専用
		PS_AMBIENT,                // 環境光（シャドウマップなし）
		PS_NULLPTR,
		PS_MAX,
	};
	// 使用するコンピュートシェーダー
	enum CS_NAME
	{
		CS_PARTICLE,               // パーティクルの座標計算
		CS_FRUSTUM,                // フラスタムカリング用の計算
		CS_BROAD_PHASE,            // ブロードフェーズ用の当たり判定
		CS_MAX,
	};
	// 使用するジオメトリシェーダー
	enum GS_NAME
	{
		GS_ITEM_ANIMATION,         // アイテムアニメーション専用
		GS_MAX,
	};

	// 頂点シェーダーデータ
	struct VertexShaderData
	{
		ComPtr<ID3D11VertexShader> vShader;
		ComPtr<ID3D11InputLayout> iLayuot;
		std::vector<D3D11_INPUT_ELEMENT_DESC> vDesc;
		std::string shaderPath;
	};

	// ピクセルシェーダーデータ
	struct PixelShaderData
	{
		ComPtr<ID3D11PixelShader> pShader;
		std::string shaderPath;
	};

	// コンピュートシェーダーデータ
	struct ComputeShaderData
	{
		ComPtr<ID3D11ComputeShader> cShader;
		std::string shaderPath;
	};

	// ジオメトリシェーダーデータ
	struct GeometryShaderData
	{
		ComPtr<ID3D11GeometryShader> gShader;
		std::string shaderPath;
	};

private:
	static std::unordered_map<VS_NAME, VertexShaderData> vertexShaderMap;
	static std::unordered_map<PS_NAME, PixelShaderData> pixelShaderMap;
	static std::unordered_map<CS_NAME, ComputeShaderData> computeShaderMap;
	static std::unordered_map<GS_NAME, GeometryShaderData> geometryShaderMap;

public:
	// シェーダー作成
	static void CreateShader();
	// 頂点シェーダー作成
	static void CreateVertexShader(VertexShaderData& vsData,ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel);
	// ピクセルシェーダー作成
	static void CreatePixelShader(PixelShaderData& psData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel);
	// コンピュートシェーダー作成
	static void CreateComputeShader(ComputeShaderData& csData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel);
	// ジオメトリシェーダー作成
	static void CreateGeometryShader(GeometryShaderData& gsData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel);
	// シェーダーコンパイル
	static HRESULT CompileShaderFromFile(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	static HRESULT CompileShader(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	static HRESULT LoadShaderFromCSO(const char* szFileName, ID3DBlob** ppBlobOut);
	
	// 各種シェーダーのGet関数
	static ID3D11VertexShader* GetVS(const VS_NAME _vsName) { return vertexShaderMap[_vsName].vShader.Get(); }
	static ID3D11PixelShader* GetPS(const PS_NAME _psName) { return  pixelShaderMap[_psName].pShader.Get(); }
	static ID3D11ComputeShader* GetCS(const CS_NAME _csName) { return  computeShaderMap[_csName].cShader.Get(); }
	static ID3D11GeometryShader* GetGS(const GS_NAME _gsName) { return  geometryShaderMap[_gsName].gShader.Get(); }
	static ID3D11InputLayout* GetLayout(const VS_NAME _vsName) { return  vertexShaderMap[_vsName].iLayuot.Get(); }

};