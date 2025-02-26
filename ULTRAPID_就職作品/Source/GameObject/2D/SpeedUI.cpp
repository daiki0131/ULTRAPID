#include "SpeedUI.h"
#include "Geometry.h"
#include "DebugConsole.h"

using namespace DirectX;

SpeedUI::SpeedUI()
{
	std::vector<XMFLOAT3> positions =
	{
		XMFLOAT3(0.275f, -1.08f, 0.0f), // 数字UI1
		XMFLOAT3(0.6f, -1.08f, 0.0f),   // 数字UI2
		XMFLOAT3(0.925f, -1.08f, 0.0f), // 数字UI3
		XMFLOAT3(1.25f, -1.08f, 0.0f),  // 数字UI4
		//XMFLOAT3(1.55f, -1.17f, 0.0f),  // 数字UI5
		//XMFLOAT3(1.73f, -1.17f, 0.0f),  // 数字UI6
		//XMFLOAT3(1.44f, -1.28f, 0.0f),  // ドット
		
	};

	std::vector<XMFLOAT3> scales =
	{
		XMFLOAT3(0.45f, 0.675f, 1.0f),    // 数字UI1
		XMFLOAT3(0.45f, 0.675f, 1.0f),    // 数字UI2
		XMFLOAT3(0.45f, 0.675f, 1.0f),    // 数字UI3
		XMFLOAT3(0.45f, 0.675f, 1.0f),    // 数字UI4
		//XMFLOAT3(0.2625f, 0.39375f, 1.0f),// 数字UI5
		//XMFLOAT3(0.2625f, 0.39375f, 1.0f),// 数字UI6
		//XMFLOAT3(0.17f, 0.21f, 1.0f),     // ドット
		
	};

	speedNums.resize(4);

	// オブジェクト生成・シェーダーセット
	for (int i = 0; i < speedNums.size(); i++)
	{
		speedNums[i] = std::make_unique<Object2D>(
			positions[i],
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			scales[i],
			XMFLOAT4(1, 1, 1, 1));

		speedNums[i]->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	}

	// モデルセット
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/number.png");
	// 数字UI
	for (int i = 0; i < speedNums.size(); i++)
	{
		speedNums[i]->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
		speedNums[i]->SetModel(model);
	}
	//// ドット
	//model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1.0f, 1.0f });
	//model.texture = Renderer::LoadTexture(L"Assets/Texture/dot.png");
	//speedNums[6]->SetModel(model);
	

	// 各UIの数字タイプをセット
	speedPlace[0] = PLACE_1000;
	speedPlace[1] = PLACE_100;       // 100の位
	speedPlace[2] = PLACE_10;        // 10の位
	speedPlace[3] = PLACE_1;         // 1の位
	//speedPlace[4] = MILLI_PLACE_10;  // 小数第1位
	//speedPlace[5] = MILLI_PLACE_1;   // 小数第2位
	//speedPlace[6] = OTHER;           // ドット
	

	// ワールド変換行列セット
	SetWorldMatrix(speedNums);
}

// 更新処理
void SpeedUI::Update(const float _speed)
{
	float speedConversionFactor = MaxSpeedScaledValue / maxSpeed;
	float speed = _speed * speedConversionFactor;

	if (speed > MaxSpeedScaledValue) speed = MaxSpeedScaledValue;

	const unsigned int num = (int)speed; // 整数部分切り取り
	const unsigned int milliNum = (int)((speed - (int)speed) * 100); // 小数部分切り取り

	// 切り分けた数字に基づいてUV値を決定する
	for (int i = 0; i < speedNums.size(); i++)
	{
		// 最初に半透明状態にする
		// 使用状態になれば後からα値を1.0にする
		speedNums[i]->SetColorAlpha(NumberTexture2D::UiLowAlpha);
		auto it = speedPlace.find(i); // 数字のタイプを取得

		if (it == speedPlace.end())
		{
			DebugConsole::DebugLog("速度UIのタイプが設定されてません：SpeedUI::Update内で発生", DebugConsole::WARNING_LOG);
			speedPlace[i] = PLACE_1;
		}

		switch (it->second)
		{
		case SpeedUI::PLACE_1:   // 1の位
			SetUV(num % 10);
			speedNums[i]->SetColorAlpha(1.0f);
			break;
		case SpeedUI::PLACE_10:  //10の位
			SetUV((num / 10) % 10);
			if (num >= 10)
			{
				speedNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case SpeedUI::PLACE_100: // 100の位
			SetUV((num / 100) % 10);
			if (num >= 100)
			{
				speedNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case SpeedUI::PLACE_1000: // 1000の位
			SetUV((num / 1000) % 10);
			if (num >= 1000)
			{
				speedNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case SpeedUI::MILLI_PLACE_1:  // 小数第1位
			SetUV(milliNum % 10);
			speedNums[i]->SetColorAlpha(1.0f);
			break;
		case SpeedUI::MILLI_PLACE_10: // 小数第2位
			SetUV(milliNum / 10);
			speedNums[i]->SetColorAlpha(1.0f);
			break;
		case SpeedUI::OTHER:          // ドット・km/h
			SetUV(10);
			speedNums[i]->SetColorAlpha(1.0f);
			break;
		default:
			break;
		}
		// UVセット
		speedNums[i]->SetUV(uv);
	}
}

// 描画処理
void SpeedUI::Draw()
{
	for (auto& ui : speedNums)
	{
		ui->Draw();
	}
}

// 親座標の更新
void SpeedUI::SetParentPosition(DirectX::XMFLOAT3 _pos)
{
	parentPosition = _pos;
	SetWorldMatrix(speedNums);
}

// 親スケールの更新
void SpeedUI::SetParentScale(DirectX::XMFLOAT3 _scale)
{
	parentScale = _scale;
	SetWorldMatrix(speedNums);
}

// 速度割合に応じた色の更新
void SpeedUI::UpdateSpeedRatioColor(float _ratio)
{
	XMFLOAT3 color;

	// Phase1
	if (_ratio < changeColorRatioPhase2)
	{
		color.x = speedRatioColor1.x;
		color.y = speedRatioColor1.y;
		color.z = speedRatioColor1.z;
	}
	// Phase2
	else if (_ratio < changeColorRatioPhase3)
	{
		color.x = speedRatioColor2.x;
		color.y = speedRatioColor2.y;
		color.z = speedRatioColor2.z;
	}
	// Phase3
	else if (_ratio < changeColorRatioPhase4)
	{
		color.x = speedRatioColor3.x;
		color.y = speedRatioColor3.y;
		color.z = speedRatioColor3.z;
	}
	// Phase4
	else
	{
		color.x = speedRatioColor4.x;
		color.y = speedRatioColor4.y;
		color.z = speedRatioColor4.z;
	}

	// カラーセット
	for (auto& ui : speedNums)
	{
		ui->SetRGBColor(color);
	}
}

// 各割合数を設定
void SpeedUI::SetSpeedRatioColorPhase(float phase1, float phase2, float phase3, float phase4)
{
	changeColorRatioPhase1 = phase1;
	changeColorRatioPhase2 = phase2;
	changeColorRatioPhase3 = phase3;
	changeColorRatioPhase4 = phase4;
}

// 各割合の色を設定
void SpeedUI::SetSpeedRatioColor(
	DirectX::XMFLOAT3 colorPhase1,
	DirectX::XMFLOAT3 colorPhase2,
	DirectX::XMFLOAT3 colorPhase3,
	DirectX::XMFLOAT3 colorPhase4)
{
	speedRatioColor1 = colorPhase1;
	speedRatioColor2 = colorPhase2;
	speedRatioColor3 = colorPhase3;
	speedRatioColor4 = colorPhase4;
}

// アクティブ状態を一括切り替え
void SpeedUI::SetActive(bool _active)
{
	for (auto& ui : speedNums)
	{
		ui->SetIsActive(_active);
	}
}