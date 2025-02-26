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
// �`��S�ʂɊւ���N���X
//===========================================//
class Renderer
{
public:
	static constexpr int InstancingMaxNum = 10000; // ��x�̃C���X�^���V���O��GPU�ւ̑��M��
	static constexpr float ShadowMapSize = 2048.0f;     // �V���h�E�}�b�v�̉𑜓x

public:
	// �F���
	struct Color
	{
		float r;
		float g;
		float b;
		float a;
	};

	// ���f�����
	struct Model
	{
		ComPtr<ID3D11Buffer> vertexBuffer;         // ���_�o�b�t�@
		ComPtr<ID3D11ShaderResourceView> texture;  // �e�N�X�`��
		ComPtr<ID3D11Buffer> indexBuffer;          // �C���f�b�N�X�o�b�t�@
		int numIndex;                              // �C���f�b�N�X��
		DirectX::XMMATRIX matrixUV;                // �e�N�X�`��UV�s��

		Model() : 
			vertexBuffer(nullptr),
			texture(nullptr), 
			indexBuffer(nullptr),
			numIndex(0), 
			matrixUV(DirectX::XMMatrixIdentity())
		{
		}
	};

	//���_�f�[�^
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
	};

	//===========================================//
	// �萔�o�b�t�@��`
	//===========================================//

	// �J�����o�b�t�@
	struct CameraBuffer
	{
		DirectX::XMMATRIX viewProj;
	};

	// ���C�g�o�b�t�@
	struct LightBuffer
	{
		DirectX::XMMATRIX viewProj;
	};
	
	// �I�u�W�F�N�g�o�b�t�@
	struct ObjectBuffer
	{
		DirectX::XMMATRIX matrixWorld;
		DirectX::XMFLOAT4 color;
	};

	// �C���X�^���V���O�o�b�t�@�i�J���[�j
	struct InstancingColorBuffer
	{
		DirectX::XMFLOAT4 color[InstancingMaxNum];
	};

	// UV�s��o�b�t�@
	struct UVBuffer
	{
		DirectX::XMMATRIX matrixUV;
	};

	// �}�e���A��
	struct Material
	{
		DirectX::XMFLOAT4 emission;  // emissionColor (xyz) + emissionStrength (w)
		DirectX::XMFLOAT4 specular;  // specularColor (xyz) + specularStrength (w)
		DirectX::XMFLOAT4 ambient;   // ambientColor (xyz) + ambientStrength (w)
		DirectX::XMFLOAT4 other;     // chromaticAberrationStrength (x) + �p�f�B���O (yzw)
	};

	// �W�I���g���V�F�[�_�[�A�j���[�V�����o�b�t�@
	struct ShaderAnimationGSBuffer
	{
		float elapsedTime;           // �o�ߎ���
		DirectX::XMFLOAT3 center;    // ���S���W
	};

	// ���x�Q�[�W��p�̃o�b�t�@
	struct GaugeColorBuffer
	{
		DirectX::XMFLOAT4 color[4];   // �����珇�̐F
		DirectX::XMFLOAT4 thresholds; // �����珇�̋��E�l
		float speedRatio;             // ���݂̑��x����
		float lowAlphaValue;          // ���������̃��l
		float padding[2];
	};
	
private:
	static ComPtr<ID3D11Device> device;                       // Direct3D 11 �f�o�C�X
	static ComPtr<ID3D11DeviceContext> deviceContext;         // �f�o�C�X�R���e�L�X�g (�`��R�}���h�̔��s�Ɏg�p)
	static ComPtr<IDXGISwapChain> swapChain;                  // �X���b�v�`�F�[�� (�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̊Ǘ�)
	static ComPtr<ID3D11RenderTargetView> renderTargetView;   // �����_�[�^�[�Q�b�g�r���[ (�`�挋�ʂ̏o�͐�)
	static ComPtr<ID3D11Texture2D> depthStencilTexture;       // �[�x�X�e���V���p�̃e�N�X�`�� (Z�o�b�t�@)
	static ComPtr<ID3D11DepthStencilView> depthStencilView;   // �[�x�X�e���V���r���[ (Z�o�b�t�@�̃r���[)
	static ComPtr<ID3D11DepthStencilState> depthStencilState; // �[�x�X�e���V���X�e�[�g (�[�x�o�b�t�@�̔�r���@�̐ݒ�)
	static D3D11_VIEWPORT viewport;                           // �r���[�|�[�g (�`��̈�̐ݒ�)
	static ComPtr<ID3D11BlendState> blendStateAlpha;          // �u�����h�X�e�[�g (���u�����h�p)
	static ComPtr<ID3D11BlendState> blendStateAdditive;       // �u�����h�X�e�[�g (���Z�����p)
	static ComPtr<ID3D11SamplerState> sampler;                // �T���v���[�X�e�[�g (�e�N�X�`���̃T���v�����O�ݒ�)
	static ComPtr<ID3D11RasterizerState> noneCullState;       // ���X�^���C�U�[�X�e�[�g (�J�����O�Ȃ�)
	static ComPtr<ID3D11RasterizerState> backCullState;    	  // ���X�^���C�U�[�X�e�[�g (���ʃJ�����O)
	static UINT strides;                                      // ���_�o�b�t�@�̃X�g���C�h (1���_������̃o�C�g��)
	static UINT offsets;                                      // ���_�o�b�t�@�̃I�t�Z�b�g
	static CRect rect;                                        // �`��̈�̋�`
	static ComPtr<ID3D11Buffer> cameraBuffer;                 // �J�����p�萔�o�b�t�@
	static ComPtr<ID3D11Buffer> lightBuffer;                  // ���C�g�p�萔�o�b�t�@
	static ComPtr<ID3D11Buffer> objectBuffer;	              // �I�u�W�F�N�g�p�萔�o�b�t�@
	static ComPtr<ID3D11Buffer> uvBuffer;                     // UV���W�p�o�b�t�@
	static ComPtr<ID3D11Buffer> instancingObjectBuffer;       // �C���X�^���V���O�p�̃��[���h�s��o�b�t�@
	static ComPtr<ID3D11Buffer> instancingColorBuffer;        // �C���X�^���V���O�p�̃J���[���o�b�t�@
	static ComPtr<ID3D11Buffer> materialBuffer;               // �}�e���A�������i�[����o�b�t�@
	static ComPtr<ID3D11Buffer> gaugeBuffer;                  // ���x�Q�[�W�o�b�t�@
	static ComPtr<ID3D11Buffer> shaderAnimationGSBuffer;      // �V�F�[�_�[�̃A�j���[�V�����p�W�I���g���V�F�[�_�[�o�b�t�@
	static DirectX::XMMATRIX cameraViewMatrix;                // �J�����̃r���[�s�� 
	static DirectX::XMMATRIX cameraProjectionMatrix;          // �J�����̃v���W�F�N�V�����s�� 
	static DirectX::XMMATRIX cameraViewProjection;	          // �J�����̃r���[�E�v���W�F�N�V�����s��
	static DirectX::XMFLOAT3 cameraPos;                       // �J�����̃��[���h���W
	static DirectX::XMMATRIX lightViewProjectionMatrix;       // ���C�g�̃r���[�E�v���W�F�N�V�����s��
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
	// ������
	static HRESULT Init(HWND hwnd);
	// �X�N���[�����Z�b�g
	static void ClearScreen();
	static void ClearShadowMapScreen();
	// �X�N���[���X�V
	static void UpdateScreen();
	// �I�u�W�F�N�g�t�@�C���ǂݍ���
	static Model LoadObjModel(const wchar_t* fileName);
	// �e�N�X�`���ǂݍ���
	static ComPtr<ID3D11ShaderResourceView> LoadTexture(const wchar_t* fileName);
	// Set�֐�
	static void SetCameraViewProjection(const DirectX::XMMATRIX& _view, const DirectX::XMMATRIX& _projection);
	static void SetLightViewProjection(const DirectX::XMMATRIX& _viewProjection) { lightViewProjectionMatrix = _viewProjection; };
	// Get�֐�
	static ComPtr<ID3D11Device> GetDevice() { return device; }
	static ComPtr<ID3D11DeviceContext> GetContext() { return deviceContext; }
	static ComPtr<ID3D11DepthStencilState> GetDepthStencilState() { return depthStencilState; }
	static ComPtr<ID3D11DepthStencilState> GetShadowDepthStencilState() { return shadowDepthStencilState; }
	static DirectX::XMMATRIX GetViewMatrix() { return cameraViewMatrix; }
	// �ʏ�`��
	static void DrawObject(
		Renderer::Model& _model,
		Renderer::ObjectBuffer& _ob,
		const std::optional<Renderer::Material>& _mat,
		Shader::VS_NAME _vs, Shader::PS_NAME _ps,
		bool _backCulling,
		bool _writeDepth,
		bool _shadowMap);
	// �C���X�^���V���O�`��
	static void DrawObjectInstancing(
		Renderer::Model& _model, 
		const std::vector<Renderer::ObjectBuffer>& _data,
		const std::optional<Renderer::Material>& _mat, 
		Shader::VS_NAME _vs, Shader::PS_NAME _ps, 
		bool _backCulling,
		bool _writeDepth,
		bool _shadowMap);
	//�@�W�I���g���V�F�[�_�[�A�j���[�V�����`��
	static void DrawShaderAnimation(
		Renderer::Model& _model, 
		Renderer::ObjectBuffer& _ob,
		Renderer::ShaderAnimationGSBuffer& _gsb,
		const std::optional<Renderer::Material>& _mat, 
		Shader::VS_NAME _vs,
		Shader::PS_NAME _ps,
		Shader::GS_NAME _gs, 
		bool _backCulling);
	// ���x�Q�[�W��p�`��
	static void DrawGaugeObject(
		Renderer::Model& _model,
		Renderer::ObjectBuffer& _ob,
		Renderer::GaugeColorBuffer& _cgb,
		Shader::VS_NAME _vs, Shader::PS_NAME _ps,
		bool _backCulling);
	// GPU���M�o�b�t�@�X�V
	static void UpdateBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize);
	// �J�����O���[�h�ؑ�
	static void SetCullingMode(bool _backCulling);
	// �J������GPU�ɃZ�b�g
	static void SetCameraBuffer();
	// ���C�g��GPU�ɃZ�b�g
	static void SetLightBuffer();
	// �}�e���A����GPU�ɃZ�b�g
	static void SetMaterialBuffer(const std::optional<Renderer::Material>& _mat);
	// �e�N�X�`�����\�[�X��GPU�ɃZ�b�g
	static void SetTextureResources(Renderer::Model& _model);
	static void SetShadowMapTextureResources(bool _shadowMap);
	// �V�F�[�_�[��GPU�ɃZ�b�g
	static void SetupShaderAndPipeline(Shader::VS_NAME _vs, Shader::PS_NAME _ps);

};