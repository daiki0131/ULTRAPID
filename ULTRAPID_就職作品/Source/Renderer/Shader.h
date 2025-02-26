#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include <unordered_map>

using Microsoft::WRL::ComPtr;

//===========================================//
// �V�F�[�_�[�Ɋւ���N���X
//===========================================//
class Shader
{
public:
	// �g�p���钸�_�V�F�[�_�[
	enum VS_NAME
	{
		VS_TEXTURE,                // �e�N�X�`���t���`��
		VS_INSTANCING_TEXTURE,     // �C���X�^���V���O�i�e�N�X�`���t���j
		VS_OBJECT,                 // �e�N�X�`���Ȃ��`��
		VS_INSTANCING,             // �C���X�^���V���O�i�e�N�X�`���Ȃ��j
		VS_ITEM_ANIMATION,         // �A�C�e���A�j���[�V������p
		VS_SPEED_GAUGE,            // ���x�Q�[�W��p
		VS_SHADOW_MAP,             // �V���h�E�}�b�v��p
		VS_INSTANCING_SHADOW_MAP,  // �V���h�E�}�b�v��p�i�C���X�^���V���O�j
		VS_AMBIENT_INSTANCING,     // �����p�i�C���X�^���V���O�j
		VS_TEXTURE_3D,
		VS_NULLPTR,
		VS_MAX,
	};
	// �g�p����s�N�Z���V�F�[�_�[
	enum PS_NAME
	{
		PS_TEXTURE,                // �e�N�X�`���t���`��
		PS_SPECULER_TEXTURE,       // ����t���`��i�e�N�X�`���t���j
		PS_SPECULER,               // ����t���`��i�e�N�X�`���Ȃ��j
		PS_COLOR,                  // �n���ꂽ�F�����̂܂܏o��
		PS_SPEED_GAUGE,            // ���x�Q�[�W��p
		PS_AMBIENT,                // �����i�V���h�E�}�b�v�Ȃ��j
		PS_NULLPTR,
		PS_MAX,
	};
	// �g�p����R���s���[�g�V�F�[�_�[
	enum CS_NAME
	{
		CS_PARTICLE,               // �p�[�e�B�N���̍��W�v�Z
		CS_FRUSTUM,                // �t���X�^���J�����O�p�̌v�Z
		CS_BROAD_PHASE,            // �u���[�h�t�F�[�Y�p�̓����蔻��
		CS_MAX,
	};
	// �g�p����W�I���g���V�F�[�_�[
	enum GS_NAME
	{
		GS_ITEM_ANIMATION,         // �A�C�e���A�j���[�V������p
		GS_MAX,
	};

	// ���_�V�F�[�_�[�f�[�^
	struct VertexShaderData
	{
		ComPtr<ID3D11VertexShader> vShader;
		ComPtr<ID3D11InputLayout> iLayuot;
		std::vector<D3D11_INPUT_ELEMENT_DESC> vDesc;
		std::string shaderPath;
	};

	// �s�N�Z���V�F�[�_�[�f�[�^
	struct PixelShaderData
	{
		ComPtr<ID3D11PixelShader> pShader;
		std::string shaderPath;
	};

	// �R���s���[�g�V�F�[�_�[�f�[�^
	struct ComputeShaderData
	{
		ComPtr<ID3D11ComputeShader> cShader;
		std::string shaderPath;
	};

	// �W�I���g���V�F�[�_�[�f�[�^
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
	// �V�F�[�_�[�쐬
	static void CreateShader();
	// ���_�V�F�[�_�[�쐬
	static void CreateVertexShader(VertexShaderData& vsData,ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel);
	// �s�N�Z���V�F�[�_�[�쐬
	static void CreatePixelShader(PixelShaderData& psData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel);
	// �R���s���[�g�V�F�[�_�[�쐬
	static void CreateComputeShader(ComputeShaderData& csData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel);
	// �W�I���g���V�F�[�_�[�쐬
	static void CreateGeometryShader(GeometryShaderData& gsData, ID3D11Device* device, LPCSTR szEntryPoint, LPCSTR szShaderModel);
	// �V�F�[�_�[�R���p�C��
	static HRESULT CompileShaderFromFile(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	static HRESULT CompileShader(const char* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	static HRESULT LoadShaderFromCSO(const char* szFileName, ID3DBlob** ppBlobOut);
	
	// �e��V�F�[�_�[��Get�֐�
	static ID3D11VertexShader* GetVS(const VS_NAME _vsName) { return vertexShaderMap[_vsName].vShader.Get(); }
	static ID3D11PixelShader* GetPS(const PS_NAME _psName) { return  pixelShaderMap[_psName].pShader.Get(); }
	static ID3D11ComputeShader* GetCS(const CS_NAME _csName) { return  computeShaderMap[_csName].cShader.Get(); }
	static ID3D11GeometryShader* GetGS(const GS_NAME _gsName) { return  geometryShaderMap[_gsName].gShader.Get(); }
	static ID3D11InputLayout* GetLayout(const VS_NAME _vsName) { return  vertexShaderMap[_vsName].iLayuot.Get(); }

};