#include "TimeUI.h"
#include "Geometry.h"
#include "DebugConsole.h"

using namespace DirectX;

TimeUI::TimeUI()
{
	std::vector<XMFLOAT3> positions =
	{
		XMFLOAT3(0.55f, -0.9f, -0.21f),    // ミリ秒（一番右の値）
		XMFLOAT3(0.79f, -0.9f, -0.21f),    // ミリ秒（右から2番目の値）
		XMFLOAT3(1.10f, -0.9f, -0.21f),    // 秒（右から3番目の値）
		XMFLOAT3(1.34f, -0.9f, -0.21f),    // 秒（右から4番目の値）
		XMFLOAT3(1.63f, -0.955f, -0.21f),  // 分（右から5番目の値）
		XMFLOAT3(1.785f, -0.955f, -0.21f), // 分（右から6番目の値）
		XMFLOAT3(0.96f, -1.035f, -0.21f),  // ドット1（分と秒の間）
		XMFLOAT3(1.5f, -1.038f, -0.21f),   // ドット2（秒とミリ秒の間）
	};

	std::vector<XMFLOAT3> scales =
	{
		XMFLOAT3(0.3f, 0.45f, 1.0f),      // ミリ秒（一番右の値）
		XMFLOAT3(0.3f, 0.45f, 1.0f),      // ミリ秒（右から2番目の値）
		XMFLOAT3(0.3f, 0.45f, 1.0f),      // 秒（右から3番目の値）
		XMFLOAT3(0.3f, 0.45f, 1.0f),      // 秒（右から4番目の値）
		XMFLOAT3(0.2f, 0.3f, 1.0f),       // 分（右から5番目の値）
		XMFLOAT3(0.2f, 0.3f, 1.0f),       // 分（右から6番目の値）
		XMFLOAT3(0.17f, 0.21f, 1.0f),     // ドット1（分と秒の間）
		XMFLOAT3(0.14f, 0.173f, 1.0f),    // ドット2（秒とミリ秒の間）
	};

	timeNums.resize(8);

	// オブジェクト生成・シェーダーセット
	for (int i = 0; i < timeNums.size(); i++)
	{
		timeNums[i] = std::make_unique<Object2D>(
			positions[i],
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			scales[i],
			XMFLOAT4(1, 1, 1, 1));

		timeNums[i]->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	}

	// モデルセット
	// 数字UI
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/number.png");
	for (int i = 0; i < timeNums.size() -2; i++)
	{
		timeNums[i]->SetModel(model);
	}
	// ドットUI
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1.0f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/dot.png");
	timeNums[6]->SetModel(model);
	timeNums[7]->SetModel(model);

	// 各UIのタイプを設定
	timePlace[0] = MINUTE_DIGIT2;
	timePlace[1] = MINUTE_DIGIT1;
	timePlace[2] = SECOND_DIGIT2;
	timePlace[3] = SECOND_DIGIT1;
	timePlace[4] = MILLISECONND_DIGIT2;
	timePlace[5] = MILLISECONND_DIGIT1;
	timePlace[6] = OTHER;
	timePlace[7] = OTHER;

	// ワールド変換行列セット
	SetWorldMatrix(timeNums);
}

void TimeUI::Update(const float _time)
{
	if (_time < 0.0f) return;

	// 指定の秒数以下になると色を変更する
	if (_time < ChangeColorSecond)
	{
		for (auto& ui : timeNums)
		{
			ui->SetRGBColor(XMFLOAT3(1, 0, 0)); // 赤に変える
		}
	}

	// 分成分を抜き取る
	const unsigned int minute = (int)_time / 60;
	// 秒成分を抜き取る
	const unsigned int second = (int)_time % 60;
	// ミリ秒成分を抜き取る
	const unsigned int milliSecond = (int)((_time - (int)_time) * 100);

	for (int i = 0; i < timeNums.size(); i++)
	{
		// 最初に半透明状態にする
		// 使用状態になれば後からα値を1.0にする
		timeNums[i]->SetColorAlpha(NumberTexture2D::UiLowAlpha);
		auto it = timePlace.find(i); // 数字のタイプを取得

		if (it == timePlace.end())
		{
			DebugConsole::DebugLog("タイムUIのタイプが設定されてません：TimeUI::Update内で発生", DebugConsole::WARNING_LOG);
			timePlace[i] = SECOND_DIGIT1;
			continue;
		}

		switch (it->second)
		{
		case TimeUI::MINUTE_DIGIT1:       // 分1桁目
			SetUV(minute % 10);
			if (minute > 0)
			{
				timeNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case TimeUI::MINUTE_DIGIT2:       // 分2桁目
			SetUV(minute / 10);
			if (minute >= 10)
			{
				timeNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case TimeUI::SECOND_DIGIT1:       // 秒1桁目
			SetUV(second % 10);
			timeNums[i]->SetColorAlpha(1.0f);
			break;
		case TimeUI::SECOND_DIGIT2:       // 秒2桁目
			SetUV(second / 10);
			if (second >= 10 || minute > 0)
			{
				timeNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case TimeUI::MILLISECONND_DIGIT1: // ミリ秒1桁目
			SetUV(milliSecond % 10);
			timeNums[i]->SetColorAlpha(1.0f);
			break;
		case TimeUI::MILLISECONND_DIGIT2: // ミリ秒1桁目
			SetUV(milliSecond / 10);
			timeNums[i]->SetColorAlpha(1.0f);
			break;
		case TimeUI::OTHER:               // ドット
			SetUV(10);
			timeNums[i]->SetColorAlpha(1.0f);
			break;
		default:
			break;
		}
		// UVセット
		timeNums[i]->SetUV(uv);
	}
}

// 描画処理
void TimeUI::Draw()
{
	for (auto& ui : timeNums)
	{
		ui->Draw();
	}
}

// 親座標の更新
void TimeUI::SetParentPosition(DirectX::XMFLOAT3 _pos)
{
	parentPosition = _pos;
	SetWorldMatrix(timeNums);
}

// 親スケールの更新
void TimeUI::SetParentScale(DirectX::XMFLOAT3 _scale)
{
	parentScale = _scale;
	SetWorldMatrix(timeNums);
}

// アクティブ状態を一括切り替え
void TimeUI::SetActive(bool _active)
{
	for (auto& ui : timeNums)
	{
		ui->SetIsActive(_active);
	}
}