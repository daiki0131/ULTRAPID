#pragma once
#include "Object2D.h"

//===========================================//
// ���x�Q�[�WUI��p�N���X
//===========================================//
class SpeedGaugeUI : public Object2D
{
private:
    float changeColorRatioPhase1 = 0.0f;              // �F�ύX�̑�1�i�K��
    float changeColorRatioPhase2 = 0.0f;              // �F�ύX�̑�2�i�K��
    float changeColorRatioPhase3 = 0.0f;              // �F�ύX�̑�3�i�K��
    float changeColorRatioPhase4 = 0.0f;              // �F�ύX�̑�4�i�K��

    DirectX::XMFLOAT3 speedRatioColor1{};             // ��1�i�K�ڂ̐F
    DirectX::XMFLOAT3 speedRatioColor2{};             // ��2�i�K�ڂ̐F
    DirectX::XMFLOAT3 speedRatioColor3{};             // ��3�i�K�ڂ̐F
    DirectX::XMFLOAT3 speedRatioColor4{};             // ��4�i�K�ڂ̐F

    Renderer::GaugeColorBuffer gcb{};                 // Renderer���M�p�f�[�^
    float ratio = 0.0f;                               // ���x����
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

