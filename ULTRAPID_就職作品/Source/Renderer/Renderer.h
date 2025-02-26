#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <atltypes.h>
#include <wrl/client.h>
#include <optional>
#include "Shader.h"

using Microsoft::WRL::ComPtr;

//===========================================//
// 描画全般に関するクラス
//===========================================//
class Renderer
{
public:
	static constexpr int InstancingMaxNum = 10000; // 一度のインスタンシングのGPUへの送信量
	static constexpr float ShadowMapSize = 2048.0f;     // シャドウマップの解像度

public:
	// 色情報
	struct Color
	{
		float r;
		float g;
		float b;
		float a;
	};

	// モデル情報
	struct Model
	{
		ComPtr<ID3D11Buffer> vertexBuffer;         // 頂点バッファ
		ComPtr<ID3D11ShaderResourceView> texture;  // テクスチャ
		ComPtr<ID3D11Buffer> indexBuffer;          // インデックスバッファ
		int numIndex;                              // インデックス数
		DirectX::XMMATRIX matrixUV;                // テクスチャUV行列

		Model() : 
			vertexBuffer(nullptr),
			texture(nullptr), 
			indexBuffer(nullptr),
			numIndex(0), 
			matrixUV(DirectX::XMMatrixIdentity())
		{
		}
	};

	//頂点データ
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
	};

	//===========================================//
	// 定数バッファ定義
	//===========================================//

	// カメラバッファ
	struct CameraBuffer
	{
		DirectX::XMMATRIX viewProj;
	};

	// ライトバッファ
	struct LightBuffer
	{
		DirectX::XMMATRIX viewProj;
	};
	
	// オブジェクトバッファ
	struct ObjectBuffer
	{
		DirectX::XMMATRIX matrixWorld;
		DirectX::XMFLOAT4 color;
	};

	// インスタンシングバッファ（カラー）
	struct InstancingColorBuffer
	{
		DirectX::XMFLOAT4 color[InstancingMaxNum];
	};

	// UV行列バッファ
	struct UVBuffer
	{
		DirectX::XMMATRIX matrixUV;
	};

	// マテリアル
	struct Material
	{
		DirectX::XMFLOAT4 emission;  // emissionColor (xyz) + emissionStrength (w)
		DirectX::XMFLOAT4 specular;  // specularColor (xyz) + specularStrength (w)
		DirectX::XMFLOAT4 ambient;   // ambientColor (xyz) + ambientStrength (w)
		DirectX::XMFLOAT4 other;     // chromaticAberrationStrength (x) + パディング (yzw)
	};

	// ジオメトリシェーダーアニメーションバッファ
	struct ShaderAnimationGSBuffer
	{
		float elapsedTime;           // 経過時間
		DirectX::XMFLOAT3 center;    // 中心座標
	};

	// 速度ゲージ専用のバッファ
	struct GaugeColorBuffer
	{
		DirectX::XMFLOAT4 color[4];   // 左から順の色
		DirectX::XMFLOAT4 thresholds; // 左から順の境界値
		float speedRatio;             // 現在の速度割合
		float lowAlphaValue;          // 半透明時のα値
		float padding[2];
	};
	
private:
	static ComPtr<ID3D11Device> device;                       // Direct3D 11 デバイス
	static ComPtr<ID3D11DeviceContext> deviceContext;         // デバイスコンテキスト (描画コマンドの発行に使用)
	static ComPtr<IDXGISwapChain> swapChain;                  // スワップチェーン (バックバッファとフロントバッファの管理)
	static ComPtr<ID3D11RenderTargetView> renderTargetView;   // レンダーターゲットビュー (描画結果の出力先)
	static ComPtr<ID3D11Texture2D> depthStencilTexture;       // 深度ステンシル用のテクスチャ (Zバッファ)
	static ComPtr<ID3D11DepthStencilView> depthStencilView;   // 深度ステンシルビュー (Zバッファのビュー)
	static ComPtr<ID3D11DepthStencilState> depthStencilState; // 深度ステンシルステート (深度バッファの比較方法の設定)
	static D3D11_VIEWPORT viewport;                           // ビューポート (描画領域の設定)
	static ComPtr<ID3D11BlendState> blendStateAlpha;          // ブレンドステート (αブレンド用)
	static ComPtr<ID3D11BlendState> blendStateAdditive;       // ブレンドステート (加算合成用)
	static ComPtr<ID3D11SamplerState> sampler;                // サンプラーステート (テクスチャのサンプリング設定)
	static ComPtr<ID3D11RasterizerState> noneCullState;       // ラスタライザーステート (カリングなし)
	static ComPtr<ID3D11RasterizerState> backCullState;    	  // ラスタライザーステート (裏面カリング)
	static UINT strides;                                      // 頂点バッファのストライド (1頂点あたりのバイト数)
	static UINT offsets;                                      // 頂点バッファのオフセット
	static CRect rect;                                        // 描画領域の矩形
	static ComPtr<ID3D11Buffer> cameraBuffer;                 // カメラ用定数バッファ
	static ComPtr<ID3D11Buffer> lightBuffer;                  // ライト用定数バッファ
	static ComPtr<ID3D11Buffer> objectBuffer;	              // オブジェクト用定数バッファ
	static ComPtr<ID3D11Buffer> uvBuffer;                     // UV座標用バッファ
	static ComPtr<ID3D11Buffer> instancingObjectBuffer;       // インスタンシング用のワールド行列バッファ
	static ComPtr<ID3D11Buffer> instancingColorBuffer;        // インスタンシング用のカラー情報バッファ
	static ComPtr<ID3D11Buffer> materialBuffer;               // マテリアル情報を格納するバッファ
	static ComPtr<ID3D11Buffer> gaugeBuffer;                  // 速度ゲージバッファ
	static ComPtr<ID3D11Buffer> shaderAnimationGSBuffer;      // シェーダーのアニメーション用ジオメトリシェーダーバッファ
	static DirectX::XMMATRIX cameraViewMatrix;                // カメラのビュー行列 
	static DirectX::XMMATRIX cameraProjectionMatrix;          // カメラのプロジェクション行列 
	static DirectX::XMMATRIX cameraViewProjection;	          // カメラのビュー・プロジェクション行列
	static DirectX::XMFLOAT3 cameraPos;                       // カメラのワールド座標
	static DirectX::XMMATRIX lightViewProjectionMatrix;       // ライトのビュー・プロジェクション行列
	static std::vector<Renderer::ObjectBuffer> instancingObjData;
	static ComPtr<ID3D11Texture2D> shadowMapTexture;
	static ComPtr<ID3D11DepthStencilView> shadowDepthStencilView;
	static ComPtr<ID3D11ShaderResourceView> shadowShaderResourceView;
	static ComPtr<ID3D11SamplerState> shadowSampler;
	static ComPtr<ID3D11DepthStencilState> shadowDepthStencilState;
	static ComPtr<ID3D11RenderTargetView> blurRenderTargetView;
	static ComPtr<ID3D11Texture2D> blurTexture;
	static ComPtr<ID3D11ShaderResourceView> blurShaderResourceView;


public:
	// 初期化
	static HRESULT Init(HWND hwnd);
	// スクリーンリセット
	static void ClearScreen();
	static void ClearShadowMapScreen();
	// スクリーン更新
	static void UpdateScreen();
	// オブジェクトファイル読み込み
	static Model LoadObjModel(const wchar_t* fileName);
	// テクスチャ読み込み
	static ComPtr<ID3D11ShaderResourceView> LoadTexture(const wchar_t* fileName);
	// Set関数
	static void SetCameraViewProjection(const DirectX::XMMATRIX& _view, const DirectX::XMMATRIX& _projection);
	static void SetLightViewProjection(const DirectX::XMMATRIX& _viewProjection) { lightViewProjectionMatrix = _viewProjection; };
	// Get関数
	static ComPtr<ID3D11Device> GetDevice() { return device; }
	static ComPtr<ID3D11DeviceContext> GetContext() { return deviceContext; }
	static ComPtr<ID3D11DepthStencilState> GetDepthStencilState() { return depthStencilState; }
	static ComPtr<ID3D11DepthStencilState> GetShadowDepthStencilState() { return shadowDepthStencilState; }
	static DirectX::XMMATRIX GetViewMatrix() { return cameraViewMatrix; }
	// 通常描画
	static void DrawObject(
		Renderer::Model& _model,
		Renderer::ObjectBuffer& _ob,
		const std::optional<Renderer::Material>& _mat,
		Shader::VS_NAME _vs, Shader::PS_NAME _ps,
		bool _backCulling,
		bool _writeDepth,
		bool _shadowMap);
	// インスタンシング描画
	static void DrawObjectInstancing(
		Renderer::Model& _model, 
		const std::vector<Renderer::ObjectBuffer>& _data,
		const std::optional<Renderer::Material>& _mat, 
		Shader::VS_NAME _vs, Shader::PS_NAME _ps, 
		bool _backCulling,
		bool _writeDepth,
		bool _shadowMap);
	//　ジオメトリシェーダーアニメーション描画
	static void DrawShaderAnimation(
		Renderer::Model& _model, 
		Renderer::ObjectBuffer& _ob,
		Renderer::ShaderAnimationGSBuffer& _gsb,
		const std::optional<Renderer::Material>& _mat, 
		Shader::VS_NAME _vs,
		Shader::PS_NAME _ps,
		Shader::GS_NAME _gs, 
		bool _backCulling);
	// 速度ゲージ専用描画
	static void DrawGaugeObject(
		Renderer::Model& _model,
		Renderer::ObjectBuffer& _ob,
		Renderer::GaugeColorBuffer& _cgb,
		Shader::VS_NAME _vs, Shader::PS_NAME _ps,
		bool _backCulling);
	// GPU送信バッファ更新
	static void UpdateBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize);
	// カリングモード切替
	static void SetCullingMode(bool _backCulling);
	// カメラをGPUにセット
	static void SetCameraBuffer();
	// ライトをGPUにセット
	static void SetLightBuffer();
	// マテリアルをGPUにセット
	static void SetMaterialBuffer(const std::optional<Renderer::Material>& _mat);
	// テクスチャリソースをGPUにセット
	static void SetTextureResources(Renderer::Model& _model);
	static void SetShadowMapTextureResources(bool _shadowMap);
	// シェーダーをGPUにセット
	static void SetupShaderAndPipeline(Shader::VS_NAME _vs, Shader::PS_NAME _ps);

};