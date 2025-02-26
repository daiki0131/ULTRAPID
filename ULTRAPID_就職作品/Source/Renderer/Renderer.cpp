#include "WaveFrontReader.h"
#include "WICTextureLoader.h"
#include "Renderer.h"
#include "DebugConsole.h"

#pragma comment (lib, "d3d11.lib")

using namespace DirectX;

ComPtr<ID3D11Device> Renderer::device;
ComPtr<ID3D11DeviceContext> Renderer::deviceContext;
ComPtr<IDXGISwapChain> Renderer::swapChain;
ComPtr<ID3D11RenderTargetView> Renderer::renderTargetView;
ComPtr<ID3D11Texture2D> Renderer::depthStencilTexture;
ComPtr<ID3D11DepthStencilView> Renderer::depthStencilView;
ComPtr<ID3D11DepthStencilState> Renderer::depthStencilState;
D3D11_VIEWPORT Renderer::viewport;
ComPtr<ID3D11BlendState> Renderer::blendStateAlpha;
ComPtr<ID3D11BlendState> Renderer::blendStateAdditive;
ComPtr<ID3D11SamplerState> Renderer::sampler;
ComPtr<ID3D11RasterizerState> Renderer::noneCullState;
ComPtr<ID3D11RasterizerState> Renderer::backCullState;
UINT Renderer::strides = sizeof(Renderer::Vertex);
UINT Renderer::offsets = 0;
CRect Renderer::rect;
ComPtr<ID3D11Buffer> Renderer::cameraBuffer;
ComPtr<ID3D11Buffer> Renderer::lightBuffer;
ComPtr<ID3D11Buffer> Renderer::objectBuffer;
ComPtr<ID3D11Buffer> Renderer::uvBuffer;
ComPtr<ID3D11Buffer> Renderer::instancingObjectBuffer;
ComPtr<ID3D11Buffer> Renderer::instancingColorBuffer;
ComPtr<ID3D11Buffer> Renderer::materialBuffer;
ComPtr<ID3D11Buffer> Renderer::gaugeBuffer;
ComPtr<ID3D11Buffer>  Renderer::shaderAnimationGSBuffer;
XMMATRIX Renderer::cameraViewMatrix;
XMMATRIX Renderer::cameraProjectionMatrix;
XMMATRIX Renderer::cameraViewProjection;
XMMATRIX Renderer::lightViewProjectionMatrix;
ComPtr<ID3D11Texture2D> Renderer::shadowMapTexture;
ComPtr<ID3D11DepthStencilView> Renderer::shadowDepthStencilView;
ComPtr<ID3D11ShaderResourceView> Renderer::shadowShaderResourceView;
ComPtr<ID3D11SamplerState> Renderer::shadowSampler;
ComPtr<ID3D11DepthStencilState> Renderer::shadowDepthStencilState;
std::vector<Renderer::ObjectBuffer> Renderer::instancingObjData;
ComPtr<ID3D11RenderTargetView> Renderer::blurRenderTargetView;
ComPtr<ID3D11Texture2D> Renderer::blurTexture;
ComPtr<ID3D11ShaderResourceView> Renderer::blurShaderResourceView;

HRESULT Renderer::Init(HWND hwnd)
{
	HRESULT hr;

	UINT flags = 0;

#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG; // デバッグモードの場合、デバッグフラグを設定
#endif

    // 使用するDirect3Dのバージョンを指定
	D3D_FEATURE_LEVEL pLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL level;

    DXGI_SWAP_CHAIN_DESC scDesc; // スワップチェインの設定

    // ウィンドウのクライアント領域のサイズを取得
    ::GetClientRect(hwnd, &rect);

    // スワップチェインの設定を初期化
    memset(&scDesc, 0, sizeof(scDesc));
    scDesc.BufferCount = 1;
    scDesc.BufferDesc.Width = rect.Width();
    scDesc.BufferDesc.Height = rect.Height();
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32bitのカラーフォーマット
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // レンダリング出力として使用
    scDesc.OutputWindow = hwnd;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.Windowed = TRUE; // ウィンドウモード

    // デバイスとスワップチェインを同時に作成する関数の呼び出し
    hr = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        flags,
        pLevels,
        1,
        D3D11_SDK_VERSION,
        &scDesc,
        swapChain.GetAddressOf(),
        device.GetAddressOf(),
        &level,
        deviceContext.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("D3D11CreateDeviceAndSwapChain 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }
        

    // バックバッファの取得
    ID3D11Texture2D* pBackBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("GetBuffer 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }
       
    // レンダーターゲットビューを作成
    hr = device->CreateRenderTargetView(pBackBuffer, NULL, renderTargetView.GetAddressOf());
    pBackBuffer->Release();
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateRenderTargetView 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }
     

    // 深度ステンシルステートの設定
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度バッファに書き込まない
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateDepthStencilState 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    D3D11_DEPTH_STENCIL_DESC dsDescShadow = {};
    dsDescShadow.DepthEnable = TRUE;
    dsDescShadow.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDescShadow.DepthFunc = D3D11_COMPARISON_LESS;
    dsDescShadow.StencilEnable = FALSE;

    hr = device->CreateDepthStencilState(&dsDescShadow, shadowDepthStencilState.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateDepthStencilState 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // 深度ステンシルバッファを作成
    D3D11_TEXTURE2D_DESC txDesc;
    memset(&txDesc, 0, sizeof(txDesc));
    txDesc.Width = rect.Width();
    txDesc.Height = rect.Height();
    txDesc.MipLevels = 1;
    txDesc.ArraySize = 1;
    txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    txDesc.SampleDesc.Count = 1;
    txDesc.SampleDesc.Quality = 0;
    txDesc.Usage = D3D11_USAGE_DEFAULT;
    txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    txDesc.CPUAccessFlags = 0;
    txDesc.MiscFlags = 0;
    hr = device->CreateTexture2D(&txDesc, NULL, depthStencilTexture.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateTexture2D 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // 深度ステンシルビューを作成
    D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
    memset(&dsDesc, 0, sizeof(dsDesc));
    dsDesc.Format = txDesc.Format;
    dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsDesc.Texture2D.MipSlice = 0;
    hr = device->CreateDepthStencilView(depthStencilTexture.Get(), &dsDesc, depthStencilView.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateDepthStencilView 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // ビューポートを作成
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)rect.Width();
    viewport.Height = (FLOAT)rect.Height();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // サンプラー作成
    D3D11_SAMPLER_DESC smpDesc;

    memset(&smpDesc, 0, sizeof(D3D11_SAMPLER_DESC));
    // 適用されるフィルター（アルゴリズム）の種類
    smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    hr = device->CreateSamplerState(&smpDesc, sampler.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateSamplerState 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    D3D11_SAMPLER_DESC shadowSamplerDesc = {};
    shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    shadowSamplerDesc.BorderColor[0] = 1.0f; // 白色の境界
    shadowSamplerDesc.MinLOD = 0.0f;
    shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&shadowSamplerDesc, shadowSampler.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateSamplerState 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }
    // ブレンドステート作成
    D3D11_BLEND_DESC BlendState;
    memset(&BlendState, 0, sizeof(D3D11_BLEND_DESC));
    BlendState.AlphaToCoverageEnable = FALSE;
    BlendState.IndependentBlendEnable = FALSE;
    BlendState.RenderTarget[0].BlendEnable = TRUE;
    BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    // 背景色に掛け合わせる係数
    BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = device->CreateBlendState(&BlendState, blendStateAlpha.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateBlendState 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    deviceContext->OMSetBlendState(blendStateAlpha.Get(), NULL, 0xffffffff);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // ラスタライザステート設定
    D3D11_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    hr = device->CreateRasterizerState(&rasterizerDesc, backCullState.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateRasterizerState 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    hr = device->CreateRasterizerState(&rasterizerDesc, noneCullState.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateRasterizerState 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // ======================================================
    //  定数バッファ作成
    // ======================================================
  
    // カメラバッファ
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.ByteWidth = sizeof(CameraBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    hr = device->CreateBuffer(&cbDesc, NULL, cameraBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("cameraBuffer 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // ライトバッファ
    cbDesc.ByteWidth = sizeof(LightBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, lightBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("lightBuffer 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // オブジェクトバッファ
    cbDesc.ByteWidth = sizeof(ObjectBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, objectBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("objectBuffer 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // UVバッファ
    cbDesc.ByteWidth = sizeof(UVBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, uvBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("uvBuffer 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // インスタンシングバッファ(WordMatrix)
    D3D11_BUFFER_DESC icbDesc;
    icbDesc.ByteWidth = sizeof(ObjectBuffer) * InstancingMaxNum;
    icbDesc.Usage = D3D11_USAGE_DYNAMIC; // 動的バッファの場合
    icbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 必須: 頂点バッファとしてバインド可能にする
    icbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU から書き込む場合
    icbDesc.MiscFlags = 0;
    icbDesc.StructureByteStride = 0;

    hr = device->CreateBuffer(&icbDesc, NULL, instancingObjectBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("instancingMatrixBuffer 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // インスタンシングバッファ(Color)
    cbDesc.ByteWidth = sizeof(InstancingColorBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, instancingColorBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("instancingColorBuffer 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // マテリアル
    cbDesc.ByteWidth = sizeof(Material);

    hr = device->CreateBuffer(&cbDesc, NULL, materialBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("materialBuffer 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // 速度ゲージ
    cbDesc.ByteWidth = sizeof(GaugeColorBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, gaugeBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("gaugeBuffer 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // GSBuffer
    cbDesc.ByteWidth = sizeof(ShaderAnimationGSBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, shaderAnimationGSBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("shaderAnimationGSBuffer 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // シャドウマップ用のテクスチャ
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = ShadowMapSize;
    textureDesc.Height = ShadowMapSize;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

    hr = device->CreateTexture2D(&textureDesc, NULL, shadowMapTexture.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateTexture2D 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // シャドウマップ用の深度ステンシルビューの作成
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = device->CreateDepthStencilView(shadowMapTexture.Get(), &dsvDesc, shadowDepthStencilView.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateDepthStencilView 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // シェーダーリソースビューの作成
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(shadowMapTexture.Get(), &srvDesc, shadowShaderResourceView.GetAddressOf());

    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateShaderResourceView 失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // ブラー専用のテクスチャ作成
    D3D11_TEXTURE2D_DESC blurTextureDesc = {};
    blurTextureDesc.Width = static_cast<UINT>(rect.Width());
    blurTextureDesc.Height = static_cast<UINT>(rect.Height());
    blurTextureDesc.MipLevels = 1;
    blurTextureDesc.ArraySize = 1;
    blurTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    blurTextureDesc.SampleDesc.Count = 1;
    blurTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    blurTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    hr = device->CreateTexture2D(&blurTextureDesc, nullptr, blurTexture.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("blurTexture 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // ブラー専用のレンダーターゲット作成
    D3D11_RENDER_TARGET_VIEW_DESC rtvDescBlur = {};
    rtvDescBlur.Format = blurTextureDesc.Format;
    rtvDescBlur.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDescBlur.Texture2D.MipSlice = 0;

    hr = device->CreateRenderTargetView(blurTexture.Get(), &rtvDescBlur, blurRenderTargetView.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("blurRenderTargetView 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    // ブラー専用のシェーダーリソースビュー作成
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDescBlur = {};
    srvDescBlur.Format = blurTextureDesc.Format;
    srvDescBlur.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDescBlur.Texture2D.MostDetailedMip = 0;
    srvDescBlur.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(blurTexture.Get(), &srvDescBlur, blurShaderResourceView.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("blurShaderResourceView 作成失敗しました：Renderer::Init内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    instancingObjData.reserve(InstancingMaxNum);

    return hr;
}


void Renderer::ClearScreen()
{
    // レンダーターゲットと深度ステンシルビューをセット
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    // ビューポートの設定
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>((FLOAT)rect.Width());
    viewport.Height = static_cast<float>((FLOAT)rect.Height());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // ビューポートをデバイスコンテキストに設定
    deviceContext->RSSetViewports(1, &viewport);

    // バッファのクリア
    float clearColor[] = { 0.1f, 0.1f, 0.3f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), clearColor);

    // 深度ステンシルバッファをクリア
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
   
}

void Renderer::ClearShadowMapScreen()
{
    deviceContext->OMSetRenderTargets(0, nullptr, shadowDepthStencilView.Get());

    // ビューポートの設定
    D3D11_VIEWPORT vp = {};
    vp.Width = ShadowMapSize;
    vp.Height = ShadowMapSize;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    deviceContext->RSSetViewports(1, &vp);

    deviceContext->ClearDepthStencilView(shadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::UpdateScreen()
{
    // フレームバッファをスワップチェインに提示 (画面更新)
    swapChain->Present(0, 0);
}

Renderer::Model Renderer::LoadObjModel(const wchar_t* fileName)
{
    //Objファイルを読み込めるようにする
    WaveFrontReader<uint32_t> reader;

    HRESULT hr;
    hr = reader.Load(fileName, true);

    // 読み込み失敗したら
    if (FAILED(hr))
    {
        DebugConsole::DebugLog(" モデル読み込み失敗：Renderer::LoadObjModelで発生", DebugConsole::ERROR_LOG);
        return Model();
    }

    // readerの中に読み込まれたデータを流し込む配列
    // 頂点の個数を取得する
    const int numVertex = static_cast<int>(reader.vertices.size());
    Vertex* vertexList;
    vertexList = new Vertex[numVertex]; // 配列の動的生成

    // 頂点データを流し込む
    for (int i = 0; i < numVertex; i++)
    {
        vertexList[i].position = reader.vertices[i].position;
        vertexList[i].uv = reader.vertices[i].textureCoordinate;
        // 法線ベクトルを取得
        vertexList[i].normal = reader.vertices[i].normal;

        // 右手座標系モデルのUV対策
        vertexList[i].uv.y = 1.0f - vertexList[i].uv.y;

        // 法線ベクトルが逆なので反転する
        vertexList[i].normal.x *= -1;
        vertexList[i].normal.y *= -1;
        vertexList[i].normal.z *= -1;
    }

    // 頂点バッファを作成する
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(Vertex) * numVertex;  // 確保するバッファサイズを指定
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // 頂点バッファ作成を指定
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subResourceData;
    subResourceData.pSysMem = vertexList;  // VRAMに送るデータを指定
    subResourceData.SysMemPitch = 0;
    subResourceData.SysMemSlicePitch = 0;

    Model model = {};
    // 頂点バッファ作成して、それを変数（第３引数）に保存
    hr = device->CreateBuffer(&bufferDesc, &subResourceData, model.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog(" vertexBuffer 作成失敗：Renderer::LoadObjModelで発生", DebugConsole::ERROR_LOG);
        return Model();
    }

    D3D11_BUFFER_DESC ibDesc;
    ibDesc.ByteWidth = sizeof(UINT) * static_cast<UINT>(reader.indices.size()); // 2バイト×個数
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;
    ibDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA irData;
    irData.pSysMem = reader.indices.data(); // vector.data関数で配列データ取得
    irData.SysMemPitch = 0;
    irData.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&ibDesc, &irData, model.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog(" indexBuffer 作成失敗：Renderer::LoadObjModelで発生", DebugConsole::ERROR_LOG);
        return Model();
    }

    // インデックス数を保存
    model.numIndex = static_cast<int>(reader.indices.size());

    // 動的生成した配列を解放
    delete[] vertexList;

    // テクスチャ読み込み
    // ファイル名を取得してフォルダ名と結合する
    wchar_t textureName[256];
    wcscpy_s(textureName, L"Assets/Texture/");                // 文字列コピー関数
    wcscat_s(textureName, reader.materials[1].strTexture);    // 文字列結合関数
    model.texture = LoadTexture(textureName);

    return model;
}

ComPtr<ID3D11ShaderResourceView> Renderer::LoadTexture(const wchar_t* fileName)
{
    ComPtr<ID3D11ShaderResourceView> texture;

    if (!fileName)
    {
        DebugConsole::DebugLog(" ファイル名が NULL です：Renderer::LoadTextureで発生", DebugConsole::ERROR_LOG);
        return nullptr;
    }

    HRESULT hr;
    // 第４引数：読み込んだテクスチャを保持する変数
    hr = DirectX::CreateWICTextureFromFile(device.Get(), fileName, NULL, texture.GetAddressOf());

    // 読み込みに失敗した場合
    if (FAILED(hr))
    {
        DebugConsole::DebugLog(" テクスチャ読み込みに失敗：Renderer::LoadTextureで発生", DebugConsole::ERROR_LOG);
    }

    return texture;
}

// カメラ行列を設定
void Renderer::SetCameraViewProjection(const DirectX::XMMATRIX& _view, const DirectX::XMMATRIX& _projection)
{
    cameraViewMatrix = _view;
    cameraProjectionMatrix = _projection;
    cameraViewProjection =  XMMatrixTranspose(_view * _projection);
}

// カメラバッファをGPUセット
void Renderer::SetCameraBuffer()
{
    // カメラ用の定数バッファ
    Renderer::CameraBuffer cb;
    cb.viewProj = cameraViewProjection;

    // バッファ更新
    UpdateBuffer(cameraBuffer.Get(), &cb, sizeof(CameraBuffer));

    // 頂点シェーダーにセット
    deviceContext->VSSetConstantBuffers(0, 1, cameraBuffer.GetAddressOf());
}

// ライトバッファをGPUセット
void Renderer::SetLightBuffer()
{
    Renderer::LightBuffer lb;
    lb.viewProj = XMMatrixTranspose(lightViewProjectionMatrix);

    // バッファ更新
    UpdateBuffer(lightBuffer.Get(), &lb, sizeof(LightBuffer));

    // 頂点シェーダーにセット
    deviceContext->VSSetConstantBuffers(4, 1, lightBuffer.GetAddressOf());
}

// マテリアルバッファをGPUセット
void Renderer::SetMaterialBuffer(const std::optional<Renderer::Material>& _mat)
{
    // マテリアルが設定されている場合
    if (_mat)
    {
        // バッファ更新
        UpdateBuffer(materialBuffer.Get(), &_mat, sizeof(Material));

        // ピクセルシェーダーにセット
        deviceContext->PSSetConstantBuffers(0, 1, materialBuffer.GetAddressOf());
    }
}

// テクスチャをGPUセット
void Renderer::SetTextureResources(Renderer::Model& _model)
{
    // テクスチャが設定されている場合
    if (_model.texture)
    {
        // ピクセルシェーダにテクスチャを設定
        deviceContext->PSSetShaderResources(0, 1, _model.texture.GetAddressOf());

        // ピクセルシェーダにサンプラーを設定
        deviceContext->PSSetSamplers(0, 1, sampler.GetAddressOf());

        // UV行列データを定数バッファに設定
        Renderer::UVBuffer ub;
        ub.matrixUV = _model.matrixUV;

        // バッファ更新
        UpdateBuffer(uvBuffer.Get(), &ub, sizeof(UVBuffer));

        // 頂点シェーダにセット
        deviceContext->VSSetConstantBuffers(3, 1, uvBuffer.GetAddressOf());
    }
}

void Renderer::SetShadowMapTextureResources(bool _shadowMap)
{
    if (_shadowMap)
    {
        // ピクセルシェーダにテクスチャを設定
        deviceContext->PSSetShaderResources(1, 1, shadowShaderResourceView.GetAddressOf());

        // ピクセルシェーダにサンプラーを設定
        deviceContext->PSSetSamplers(1, 1, shadowSampler.GetAddressOf());
    }
}

// シェーダーをセット
void Renderer::SetupShaderAndPipeline(Shader::VS_NAME _vs, Shader::PS_NAME _ps)
{
    deviceContext->IASetInputLayout(Shader::GetLayout(_vs));
    deviceContext->VSSetShader(Shader::GetVS(_vs), nullptr, 0);

    deviceContext->PSSetShader(Shader::GetPS(_ps), nullptr, 0);
   
}

// オブジェクトの通常描画（単体描画）
void Renderer::DrawObject(
    Renderer::Model& _model,
    Renderer::ObjectBuffer& _ob, 
    const std::optional<Renderer::Material>& _mat, 
    Shader::VS_NAME _vs, 
    Shader::PS_NAME _ps, 
    bool _backCulling,
    bool _writeDepth,
    bool _shadowMap)
{
 
    if (!_writeDepth)
    {
        // カメラ情報を定数バッファに設定
        SetCameraBuffer();
    }
   
    // ライト情報を定数バッファに設定
    SetLightBuffer();

    // マテリアル情報を定数バッファに設定
    SetMaterialBuffer(_mat);

    // モデルのテクスチャリソースを設定
    SetTextureResources(_model);

    // バックカリング設定
    SetCullingMode(_backCulling);

    // 頂点シェーダとピクセルシェーダを設定
    SetupShaderAndPipeline(_vs, _ps);

    // オブジェクトバッファを更新
    UpdateBuffer(objectBuffer.Get(), &_ob, sizeof(ObjectBuffer));

    // 頂点シェーダにオブジェクトバッファを設定
    deviceContext->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());

    // 頂点バッファを設定
    deviceContext->IASetVertexBuffers(0, 1, _model.vertexBuffer.GetAddressOf(), &strides, &offsets);

    // インデックスバッファを設定
    deviceContext->IASetIndexBuffer(_model.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // インデックス付き描画を実行
    deviceContext->DrawIndexed(_model.numIndex, 0, 0);
}


// インスタンシング描画
void Renderer::DrawObjectInstancing(
    Renderer::Model& _model, 
    const std::vector<Renderer::ObjectBuffer>& _data,
    const std::optional<Renderer::Material>& _mat, 
    Shader::VS_NAME _vs, 
    Shader::PS_NAME _ps,
    bool _backCulling, 
    bool _writeDepth,
    bool _shadowMap)
{
    
    if (!_writeDepth)
    {
        // カメラ情報を定数バッファに設定
        SetCameraBuffer();
    }

    // ライト情報を定数バッファに設定
    SetLightBuffer();

    // マテリアル情報を定数バッファに設定
    SetMaterialBuffer(_mat);

    // モデルのテクスチャリソースを設定
    SetTextureResources(_model);

    SetShadowMapTextureResources(_shadowMap);

    // バックカリング設定
    SetCullingMode(_backCulling);

    // 頂点シェーダとピクセルシェーダを設定
    SetupShaderAndPipeline(_vs, _ps);

    for (int i = 0; i < _data.size(); i += InstancingMaxNum)
    {
        int instanceCount = std::min(InstancingMaxNum, static_cast<int>(_data.size()) - i);

        // インスタンシング用のワールド変換行列バッファを更新
        UpdateBuffer(instancingObjectBuffer.Get(), _data.data() + i, sizeof(ObjectBuffer) * instanceCount);

        UINT strides[2] = { sizeof(Renderer::Vertex), sizeof(ObjectBuffer) };
        UINT offsets[2] = { 0, 0 };
        ID3D11Buffer* buffers[2] = { _model.vertexBuffer.Get(), instancingObjectBuffer.Get() };

        // 頂点バッファの設定
        deviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
        deviceContext->IASetIndexBuffer(_model.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        // インスタンシングを使って描画を実行
        deviceContext->DrawIndexedInstanced(_model.numIndex, instanceCount, 0, 0, 0);
    }
}

// ジオメトリシェーダーアニメーション
void Renderer::DrawShaderAnimation(Renderer::Model& _model, Renderer::ObjectBuffer& _ob, Renderer::ShaderAnimationGSBuffer& _gsb, const std::optional<Renderer::Material>& _mat, Shader::VS_NAME _vs, Shader::PS_NAME _ps, Shader::GS_NAME _gs, bool _backCulling)
{
    // カメラ情報を定数バッファに設定
    SetCameraBuffer();

    // マテリアル情報を定数バッファに設定
    SetMaterialBuffer(_mat);

    // モデルのテクスチャリソースを設定
    SetTextureResources(_model);

    // バックカリング設定
    SetCullingMode(_backCulling);

    // 頂点シェーダとピクセルシェーダを設定
    SetupShaderAndPipeline(_vs, _ps);

    // オブジェクトバッファを更新
    UpdateBuffer(objectBuffer.Get(), &_ob, sizeof(ObjectBuffer));

    // シェーダーアニメーション用のバッファを更新
    UpdateBuffer(shaderAnimationGSBuffer.Get(), &_gsb, sizeof(ShaderAnimationGSBuffer));

    // 定数バッファをシェーダに設定
    deviceContext->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
    deviceContext->GSSetConstantBuffers(0, 1, shaderAnimationGSBuffer.GetAddressOf());
    deviceContext->GSSetConstantBuffers(1, 1, cameraBuffer.GetAddressOf());
   
    // ジオメトリシェーダを設定
    deviceContext->GSSetShader(Shader::GetGS(_gs), NULL, 0);

    // 頂点バッファとインデックスバッファを設定
    deviceContext->IASetVertexBuffers(0, 1, _model.vertexBuffer.GetAddressOf(), &strides, &offsets);
    deviceContext->IASetIndexBuffer(_model.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // インデックス付き描画を実行
    deviceContext->DrawIndexed(_model.numIndex, 0, 0);

    // ジオメトリシェーダを無効化
    deviceContext->GSSetShader(NULL, NULL, 0);
}

// 速度ゲージ専用の描画
void Renderer::DrawGaugeObject(Renderer::Model& _model, Renderer::ObjectBuffer& _ob, Renderer::GaugeColorBuffer& _cgb, Shader::VS_NAME _vs, Shader::PS_NAME _ps, bool _backCulling)
{
    // カメラ情報を定数バッファに設定
    SetCameraBuffer();

    // バックカリングの設定
    SetCullingMode(_backCulling);

    // 頂点シェーダとピクセルシェーダを設定
    SetupShaderAndPipeline(_vs, _ps);

    // オブジェクトバッファを更新し、定数バッファに送信
    UpdateBuffer(objectBuffer.Get(), &_ob, sizeof(ObjectBuffer));

    // UV座標用のバッファを更新し、定数バッファに送信
    Renderer::UVBuffer ub;
    ub.matrixUV = _model.matrixUV;
    UpdateBuffer(uvBuffer.Get(), &ub, sizeof(UVBuffer));

    // ゲージ色用のバッファを更新し、定数バッファに送信
    UpdateBuffer(gaugeBuffer.Get(), &_cgb, sizeof(GaugeColorBuffer));
  
    // 各シェーダの定数バッファを設定
    deviceContext->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
    deviceContext->VSSetConstantBuffers(2, 1, uvBuffer.GetAddressOf());
    deviceContext->PSSetConstantBuffers(0, 1, gaugeBuffer.GetAddressOf());

    // 頂点バッファとインデックスバッファを設定
    deviceContext->IASetVertexBuffers(0, 1, _model.vertexBuffer.GetAddressOf(), &strides, &offsets);
    deviceContext->IASetIndexBuffer(_model.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // インデックス付き描画を実行
    deviceContext->DrawIndexed(_model.numIndex, 0, 0);
}

// バッファを更新する
void Renderer::UpdateBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    // バッファをマップ
    HRESULT hr = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) 
    {
        DebugConsole::DebugLog(" バッファ更新失敗：Renderer::UpdateBufferで発生", DebugConsole::ERROR_LOG);
    }

    // データをコピー
    memcpy(mappedResource.pData, data, dataSize);

    // マップ解除
    deviceContext->Unmap(buffer, 0);
}

// カリングモードを設定する
void Renderer::SetCullingMode(bool _backCulling)
{
    ID3D11RasterizerState* cullState = _backCulling ? backCullState.Get() : noneCullState.Get();
    deviceContext->RSSetState(cullState);
}