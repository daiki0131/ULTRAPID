#pragma once
#include "NumberTexture2D.h"
#include <map>

//===========================================//
// ���xUI��p�N���X
//===========================================//
class SpeedUI : public NumberTexture2D
{
public:
    // ���x�̊e���̈�
    enum SPEED_PLACE
    {
        PLACE_1,
        PLACE_10,
        PLACE_100,
        PLACE_1000,
        MILLI_PLACE_1,
        MILLI_PLACE_10,
        OTHER,          // ��Ƀh�b�g�ȂǂɎg�p
    };
    static constexpr float MaxSpeedScaledValue = 1000.0f;
    static constexpr float SpeedConversionFactor = 5.0f;
private:
    std::vector<std::unique_ptr<Object2D>> speedNums; // ���xUI
    std::map<int, SPEED_PLACE> speedPlace;            // ���x�̈�
    Renderer::Model model;
    float maxSpeed = 0.0f;

    float changeColorRatioPhase1 = 0.0f;              // �F�ύX�̑�1�i�K��
    float changeColorRatioPhase2 = 0.0f;              // �F�ύX�̑�2�i�K��
    float changeColorRatioPhase3 = 0.0f;              // �F�ύX�̑�3�i�K��
    float changeColorRatioPhase4 = 0.0f;              // �F�ύX�̑�4�i�K��

    DirectX::XMFLOAT3 speedRatioColor1{};             // ��1�i�K�ڂ̐F
    DirectX::XMFLOAT3 speedRatioColor2{};             // ��2�i�K�ڂ̐F
    DirectX::XMFLOAT3 speedRatioColor3{};             // ��3�i�K�ڂ̐F
    DirectX::XMFLOAT3 speedRatioColor4{};             // ��4�i�K�ڂ̐F

public:
    SpeedUI();
    void Update(const float _speed);
    void Draw();
    // Set�֐�
    void SetParentPosition(DirectX::XMFLOAT3 _pos) override;
    void SetParentScale(DirectX::XMFLOAT3 _scale) override;
    void SetMaxSpeed(float _speed) { maxSpeed = _speed; }
    void SetSpeedRatioColorPhase(float phase1, float phase2, float phase3, float phase4);
    void SetSpeedRatioColor(
        DirectX::XMFLOAT3 colorPhase1,
        DirectX::XMFLOAT3 colorPhase2,
        DirectX::XMFLOAT3 colorPhase3,
        DirectX::XMFLOAT3 colorPhase4);
    void SetActive(bool _active);
    // ���xUI�̐F���X�V
    void UpdateSpeedRatioColor(float _ratio);

};

