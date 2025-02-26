#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl/client.h>
#include "Renderer.h"

using Microsoft::WRL::ComPtr;

//===========================================//
// メッシュクラス
//===========================================//
class Geometry
{
public:
	// 頂点データ
	struct VertexData
	{
		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11Buffer> indexBuffer;
		std::vector<Renderer::Vertex> vertices;
		std::vector<UINT> indices;
		int indexCount;
	};

protected:
	static VertexData boxData;           // ボックスメッシュの頂点データ
	static VertexData sphereData;        // スフィアメッシュの頂点データ
	static VertexData spriteCircleData;  // 2D円の頂点データ
	static VertexData slopeData;         // 坂道の頂点データ
	static VertexData squareData;        // 四角形の頂点データ
	static VertexData torusData;

public:
	static void Init();
	static void Release();
	// 各種メッシュ作成
	static void CreateBox();
	static void CreateSphere();
	static void CreateSlope();
	static void CreateSpriteCircle();
	static void CreateSquare();
	static void CreateTorus();
	static Renderer::Model CreateSquare(DirectX::XMFLOAT2 center, DirectX::XMFLOAT2 size, DirectX::XMFLOAT2 uv);
	// 各種メッシュの頂点データのGet関数
	static VertexData GetBoxVertexData() { return boxData; }
	static VertexData GetSphereVertexData() { return sphereData; }
	static VertexData GetSlopeVertexData() { return slopeData; }
	static VertexData GetSpriteCircleVertexData() { return spriteCircleData; }
	static VertexData GetSquareVertexData() { return squareData; }
	static VertexData GetTorusVertexData() { return torusData; }
};