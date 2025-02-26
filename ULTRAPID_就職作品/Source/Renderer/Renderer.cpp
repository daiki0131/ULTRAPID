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
	flags |= D3D11_CREATE_DEVICE_DEBUG; // �f�o�b�O���[�h�̏ꍇ�A�f�o�b�O�t���O��ݒ�
#endif

    // �g�p����Direct3D�̃o�[�W�������w��
	D3D_FEATURE_LEVEL pLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL level;

    DXGI_SWAP_CHAIN_DESC scDesc; // �X���b�v�`�F�C���̐ݒ�

    // �E�B���h�E�̃N���C�A���g�̈�̃T�C�Y���擾
    ::GetClientRect(hwnd, &rect);

    // �X���b�v�`�F�C���̐ݒ��������
    memset(&scDesc, 0, sizeof(scDesc));
    scDesc.BufferCount = 1;
    scDesc.BufferDesc.Width = rect.Width();
    scDesc.BufferDesc.Height = rect.Height();
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32bit�̃J���[�t�H�[�}�b�g
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // �����_�����O�o�͂Ƃ��Ďg�p
    scDesc.OutputWindow = hwnd;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.Windowed = TRUE; // �E�B���h�E���[�h

    // �f�o�C�X�ƃX���b�v�`�F�C���𓯎��ɍ쐬����֐��̌Ăяo��
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
        DebugConsole::DebugLog("D3D11CreateDeviceAndSwapChain ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }
        

    // �o�b�N�o�b�t�@�̎擾
    ID3D11Texture2D* pBackBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("GetBuffer ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }
       
    // �����_�[�^�[�Q�b�g�r���[���쐬
    hr = device->CreateRenderTargetView(pBackBuffer, NULL, renderTargetView.GetAddressOf());
    pBackBuffer->Release();
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateRenderTargetView ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }
     

    // �[�x�X�e���V���X�e�[�g�̐ݒ�
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // �[�x�o�b�t�@�ɏ������܂Ȃ�
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateDepthStencilState ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
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
        DebugConsole::DebugLog("CreateDepthStencilState ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �[�x�X�e���V���o�b�t�@���쐬
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
        DebugConsole::DebugLog("CreateTexture2D ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �[�x�X�e���V���r���[���쐬
    D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
    memset(&dsDesc, 0, sizeof(dsDesc));
    dsDesc.Format = txDesc.Format;
    dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsDesc.Texture2D.MipSlice = 0;
    hr = device->CreateDepthStencilView(depthStencilTexture.Get(), &dsDesc, depthStencilView.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateDepthStencilView ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �r���[�|�[�g���쐬
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)rect.Width();
    viewport.Height = (FLOAT)rect.Height();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // �T���v���[�쐬
    D3D11_SAMPLER_DESC smpDesc;

    memset(&smpDesc, 0, sizeof(D3D11_SAMPLER_DESC));
    // �K�p�����t�B���^�[�i�A���S���Y���j�̎��
    smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    hr = device->CreateSamplerState(&smpDesc, sampler.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateSamplerState ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    D3D11_SAMPLER_DESC shadowSamplerDesc = {};
    shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    shadowSamplerDesc.BorderColor[0] = 1.0f; // ���F�̋��E
    shadowSamplerDesc.MinLOD = 0.0f;
    shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&shadowSamplerDesc, shadowSampler.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateSamplerState ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }
    // �u�����h�X�e�[�g�쐬
    D3D11_BLEND_DESC BlendState;
    memset(&BlendState, 0, sizeof(D3D11_BLEND_DESC));
    BlendState.AlphaToCoverageEnable = FALSE;
    BlendState.IndependentBlendEnable = FALSE;
    BlendState.RenderTarget[0].BlendEnable = TRUE;
    BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    // �w�i�F�Ɋ|�����킹��W��
    BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = device->CreateBlendState(&BlendState, blendStateAlpha.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateBlendState ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    deviceContext->OMSetBlendState(blendStateAlpha.Get(), NULL, 0xffffffff);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // ���X�^���C�U�X�e�[�g�ݒ�
    D3D11_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    hr = device->CreateRasterizerState(&rasterizerDesc, backCullState.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateRasterizerState ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    hr = device->CreateRasterizerState(&rasterizerDesc, noneCullState.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateRasterizerState ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // ======================================================
    //  �萔�o�b�t�@�쐬
    // ======================================================
  
    // �J�����o�b�t�@
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
        DebugConsole::DebugLog("cameraBuffer �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // ���C�g�o�b�t�@
    cbDesc.ByteWidth = sizeof(LightBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, lightBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("lightBuffer �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �I�u�W�F�N�g�o�b�t�@
    cbDesc.ByteWidth = sizeof(ObjectBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, objectBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("objectBuffer �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // UV�o�b�t�@
    cbDesc.ByteWidth = sizeof(UVBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, uvBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("uvBuffer �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �C���X�^���V���O�o�b�t�@(WordMatrix)
    D3D11_BUFFER_DESC icbDesc;
    icbDesc.ByteWidth = sizeof(ObjectBuffer) * InstancingMaxNum;
    icbDesc.Usage = D3D11_USAGE_DYNAMIC; // ���I�o�b�t�@�̏ꍇ
    icbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �K�{: ���_�o�b�t�@�Ƃ��ăo�C���h�\�ɂ���
    icbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU ���珑�����ޏꍇ
    icbDesc.MiscFlags = 0;
    icbDesc.StructureByteStride = 0;

    hr = device->CreateBuffer(&icbDesc, NULL, instancingObjectBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("instancingMatrixBuffer �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �C���X�^���V���O�o�b�t�@(Color)
    cbDesc.ByteWidth = sizeof(InstancingColorBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, instancingColorBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("instancingColorBuffer �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �}�e���A��
    cbDesc.ByteWidth = sizeof(Material);

    hr = device->CreateBuffer(&cbDesc, NULL, materialBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("materialBuffer �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // ���x�Q�[�W
    cbDesc.ByteWidth = sizeof(GaugeColorBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, gaugeBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("gaugeBuffer �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // GSBuffer
    cbDesc.ByteWidth = sizeof(ShaderAnimationGSBuffer);

    hr = device->CreateBuffer(&cbDesc, NULL, shaderAnimationGSBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("shaderAnimationGSBuffer �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �V���h�E�}�b�v�p�̃e�N�X�`��
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
        DebugConsole::DebugLog("CreateTexture2D ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �V���h�E�}�b�v�p�̐[�x�X�e���V���r���[�̍쐬
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = device->CreateDepthStencilView(shadowMapTexture.Get(), &dsvDesc, shadowDepthStencilView.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateDepthStencilView ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �V�F�[�_�[���\�[�X�r���[�̍쐬
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(shadowMapTexture.Get(), &srvDesc, shadowShaderResourceView.GetAddressOf());

    if (FAILED(hr))
    {
        DebugConsole::DebugLog("CreateShaderResourceView ���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �u���[��p�̃e�N�X�`���쐬
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
        DebugConsole::DebugLog("blurTexture �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �u���[��p�̃����_�[�^�[�Q�b�g�쐬
    D3D11_RENDER_TARGET_VIEW_DESC rtvDescBlur = {};
    rtvDescBlur.Format = blurTextureDesc.Format;
    rtvDescBlur.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDescBlur.Texture2D.MipSlice = 0;

    hr = device->CreateRenderTargetView(blurTexture.Get(), &rtvDescBlur, blurRenderTargetView.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("blurRenderTargetView �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    // �u���[��p�̃V�F�[�_�[���\�[�X�r���[�쐬
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDescBlur = {};
    srvDescBlur.Format = blurTextureDesc.Format;
    srvDescBlur.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDescBlur.Texture2D.MostDetailedMip = 0;
    srvDescBlur.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(blurTexture.Get(), &srvDescBlur, blurShaderResourceView.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("blurShaderResourceView �쐬���s���܂����FRenderer::Init���Ŕ���", DebugConsole::ERROR_LOG);
        return hr;
    }

    instancingObjData.reserve(InstancingMaxNum);

    return hr;
}


void Renderer::ClearScreen()
{
    // �����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V���r���[���Z�b�g
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    // �r���[�|�[�g�̐ݒ�
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>((FLOAT)rect.Width());
    viewport.Height = static_cast<float>((FLOAT)rect.Height());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // �r���[�|�[�g���f�o�C�X�R���e�L�X�g�ɐݒ�
    deviceContext->RSSetViewports(1, &viewport);

    // �o�b�t�@�̃N���A
    float clearColor[] = { 0.1f, 0.1f, 0.3f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), clearColor);

    // �[�x�X�e���V���o�b�t�@���N���A
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
   
}

void Renderer::ClearShadowMapScreen()
{
    deviceContext->OMSetRenderTargets(0, nullptr, shadowDepthStencilView.Get());

    // �r���[�|�[�g�̐ݒ�
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
    // �t���[���o�b�t�@���X���b�v�`�F�C���ɒ� (��ʍX�V)
    swapChain->Present(0, 0);
}

Renderer::Model Renderer::LoadObjModel(const wchar_t* fileName)
{
    //Obj�t�@�C����ǂݍ��߂�悤�ɂ���
    WaveFrontReader<uint32_t> reader;

    HRESULT hr;
    hr = reader.Load(fileName, true);

    // �ǂݍ��ݎ��s������
    if (FAILED(hr))
    {
        DebugConsole::DebugLog(" ���f���ǂݍ��ݎ��s�FRenderer::LoadObjModel�Ŕ���", DebugConsole::ERROR_LOG);
        return Model();
    }

    // reader�̒��ɓǂݍ��܂ꂽ�f�[�^�𗬂����ޔz��
    // ���_�̌����擾����
    const int numVertex = static_cast<int>(reader.vertices.size());
    Vertex* vertexList;
    vertexList = new Vertex[numVertex]; // �z��̓��I����

    // ���_�f�[�^�𗬂�����
    for (int i = 0; i < numVertex; i++)
    {
        vertexList[i].position = reader.vertices[i].position;
        vertexList[i].uv = reader.vertices[i].textureCoordinate;
        // �@���x�N�g�����擾
        vertexList[i].normal = reader.vertices[i].normal;

        // �E����W�n���f����UV�΍�
        vertexList[i].uv.y = 1.0f - vertexList[i].uv.y;

        // �@���x�N�g�����t�Ȃ̂Ŕ��]����
        vertexList[i].normal.x *= -1;
        vertexList[i].normal.y *= -1;
        vertexList[i].normal.z *= -1;
    }

    // ���_�o�b�t�@���쐬����
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(Vertex) * numVertex;  // �m�ۂ���o�b�t�@�T�C�Y���w��
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // ���_�o�b�t�@�쐬���w��
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subResourceData;
    subResourceData.pSysMem = vertexList;  // VRAM�ɑ���f�[�^���w��
    subResourceData.SysMemPitch = 0;
    subResourceData.SysMemSlicePitch = 0;

    Model model = {};
    // ���_�o�b�t�@�쐬���āA�����ϐ��i��R�����j�ɕۑ�
    hr = device->CreateBuffer(&bufferDesc, &subResourceData, model.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog(" vertexBuffer �쐬���s�FRenderer::LoadObjModel�Ŕ���", DebugConsole::ERROR_LOG);
        return Model();
    }

    D3D11_BUFFER_DESC ibDesc;
    ibDesc.ByteWidth = sizeof(UINT) * static_cast<UINT>(reader.indices.size()); // 2�o�C�g�~��
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;
    ibDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA irData;
    irData.pSysMem = reader.indices.data(); // vector.data�֐��Ŕz��f�[�^�擾
    irData.SysMemPitch = 0;
    irData.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&ibDesc, &irData, model.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog(" indexBuffer �쐬���s�FRenderer::LoadObjModel�Ŕ���", DebugConsole::ERROR_LOG);
        return Model();
    }

    // �C���f�b�N�X����ۑ�
    model.numIndex = static_cast<int>(reader.indices.size());

    // ���I���������z������
    delete[] vertexList;

    // �e�N�X�`���ǂݍ���
    // �t�@�C�������擾���ăt�H���_���ƌ�������
    wchar_t textureName[256];
    wcscpy_s(textureName, L"Assets/Texture/");                // ������R�s�[�֐�
    wcscat_s(textureName, reader.materials[1].strTexture);    // �����񌋍��֐�
    model.texture = LoadTexture(textureName);

    return model;
}

ComPtr<ID3D11ShaderResourceView> Renderer::LoadTexture(const wchar_t* fileName)
{
    ComPtr<ID3D11ShaderResourceView> texture;

    if (!fileName)
    {
        DebugConsole::DebugLog(" �t�@�C������ NULL �ł��FRenderer::LoadTexture�Ŕ���", DebugConsole::ERROR_LOG);
        return nullptr;
    }

    HRESULT hr;
    // ��S�����F�ǂݍ��񂾃e�N�X�`����ێ�����ϐ�
    hr = DirectX::CreateWICTextureFromFile(device.Get(), fileName, NULL, texture.GetAddressOf());

    // �ǂݍ��݂Ɏ��s�����ꍇ
    if (FAILED(hr))
    {
        DebugConsole::DebugLog(" �e�N�X�`���ǂݍ��݂Ɏ��s�FRenderer::LoadTexture�Ŕ���", DebugConsole::ERROR_LOG);
    }

    return texture;
}

// �J�����s���ݒ�
void Renderer::SetCameraViewProjection(const DirectX::XMMATRIX& _view, const DirectX::XMMATRIX& _projection)
{
    cameraViewMatrix = _view;
    cameraProjectionMatrix = _projection;
    cameraViewProjection =  XMMatrixTranspose(_view * _projection);
}

// �J�����o�b�t�@��GPU�Z�b�g
void Renderer::SetCameraBuffer()
{
    // �J�����p�̒萔�o�b�t�@
    Renderer::CameraBuffer cb;
    cb.viewProj = cameraViewProjection;

    // �o�b�t�@�X�V
    UpdateBuffer(cameraBuffer.Get(), &cb, sizeof(CameraBuffer));

    // ���_�V�F�[�_�[�ɃZ�b�g
    deviceContext->VSSetConstantBuffers(0, 1, cameraBuffer.GetAddressOf());
}

// ���C�g�o�b�t�@��GPU�Z�b�g
void Renderer::SetLightBuffer()
{
    Renderer::LightBuffer lb;
    lb.viewProj = XMMatrixTranspose(lightViewProjectionMatrix);

    // �o�b�t�@�X�V
    UpdateBuffer(lightBuffer.Get(), &lb, sizeof(LightBuffer));

    // ���_�V�F�[�_�[�ɃZ�b�g
    deviceContext->VSSetConstantBuffers(4, 1, lightBuffer.GetAddressOf());
}

// �}�e���A���o�b�t�@��GPU�Z�b�g
void Renderer::SetMaterialBuffer(const std::optional<Renderer::Material>& _mat)
{
    // �}�e���A�����ݒ肳��Ă���ꍇ
    if (_mat)
    {
        // �o�b�t�@�X�V
        UpdateBuffer(materialBuffer.Get(), &_mat, sizeof(Material));

        // �s�N�Z���V�F�[�_�[�ɃZ�b�g
        deviceContext->PSSetConstantBuffers(0, 1, materialBuffer.GetAddressOf());
    }
}

// �e�N�X�`����GPU�Z�b�g
void Renderer::SetTextureResources(Renderer::Model& _model)
{
    // �e�N�X�`�����ݒ肳��Ă���ꍇ
    if (_model.texture)
    {
        // �s�N�Z���V�F�[�_�Ƀe�N�X�`����ݒ�
        deviceContext->PSSetShaderResources(0, 1, _model.texture.GetAddressOf());

        // �s�N�Z���V�F�[�_�ɃT���v���[��ݒ�
        deviceContext->PSSetSamplers(0, 1, sampler.GetAddressOf());

        // UV�s��f�[�^��萔�o�b�t�@�ɐݒ�
        Renderer::UVBuffer ub;
        ub.matrixUV = _model.matrixUV;

        // �o�b�t�@�X�V
        UpdateBuffer(uvBuffer.Get(), &ub, sizeof(UVBuffer));

        // ���_�V�F�[�_�ɃZ�b�g
        deviceContext->VSSetConstantBuffers(3, 1, uvBuffer.GetAddressOf());
    }
}

void Renderer::SetShadowMapTextureResources(bool _shadowMap)
{
    if (_shadowMap)
    {
        // �s�N�Z���V�F�[�_�Ƀe�N�X�`����ݒ�
        deviceContext->PSSetShaderResources(1, 1, shadowShaderResourceView.GetAddressOf());

        // �s�N�Z���V�F�[�_�ɃT���v���[��ݒ�
        deviceContext->PSSetSamplers(1, 1, shadowSampler.GetAddressOf());
    }
}

// �V�F�[�_�[���Z�b�g
void Renderer::SetupShaderAndPipeline(Shader::VS_NAME _vs, Shader::PS_NAME _ps)
{
    deviceContext->IASetInputLayout(Shader::GetLayout(_vs));
    deviceContext->VSSetShader(Shader::GetVS(_vs), nullptr, 0);

    deviceContext->PSSetShader(Shader::GetPS(_ps), nullptr, 0);
   
}

// �I�u�W�F�N�g�̒ʏ�`��i�P�̕`��j
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
        // �J��������萔�o�b�t�@�ɐݒ�
        SetCameraBuffer();
    }
   
    // ���C�g����萔�o�b�t�@�ɐݒ�
    SetLightBuffer();

    // �}�e���A������萔�o�b�t�@�ɐݒ�
    SetMaterialBuffer(_mat);

    // ���f���̃e�N�X�`�����\�[�X��ݒ�
    SetTextureResources(_model);

    // �o�b�N�J�����O�ݒ�
    SetCullingMode(_backCulling);

    // ���_�V�F�[�_�ƃs�N�Z���V�F�[�_��ݒ�
    SetupShaderAndPipeline(_vs, _ps);

    // �I�u�W�F�N�g�o�b�t�@���X�V
    UpdateBuffer(objectBuffer.Get(), &_ob, sizeof(ObjectBuffer));

    // ���_�V�F�[�_�ɃI�u�W�F�N�g�o�b�t�@��ݒ�
    deviceContext->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());

    // ���_�o�b�t�@��ݒ�
    deviceContext->IASetVertexBuffers(0, 1, _model.vertexBuffer.GetAddressOf(), &strides, &offsets);

    // �C���f�b�N�X�o�b�t�@��ݒ�
    deviceContext->IASetIndexBuffer(_model.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // �C���f�b�N�X�t���`������s
    deviceContext->DrawIndexed(_model.numIndex, 0, 0);
}


// �C���X�^���V���O�`��
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
        // �J��������萔�o�b�t�@�ɐݒ�
        SetCameraBuffer();
    }

    // ���C�g����萔�o�b�t�@�ɐݒ�
    SetLightBuffer();

    // �}�e���A������萔�o�b�t�@�ɐݒ�
    SetMaterialBuffer(_mat);

    // ���f���̃e�N�X�`�����\�[�X��ݒ�
    SetTextureResources(_model);

    SetShadowMapTextureResources(_shadowMap);

    // �o�b�N�J�����O�ݒ�
    SetCullingMode(_backCulling);

    // ���_�V�F�[�_�ƃs�N�Z���V�F�[�_��ݒ�
    SetupShaderAndPipeline(_vs, _ps);

    for (int i = 0; i < _data.size(); i += InstancingMaxNum)
    {
        int instanceCount = std::min(InstancingMaxNum, static_cast<int>(_data.size()) - i);

        // �C���X�^���V���O�p�̃��[���h�ϊ��s��o�b�t�@���X�V
        UpdateBuffer(instancingObjectBuffer.Get(), _data.data() + i, sizeof(ObjectBuffer) * instanceCount);

        UINT strides[2] = { sizeof(Renderer::Vertex), sizeof(ObjectBuffer) };
        UINT offsets[2] = { 0, 0 };
        ID3D11Buffer* buffers[2] = { _model.vertexBuffer.Get(), instancingObjectBuffer.Get() };

        // ���_�o�b�t�@�̐ݒ�
        deviceContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
        deviceContext->IASetIndexBuffer(_model.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        // �C���X�^���V���O���g���ĕ`������s
        deviceContext->DrawIndexedInstanced(_model.numIndex, instanceCount, 0, 0, 0);
    }
}

// �W�I���g���V�F�[�_�[�A�j���[�V����
void Renderer::DrawShaderAnimation(Renderer::Model& _model, Renderer::ObjectBuffer& _ob, Renderer::ShaderAnimationGSBuffer& _gsb, const std::optional<Renderer::Material>& _mat, Shader::VS_NAME _vs, Shader::PS_NAME _ps, Shader::GS_NAME _gs, bool _backCulling)
{
    // �J��������萔�o�b�t�@�ɐݒ�
    SetCameraBuffer();

    // �}�e���A������萔�o�b�t�@�ɐݒ�
    SetMaterialBuffer(_mat);

    // ���f���̃e�N�X�`�����\�[�X��ݒ�
    SetTextureResources(_model);

    // �o�b�N�J�����O�ݒ�
    SetCullingMode(_backCulling);

    // ���_�V�F�[�_�ƃs�N�Z���V�F�[�_��ݒ�
    SetupShaderAndPipeline(_vs, _ps);

    // �I�u�W�F�N�g�o�b�t�@���X�V
    UpdateBuffer(objectBuffer.Get(), &_ob, sizeof(ObjectBuffer));

    // �V�F�[�_�[�A�j���[�V�����p�̃o�b�t�@���X�V
    UpdateBuffer(shaderAnimationGSBuffer.Get(), &_gsb, sizeof(ShaderAnimationGSBuffer));

    // �萔�o�b�t�@���V�F�[�_�ɐݒ�
    deviceContext->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
    deviceContext->GSSetConstantBuffers(0, 1, shaderAnimationGSBuffer.GetAddressOf());
    deviceContext->GSSetConstantBuffers(1, 1, cameraBuffer.GetAddressOf());
   
    // �W�I���g���V�F�[�_��ݒ�
    deviceContext->GSSetShader(Shader::GetGS(_gs), NULL, 0);

    // ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@��ݒ�
    deviceContext->IASetVertexBuffers(0, 1, _model.vertexBuffer.GetAddressOf(), &strides, &offsets);
    deviceContext->IASetIndexBuffer(_model.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // �C���f�b�N�X�t���`������s
    deviceContext->DrawIndexed(_model.numIndex, 0, 0);

    // �W�I���g���V�F�[�_�𖳌���
    deviceContext->GSSetShader(NULL, NULL, 0);
}

// ���x�Q�[�W��p�̕`��
void Renderer::DrawGaugeObject(Renderer::Model& _model, Renderer::ObjectBuffer& _ob, Renderer::GaugeColorBuffer& _cgb, Shader::VS_NAME _vs, Shader::PS_NAME _ps, bool _backCulling)
{
    // �J��������萔�o�b�t�@�ɐݒ�
    SetCameraBuffer();

    // �o�b�N�J�����O�̐ݒ�
    SetCullingMode(_backCulling);

    // ���_�V�F�[�_�ƃs�N�Z���V�F�[�_��ݒ�
    SetupShaderAndPipeline(_vs, _ps);

    // �I�u�W�F�N�g�o�b�t�@���X�V���A�萔�o�b�t�@�ɑ��M
    UpdateBuffer(objectBuffer.Get(), &_ob, sizeof(ObjectBuffer));

    // UV���W�p�̃o�b�t�@���X�V���A�萔�o�b�t�@�ɑ��M
    Renderer::UVBuffer ub;
    ub.matrixUV = _model.matrixUV;
    UpdateBuffer(uvBuffer.Get(), &ub, sizeof(UVBuffer));

    // �Q�[�W�F�p�̃o�b�t�@���X�V���A�萔�o�b�t�@�ɑ��M
    UpdateBuffer(gaugeBuffer.Get(), &_cgb, sizeof(GaugeColorBuffer));
  
    // �e�V�F�[�_�̒萔�o�b�t�@��ݒ�
    deviceContext->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
    deviceContext->VSSetConstantBuffers(2, 1, uvBuffer.GetAddressOf());
    deviceContext->PSSetConstantBuffers(0, 1, gaugeBuffer.GetAddressOf());

    // ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@��ݒ�
    deviceContext->IASetVertexBuffers(0, 1, _model.vertexBuffer.GetAddressOf(), &strides, &offsets);
    deviceContext->IASetIndexBuffer(_model.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // �C���f�b�N�X�t���`������s
    deviceContext->DrawIndexed(_model.numIndex, 0, 0);
}

// �o�b�t�@���X�V����
void Renderer::UpdateBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    // �o�b�t�@���}�b�v
    HRESULT hr = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) 
    {
        DebugConsole::DebugLog(" �o�b�t�@�X�V���s�FRenderer::UpdateBuffer�Ŕ���", DebugConsole::ERROR_LOG);
    }

    // �f�[�^���R�s�[
    memcpy(mappedResource.pData, data, dataSize);

    // �}�b�v����
    deviceContext->Unmap(buffer, 0);
}

// �J�����O���[�h��ݒ肷��
void Renderer::SetCullingMode(bool _backCulling)
{
    ID3D11RasterizerState* cullState = _backCulling ? backCullState.Get() : noneCullState.Get();
    deviceContext->RSSetState(cullState);
}