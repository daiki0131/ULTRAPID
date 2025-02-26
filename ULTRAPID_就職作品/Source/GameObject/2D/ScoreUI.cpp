#include "ScoreUI.h"
#include "Geometry.h"
#include "DebugConsole.h"

using namespace DirectX;

ScoreUI::ScoreUI()
{
	std::vector<XMFLOAT3> positions =
	{
		XMFLOAT3(0.55f, -0.9f, -0.21f),  // 10000の位
		XMFLOAT3(0.79f, -0.9f, -0.21f),  // 1000の位
		XMFLOAT3(1.03f, -0.9f, -0.21f),  // 100の位
		XMFLOAT3(1.27f, -0.9f, -0.21f),  // 10の位
		XMFLOAT3(1.51f, -0.9f, -0.21f),  // 1の位
	};

	// モデル
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/number.png");

	scores.resize(5);

	// 各数字UIの生成
	for (int i = 0; i < scores.size(); i++)
	{
		scores[i] = std::make_unique<Object2D>(
			positions[i],
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(0.3f, 0.45f, 1.0f),
			XMFLOAT4(1, 1, 1, 1));
		// シェーダーセット
		scores[i]->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
		// モデルセット
		scores[i]->SetModel(model);
	}

	// 各UIの桁のタイプ設定
	scorePlace[0] = PLACE_10000;
	scorePlace[1] = PLACE_1000;
	scorePlace[2] = PLACE_100;
	scorePlace[3] = PLACE_10;
	scorePlace[4] = PLACE_1;

	// ワールド変換行列セット
	SetWorldMatrix(scores);
}

void ScoreUI::Update(const int _score)
{
	// スコアに基づいて各桁の数字を取り出してUV値を設定
	for (int i = 0; i < scores.size(); i++)
	{
		// 最初に半透明状態にする
		// 使用状態になれば後からα値を1.0にする
		scores[i]->SetColorAlpha(NumberTexture2D::UiLowAlpha);
		auto it = scorePlace.find(i); // 数字のタイプを取得

		if (it == scorePlace.end())
		{
			DebugConsole::DebugLog("スコアのタイプが設定されてません：ScoreUI::Update内で発生", DebugConsole::WARNING_LOG);
			scorePlace[i] = PLACE_1;
		}
	
		switch (it->second)
		{
		case PLACE_1:       // 1の位
			SetUV(_score % 10);
			if (_score >= 0)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		case PLACE_10:      // 10の位
			SetUV((_score / 10) % 10);
			if (_score >= 10)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		case PLACE_100:     // 100の位
			SetUV((_score / 100) % 10);
			if (_score >= 100)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		case PLACE_1000:    // 1000の位
			SetUV((_score / 1000) % 10);
			if (_score >= 1000)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		case PLACE_10000:   // 10000の位
			SetUV((_score / 10000) % 10);
			if (_score >= 10000)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		default:
			break;
		}
		// UVセット
		scores[i]->SetUV(uv);
	}
}

// 描画処理
void ScoreUI::Draw()
{
	for (auto& ui : scores)
	{
		ui->Draw();
	}
}

// 親座標の更新
void ScoreUI::SetParentPosition(DirectX::XMFLOAT3 _pos)
{
	parentPosition = _pos;
	SetWorldMatrix(scores);
}

// 親スケールの更新
void ScoreUI::SetParentScale(DirectX::XMFLOAT3 _scale)
{
	parentScale = _scale;
	SetWorldMatrix(scores);
}

// アクティブ状態を一括切り替え
void ScoreUI::SetActive(bool _active)
{
	for (auto& ui : scores)
	{
		ui->SetIsActive(_active);
	}
}