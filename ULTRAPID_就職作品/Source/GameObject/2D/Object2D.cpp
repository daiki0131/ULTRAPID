#include "Object2D.h"

using namespace DirectX;

// コンストラクタ
Object2D::Object2D(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:GameObject(_pos, _rot, _scale, _color)
{
}

// 描画処理
void Object2D::Draw()
{
    if (!isActive) return; // 非アクティブ時return

    Renderer::ObjectBuffer ob;
    ob.matrixWorld = XMMatrixTranspose(matrixWorld); // ワールド変換行列セット
    ob.color = color; // カラーをセット
    model.matrixUV = XMMatrixTranspose(GetUvMatrix()); // UV変換行列をセット

    Renderer::DrawObject(model, ob, std::nullopt, useVS, usePS, true, false, false); // 描画
}

// オブジェクトが移動する際の描画処理
// ワールド変換行列を再計算する
void Object2D::MoveDraw()
{
    ReCalculateWorldMatrix();
    this->Draw();
}

// UV変換行列を返す関数
DirectX::XMMATRIX Object2D::GetUvMatrix()
{
    return XMMatrixTranslation(uv.x, uv.y, 0.0f);
}

// シェーダーセット
void Object2D::SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps)
{
    useVS = _vs;
    usePS = _ps;
}

// ワールド変換行列再計算する関数
// （主にオブジェクトが固定ではないとき）
void Object2D::ReCalculateWorldMatrix()
{
    matrixWorld = CalculateMatrixWorld(position, scaling, rotation);
}