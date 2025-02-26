#include "Shader.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <clocale>
#include "Renderer.h"
#include "DebugConsole.h"

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

std::unordered_map<Shader::VS_NAME, Shader::VertexShaderData> Shader::vertexShaderMap;
std::unordered_map<Shader::PS_NAME, Shader::PixelShaderData> Shader::pixelShaderMap;
std::unordered_map<Shader::CS_NAME, Shader::ComputeShaderData> Shader::computeShaderMap;
std::unordered_map<Shader::GS_NAME, Shader::GeometryShaderData> Shader::geometryShaderMap;

// シェーダー作成
void Shader::CreateShader()
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> mainInputLayout;
    mainInputLayout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0 });
    mainInputLayout.push_back({ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    mainInputLayout.push_back({ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });

    std::vector<D3D11_INPUT_ELEMENT_DESC> instancingInputLayout;
    instancingInputLayout.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0 });
    instancingInputLayout.push_back({ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    instancingInputLayout.push_back({ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    instancingInputLayout.push_back({ "INSTANCE_MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
    instancingInputLayout.push_back({ "INSTANCE_MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
    instancingInputLayout.push_back({ "INSTANCE_MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
    instancingInputLayout.push_back({ "INSTANCE_MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
    instancingInputLayout.push_back({ "INSTANCE_COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 });

    //=============================================================
    //  頂点シェーダー
    //=============================================================
    vertexShaderMap[VS_TEXTURE]               = { nullptr, nullptr,  mainInputLayout,"Shader/VS_Texture.cso"};
    vertexShaderMap[VS_INSTANCING_TEXTURE]    = { nullptr, nullptr,  instancingInputLayout,"Shader/VS_InstancingTexture.cso" };
    vertexShaderMap[VS_OBJECT]                = { nullptr, nullptr,  mainInputLayout,"Shader/VS_Object.cso" };
    vertexShaderMap[VS_INSTANCING]            = { nullptr, nullptr,  instancingInputLayout,"Shader/VS_Instancing.cso" };
    vertexShaderMap[VS_ITEM_ANIMATION]        = { nullptr, nullptr,  mainInputLayout,"Shader/VS_ItemAnimation.cso" };
    vertexShaderMap[VS_SPEED_GAUGE]           = { nullptr, nullptr,  mainInputLayout,"Shader/VS_SpeedGauge.cso" };
    vertexShaderMap[VS_SHADOW_MAP]            = { nullptr, nullptr,  mainInputLayout,"Shader/VS_ShadowMap.cso" };
    vertexShaderMap[VS_INSTANCING_SHADOW_MAP] = { nullptr, nullptr,  instancingInputLayout,"Shader/VS_ShadowMapInstancing.cso" };
    vertexShaderMap[VS_AMBIENT_INSTANCING]    = { nullptr, nullptr,  instancingInputLayout,"Shader/VS_AmbientColorInstancing.cso" };
    vertexShaderMap[VS_TEXTURE_3D]            = { nullptr, nullptr,  mainInputLayout,"Shader/VS_Texture3D.cso" };
    vertexShaderMap[VS_NULLPTR]               = { nullptr, nullptr,  {} ,"" };

    // 頂点シェーダー作成
    for (auto& entry : vertexShaderMap)
    {
        CreateVertexShader(entry.second, Renderer::GetDevice().Get(), "vs_main", "vs_5_0");
    }

    //=============================================================
    //  ピクセルシェーダー
    //=============================================================
    pixelShaderMap[PS_TEXTURE]          = { nullptr,"Shader/PS_Texture.cso" };
    pixelShaderMap[PS_SPECULER_TEXTURE] = { nullptr,"Shader/PS_SpeculerTexture.cso" };
    pixelShaderMap[PS_SPECULER]         = { nullptr,"Shader/PS_Speculer.cso" };
    pixelShaderMap[PS_COLOR]            = { nullptr,"Shader/PS_Color.cso" };
    pixelShaderMap[PS_SPEED_GAUGE]      = { nullptr,"Shader/PS_SpeedGauge.cso" };
    pixelShaderMap[PS_AMBIENT]          = { nullptr,"Shader/PS_AmbientColor.cso" };
    pixelShaderMap[PS_NULLPTR]          = { nullptr,"" };

    // ピクセルシェーダー作成
    for (auto& entry : pixelShaderMap)
    {
        CreatePixelShader(entry.second, Renderer::GetDevice().Get(), "ps_main", "ps_5_0");
    }

    //=============================================================
    //  コンピュートシェーダー
    //=============================================================
    computeShaderMap[CS_PARTICLE]    = { nullptr,"Shader/CS_Particle.cso" };
    computeShaderMap[CS_FRUSTUM]     = { nullptr,"Shader/CS_Frustum.cso" };
    computeShaderMap[CS_BROAD_PHASE] = { nullptr,"Shader/CS_BroadPhase.cso" };

    // コンピュートシェーダー作成
    for (auto& entry : computeShaderMap)
    {
        CreateComputeShader(entry.second, Renderer::GetDevice().Get(),"cs_main", "cs_5_0");
    }

    //=============================================================
    //  ジオメトリシェーダー
    //=============================================================
    geometryShaderMap[GS_ITEM_ANIMATION] = { nullptr,"Shader/GS_ItemAnimation.cso" };

    // ジオメトリシェーダー作成
    for (auto& entry : geometryShaderMap)
    {
        CreateGeometryShader(entry.second, Renderer::GetDevice().Get(), "gs_main", "gs_5_0");
    }
}

// 頂点シェーダー作成
void Shader::CreateVertexShader(VertexShaderData& vsData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel)
{
    if (vsData.shaderPath == "") return;

    HRESULT hr;

    ComPtr<ID3DBlob> pBlob;

    // CSOファイル読み込み
    hr = LoadShaderFromCSO(vsData.shaderPath.c_str(), pBlob.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("頂点シェーダー読み込み失敗：Shader::CreateVertexShader内で発生", DebugConsole::ERROR_LOG);
        return;
    }

    // 頂点シェーダーを生成
    hr = device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, vsData.vShader.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("頂点シェーダー生成失敗：Shader::CreateVertexShader内で発生", DebugConsole::ERROR_LOG);
        return;
    }

    unsigned int numElements = static_cast<unsigned int>(vsData.vDesc.size());
    // 頂点データ定義生成
    hr = device->CreateInputLayout(vsData.vDesc.data(), numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), vsData.iLayuot.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("インプットレイアウト生成失敗：Shader::CreateVertexShader内で発生", DebugConsole::ERROR_LOG);
        return;
    }
}

// ピクセルシェーダー作成
void Shader::CreatePixelShader(PixelShaderData& psData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel)
{
    if (psData.shaderPath == "") return;

    HRESULT hr;

    ComPtr<ID3DBlob> pBlob;

    // CSOファイル読み込み
    hr = LoadShaderFromCSO(psData.shaderPath.c_str(), pBlob.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("ピクセルシェーダー読み込み失敗：Shader::CreatePixelShader内で発生", DebugConsole::ERROR_LOG);
        return;
    }

    // ピクセルシェーダーを生成
    hr = device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, psData.pShader.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("ピクセルシェーダー生成失敗：Shader::CreatePixelShader内で発生", DebugConsole::ERROR_LOG);
        return;
    }
}

// コンピュートシェーダー作成
void Shader::CreateComputeShader(ComputeShaderData& csData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel)
{
    HRESULT hr;

    ComPtr<ID3DBlob> pBlob;

    // CSOファイル読み込み
    hr = LoadShaderFromCSO(csData.shaderPath.c_str(), pBlob.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("コンピュートシェーダー読み込み失敗：Shader::CreateComputeShader内で発生", DebugConsole::ERROR_LOG);
        return;
    }

    // コンピュートシェーダーを生成
    hr = device->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, csData.cShader.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("コンピュートシェーダー生成失敗：Shader::CreateComputeShader内で発生", DebugConsole::ERROR_LOG);
        return;
    }
}

// ジオメトリシェーダー作成
void Shader::CreateGeometryShader(GeometryShaderData& gsData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel)
{
    HRESULT hr;

    ComPtr<ID3DBlob> pBlob;

    // CSOファイル読み込み
    hr = LoadShaderFromCSO(gsData.shaderPath.c_str(), pBlob.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("ジオメトリシェーダー読み込み失敗：Shader::CreateGeometryShader内で発生", DebugConsole::ERROR_LOG);
        return;
    }

    // ジオメトリシェーダーを生成
    hr = device->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, gsData.gShader.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("ジオメトリシェーダー生成失敗：Shader::CreateGeometryShader内で発生", DebugConsole::ERROR_LOG);
        return;
    }
}

// シェーダーファイルコンパイル
HRESULT Shader::CompileShaderFromFile(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    // ファイル名用のWCHAR配列を準備
    WCHAR	filename[512];
    size_t 	wLen = 0;
    int err = 0;

    // char（マルチバイト文字列）をwchar_t（ワイド文字列）に変換
    setlocale(LC_ALL, "japanese");
    err = mbstowcs_s(&wLen, filename, 512, szFileName, _TRUNCATE);

    // シェーダーフラグの設定
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG_1 ) || defined( _DEBUG )
    // デバッグビルドの場合、デバッグ情報を含める
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
    // コンパイルエラー用のバッファ（エラー情報を格納）
    ComPtr<ID3DBlob> pErrorBlob;

    // シェーダーファイルをコンパイル
    hr = D3DCompileFromFile(
        filename,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        szEntryPoint,
        szShaderModel,
        dwShaderFlags,
        0,
        ppBlobOut,
        pErrorBlob.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("D3DCompileFromFile 失敗：Shader::CompileShaderFromFile内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    return S_OK;
}

// シェーダーをコンパイル
HRESULT Shader::CompileShader(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr;

    // 初期化
    *ppBlobOut = nullptr;

    // シェーダーのコンパイル
    hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, ppBlobOut);
    if (FAILED(hr)) 
    {
        DebugConsole::DebugLog("CompileShaderFromFile 失敗：Shader::CompileShader内で発生", DebugConsole::ERROR_LOG);
        return hr;
    }

    return S_OK;
}

// CSOファイル読み込み
HRESULT Shader::LoadShaderFromCSO(const char* szFileName, ID3DBlob** ppBlobOut)
{
    // 初期化
    *ppBlobOut = nullptr;

    // ファイル名を wchar に変換
    WCHAR filename[512];
    size_t wLen = 0;
    mbstowcs_s(&wLen, filename, szFileName, _TRUNCATE);

    // CSO ファイルを読み込む
    return D3DReadFileToBlob(filename, ppBlobOut);
}