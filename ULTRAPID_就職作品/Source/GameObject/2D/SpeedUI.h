#pragma once
#include "NumberTexture2D.h"
#include <map>

//===========================================//
// 速度UI専用クラス
//===========================================//
class SpeedUI : public NumberTexture2D
{
public:
    // 速度の各桁の位
    enum SPEED_PLACE
    {
        PLACE_1,
        PLACE_10,
        PLACE_100,
        PLACE_1000,
        MILLI_PLACE_1,
        MILLI_PLACE_10,
        OTHER,          // 主にドットなどに使用
    };
    static constexpr float MaxSpeedScaledValue = 1000.0f;
    static constexpr float SpeedConversionFactor = 5.0f;
private:
    std::vector<std::unique_ptr<Object2D>> speedNums; // 速度UI
    std::map<int, SPEED_PLACE> speedPlace;            // 速度の位
    Renderer::Model model;
    float maxSpeed = 0.0f;

    float changeColorRatioPhase1 = 0.0f;              // 色変更の第1段階目
    float changeColorRatioPhase2 = 0.0f;              // 色変更の第2段階目
    float changeColorRatioPhase3 = 0.0f;              // 色変更の第3段階目
    float changeColorRatioPhase4 = 0.0f;              // 色変更の第4段階目

    DirectX::XMFLOAT3 speedRatioColor1{};             // 第1段階目の色
    DirectX::XMFLOAT3 speedRatioColor2{};             // 第2段階目の色
    DirectX::XMFLOAT3 speedRatioColor3{};             // 第3段階目の色
    DirectX::XMFLOAT3 speedRatioColor4{};             // 第4段階目の色

public:
    SpeedUI();
    void Update(const float _speed);
    void Draw();
    // Set関数
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
    // 速度UIの色を更新
    void UpdateSpeedRatioColor(float _ratio);

};

