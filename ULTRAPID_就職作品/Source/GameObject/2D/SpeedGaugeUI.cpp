#include "SpeedGaugeUI.h"

using namespace DirectX;

// コンストラクタ
SpeedGaugeUI::SpeedGaugeUI(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	: Object2D(_pos, _rot, _scale, _color)
{
}

// 描画処理
void SpeedGaugeUI::Draw()
{
	if (!isActive) return; // 非アクティブ状態の場合return

	// オブジェクトデータ（Colorは無視する）
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose(matrixWorld); // ワールド変換行列セット

	// UV変換行列設定
	model.matrixUV = XMMatrixTranspose(GetUvMatrix());

	// 速度割合設定（その他の内容は初期化時に設定済）
	gcb.speedRatio = ratio;

	// 描画
	Renderer::DrawGaugeObject(model, ob, gcb, useVS, usePS, true);
}

// 各割合数を設定
void SpeedGaugeUI::SetSpeedRatioColorPhase(float phase1, float phase2, float phase3, float phase4)
{
	changeColorRatioPhase1 = phase1;
	changeColorRatioPhase2 = phase2;
	changeColorRatioPhase3 = phase3;
	changeColorRatioPhase4 = phase4;
}

// 各割合の色を設定
void SpeedGaugeUI::SetSpeedRatioColor(
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

// 各割合をバッファデータに格納
// ※SetSpeedRatioColorPhaseとSetSpeedRatioColorを設定した後に呼び出す
void SpeedGaugeUI::InitGaugeColor()
{
	gcb.color[0] = XMFLOAT4(speedRatioColor1.x, speedRatioColor1.y, speedRatioColor1.z, 1.0f);
	gcb.color[1] = XMFLOAT4(speedRatioColor2.x, speedRatioColor2.y, speedRatioColor2.z, 1.0f);
	gcb.color[2] = XMFLOAT4(speedRatioColor3.x, speedRatioColor3.y, speedRatioColor3.z, 1.0f);
	gcb.color[3] = XMFLOAT4(speedRatioColor4.x, speedRatioColor4.y, speedRatioColor4.z, 1.0f);

	gcb.thresholds = XMFLOAT4(changeColorRatioPhase1, changeColorRatioPhase2, changeColorRatioPhase3, changeColorRatioPhase4);

	gcb.lowAlphaValue = Object2D::UiLowAlpha;
}