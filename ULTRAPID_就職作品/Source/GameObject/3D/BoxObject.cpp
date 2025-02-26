#include "BoxObject.h"
#include "Geometry.h"

using namespace DirectX;

// コンストラクタ
BoxObject::BoxObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:GameObject(_pos, _rot, _scale, _color)
{
}

void BoxObject::Draw()
{
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = matrixWorld; // ワールド変換行列セット
	ob.color = color; // カラーをセット
	Renderer::Model model;
	Geometry::VertexData vd = Geometry::GetBoxVertexData();
	model.indexBuffer = vd.indexBuffer;
	model.vertexBuffer = vd.vertexBuffer;
	model.numIndex = vd.indexCount;
	Renderer::Material mat;
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 10.0f);
	mat.other = XMFLOAT4(1.25f, 0.0f, 0.0f, 0.0f);

	Renderer::DrawObject(model, ob, mat, Shader::VS_OBJECT, Shader::PS_SPECULER, false, false, true); // 描画
}

float BoxObject::DistanceFromCamera(const XMFLOAT3& cameraPosition)
{
	XMVECTOR objPos = XMLoadFloat3(&position);
	XMVECTOR camPos = XMLoadFloat3(&cameraPosition);
	XMVECTOR diff = XMVectorSubtract(objPos, camPos);
	return XMVectorGetX(XMVector3Length(diff));
}

// バウンディングボックスを設定する
void BoxObject::SetBoundingBox(DirectX::XMFLOAT3 _center, DirectX::XMFLOAT3 _extents)
{
	boundingBox.center = _center;
	boundingBox.extents = DirectX::XMFLOAT3(_extents.x * 0.5f, _extents.y * 0.5f, _extents.z * 0.5f);

	float maxLength = sqrt(_extents.x * _extents.x + _extents.y * _extents.y + _extents.z * _extents.z);
	float sphereRadius = maxLength * 0.505f;

	// バウンディングボックスの情報からバウンディングスフィアを設定
	SetBoundingSpehre(_center, sphereRadius);
}

// バウンディングスフィアを設定する
void BoxObject::SetBoundingSpehre(DirectX::XMFLOAT3 _center, float _radius)
{
	boundingSphere.center = _center;
	boundingSphere.radius = _radius;
}