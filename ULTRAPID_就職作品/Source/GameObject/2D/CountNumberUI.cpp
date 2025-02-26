#include "CountNumberUI.h"
#include "Geometry.h"

using namespace DirectX;

// コンストラクタ
CountNumberUI::CountNumberUI()
{
	// UIオブジェクト生成
	numUI = std::make_unique<Object2D>(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.4f, 0.7f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	// シェーダーセット
	numUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	// モデルセット
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/number.png");
	numUI->SetModel(model);
}

// 描画処理
void CountNumberUI::Draw()
{
	if (!numUI->GetIsActive()) return; // 非アクティブ状態の時return

	numUI->Draw(); // 描画実行
}

// 数字をセット
void CountNumberUI::SetNum(int _num)
{
	int num = std::abs(_num) % 10; // 1の位を計算

	SetUV(num); // UV計算

	numUI->SetUV(uv); // UVセット
}