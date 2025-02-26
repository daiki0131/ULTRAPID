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

// �V�F�[�_�[�쐬
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
    //  ���_�V�F�[�_�[
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

    // ���_�V�F�[�_�[�쐬
    for (auto& entry : vertexShaderMap)
    {
        CreateVertexShader(entry.second, Renderer::GetDevice().Get(), "vs_main", "vs_5_0");
    }

    //=============================================================
    //  �s�N�Z���V�F�[�_�[
    //=============================================================
    pixelShaderMap[PS_TEXTURE]          = { nullptr,"Shader/PS_Texture.cso" };
    pixelShaderMap[PS_SPECULER_TEXTURE] = { nullptr,"Shader/PS_SpeculerTexture.cso" };
    pixelShaderMap[PS_SPECULER]         = { nullptr,"Shader/PS_Speculer.cso" };
    pixelShaderMap[PS_COLOR]            = { nullptr,"Shader/PS_Color.cso" };
    pixelShaderMap[PS_SPEED_GAUGE]      = { nullptr,"Shader/PS_SpeedGauge.cso" };
    pixelShaderMap[PS_AMBIENT]          = { nullptr,"Shader/PS_AmbientColor.cso" };
    pixelShaderMap[PS_NULLPTR]          = { nullptr,"" };

    // �s�N�Z���V�F�[�_�[�쐬
    for (auto& entry : pixelShaderMap)
    {
        CreatePixelShader(entry.second, Renderer::GetDevice().Get(), "ps_main", "ps_5_0");
    }

    //=============================================================
    //  �R���s���[�g�V�F�[�_�[
    //=============================================================
    computeShaderMap[CS_PARTICLE]    = { nullptr,"Shader/CS_Particle.cso" };
    computeShaderMap[CS_FRUSTUM]     = { nullptr,"Shader/CS_Frustum.cso" };
    computeShaderMap[CS_BROAD_PHASE] = { nullptr,"Shader/CS_BroadPhase.cso" };

    // �R���s���[�g�V�F�[�_�[�쐬
    for (auto& entry : computeShaderMap)
    {
        CreateComputeShader(entry.second, Renderer::GetDevice().Get(),"cs_main", "cs_5_0");
    }

    //=============================================================
    //  �W�I���g���V�F�[�_�[
    //=============================================================
    geometryShaderMap[GS_ITEM_ANIMATION] = { nullptr,"Shader/GS_ItemAnimation.cso" };

    // �W�I���g���V�F�[�_�[�쐬
    for (auto& entry : geometryShaderMap)
    {
        CreateGeometryShader(entry.second, Renderer::GetDevice().Get(), "gs_main", "gs_5_0");
    }
}

// ���_�V�F�[�_�[�쐬
void Shader::CreateVertexShader(VertexShaderData& vsData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel)
{
    if (vsData.shaderPath == "") return;

    HRESULT hr;

    ComPtr<ID3DBlob> pBlob;

    // CSO�t�@�C���ǂݍ���
    hr = LoadShaderFromCSO(vsData.shaderPath.c_str(), pBlob.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("���_�V�F�[�_�[�ǂݍ��ݎ��s�FShader::CreateVertexShader���Ŕ���", DebugConsole::ERROR_LOG);
        return;
    }

    // ���_�V�F�[�_�[�𐶐�
    hr = device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, vsData.vShader.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("���_�V�F�[�_�[�������s�FShader::CreateVertexShader���Ŕ���", DebugConsole::ERROR_LOG);
        return;
    }

    unsigned int numElements = static_cast<unsigned int>(vsData.vDesc.size());
    // ���_�f�[�^��`����
    hr = device->CreateInputLayout(vsData.vDesc.data(), numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), vsData.iLayuot.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("�C���v�b�g���C�A�E�g�������s�FShader::CreateVertexShader���Ŕ���", DebugConsole::ERROR_LOG);
        return;
    }
}

// �s�N�Z���V�F�[�_�[�쐬
void Shader::CreatePixelShader(PixelShaderData& psData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel)
{
    if (psData.shaderPath == "") return;

    HRESULT hr;

    ComPtr<ID3DBlob> pBlob;

    // CSO�t�@�C���ǂݍ���
    hr = LoadShaderFromCSO(psData.shaderPath.c_str(), pBlob.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("�s�N�Z���V�F�[�_�[�ǂݍ��ݎ��s�FShader::CreatePixelShader���Ŕ���", DebugConsole::ERROR_LOG);
        return;
    }

    // �s�N�Z���V�F�[�_�[�𐶐�
    hr = device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, psData.pShader.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("�s�N�Z���V�F�[�_�[�������s�FShader::CreatePixelShader���Ŕ���", DebugConsole::ERROR_LOG);
        return;
    }
}

// �R���s���[�g�V�F�[�_�[�쐬
void Shader::CreateComputeShader(ComputeShaderData& csData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel)
{
    HRESULT hr;

    ComPtr<ID3DBlob> pBlob;

    // CSO�t�@�C���ǂݍ���
    hr = LoadShaderFromCSO(csData.shaderPath.c_str(), pBlob.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("�R���s���[�g�V�F�[�_�[�ǂݍ��ݎ��s�FShader::CreateComputeShader���Ŕ���", DebugConsole::ERROR_LOG);
        return;
    }

    // �R���s���[�g�V�F�[�_�[�𐶐�
    hr = device->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, csData.cShader.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("�R���s���[�g�V�F�[�_�[�������s�FShader::CreateComputeShader���Ŕ���", DebugConsole::ERROR_LOG);
        return;
    }
}

// �W�I���g���V�F�[�_�[�쐬
void Shader::CreateGeometryShader(GeometryShaderData& gsData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel)
{
    HRESULT hr;

    ComPtr<ID3DBlob> pBlob;

    // CSO�t�@�C���ǂݍ���
    hr = LoadShaderFromCSO(gsData.shaderPath.c_str(), pBlob.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("�W�I���g���V�F�[�_�[�ǂݍ��ݎ��s�FShader::CreateGeometryShader���Ŕ���", DebugConsole::ERROR_LOG);
        return;
    }

    // �W�I���g���V�F�[�_�[�𐶐�
    hr = device->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, gsData.gShader.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("�W�I���g���V�F�[�_�[�������s�FShader::CreateGeometryShader���Ŕ���", DebugConsole::ERROR_LOG);
        return;
    }
}

// �V�F�[�_�[�t�@�C���R���p�C��
HRESULT Shader::CompileShaderFromFile(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    // �t�@�C�����p��WCHAR�z�������
    WCHAR	filename[512];
    size_t 	wLen = 0;
    int err = 0;

    // char�i�}���`�o�C�g������j��wchar_t�i���C�h������j�ɕϊ�
    setlocale(LC_ALL, "japanese");
    err = mbstowcs_s(&wLen, filename, 512, szFileName, _TRUNCATE);

    // �V�F�[�_�[�t���O�̐ݒ�
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG_1 ) || defined( _DEBUG )
    // �f�o�b�O�r���h�̏ꍇ�A�f�o�b�O�����܂߂�
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
    // �R���p�C���G���[�p�̃o�b�t�@�i�G���[�����i�[�j
    ComPtr<ID3DBlob> pErrorBlob;

    // �V�F�[�_�[�t�@�C�����R���p�C��
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
        DebugConsole::DebugLog("D3DCompileFromFile ���s�FShader::CompileShaderFromFile���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    return S_OK;
}

// �V�F�[�_�[���R���p�C��
HRESULT Shader::CompileShader(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr;

    // ������
    *ppBlobOut = nullptr;

    // �V�F�[�_�[�̃R���p�C��
    hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, ppBlobOut);
    if (FAILED(hr)) 
    {
        DebugConsole::DebugLog("CompileShaderFromFile ���s�FShader::CompileShader���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    return S_OK;
}

// CSO�t�@�C���ǂݍ���
HRESULT Shader::LoadShaderFromCSO(const char* szFileName, ID3DBlob** ppBlobOut)
{
    // ������
    *ppBlobOut = nullptr;

    // �t�@�C������ wchar �ɕϊ�
    WCHAR filename[512];
    size_t wLen = 0;
    mbstowcs_s(&wLen, filename, szFileName, _TRUNCATE);

    // CSO �t�@�C����ǂݍ���
    return D3DReadFileToBlob(filename, ppBlobOut);
}