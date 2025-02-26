#pragma once
#include "Object2D.h"

//===========================================//
// 速度ゲージUI専用クラス
//===========================================//
class SpeedGaugeUI : public Object2D
{
private:
    float changeColorRatioPhase1 = 0.0f;              // 色変更の第1段階目
    float changeColorRatioPhase2 = 0.0f;              // 色変更の第2段階目
    float changeColorRatioPhase3 = 0.0f;              // 色変更の第3段階目
    float changeColorRatioPhase4 = 0.0f;              // 色変更の第4段階目

    DirectX::XMFLOAT3 speedRatioColor1{};             // 第1段階目の色
    DirectX::XMFLOAT3 speedRatioColor2{};             // 第2段階目の色
    DirectX::XMFLOAT3 speedRatioColor3{};             // 第3段階目の色
    DirectX::XMFLOAT3 speedRatioColor4{};             // 第4段階目の色

    Renderer::GaugeColorBuffer gcb{};                 // Renderer送信用データ
    float ratio = 0.0f;                               // 速度割合
public:
	SpeedGaugeUI(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
	void Draw() override;
    void SetSpeedRatioColorPhase(float phase1, float phase2, float phase3, float phase4);
    void SetSpeedRatioColor(
        DirectX::XMFLOAT3 colorPhase1,
        DirectX::XMFLOAT3 colorPhase2,
        DirectX::XMFLOAT3 colorPhase3,
        DirectX::XMFLOAT3 colorPhase4);
    void InitGaugeColor();
    void SetRatio(float _ratio) { ratio = _ratio; };
};

