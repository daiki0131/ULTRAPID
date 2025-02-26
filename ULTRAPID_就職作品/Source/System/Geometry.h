#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl/client.h>
#include "Renderer.h"

using Microsoft::WRL::ComPtr;

//===========================================//
// ���b�V���N���X
//===========================================//
class Geometry
{
public:
	// ���_�f�[�^
	struct VertexData
	{
		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11Buffer> indexBuffer;
		std::vector<Renderer::Vertex> vertices;
		std::vector<UINT> indices;
		int indexCount;
	};

protected:
	static VertexData boxData;           // �{�b�N�X���b�V���̒��_�f�[�^
	static VertexData sphereData;        // �X�t�B�A���b�V���̒��_�f�[�^
	static VertexData spriteCircleData;  // 2D�~�̒��_�f�[�^
	static VertexData slopeData;         // �⓹�̒��_�f�[�^
	static VertexData squareData;        // �l�p�`�̒��_�f�[�^
	static VertexData torusData;

public:
	static void Init();
	static void Release();
	// �e�탁�b�V���쐬
	static void CreateBox();
	static void CreateSphere();
	static void CreateSlope();
	static void CreateSpriteCircle();
	static void CreateSquare();
	static void CreateTorus();
	static Renderer::Model CreateSquare(DirectX::XMFLOAT2 center, DirectX::XMFLOAT2 size, DirectX::XMFLOAT2 uv);
	// �e�탁�b�V���̒��_�f�[�^��Get�֐�
	static VertexData GetBoxVertexData() { return boxData; }
	static VertexData GetSphereVertexData() { return sphereData; }
	static VertexData GetSlopeVertexData() { return slopeData; }
	static VertexData GetSpriteCircleVertexData() { return spriteCircleData; }
	static VertexData GetSquareVertexData() { return squareData; }
	static VertexData GetTorusVertexData() { return torusData; }
};