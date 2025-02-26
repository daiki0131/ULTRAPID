#pragma once
#include "NumberTexture2D.h"
#include <map>

//===========================================//
// 制限時間UI専用クラス
//===========================================//
class TimeUI : public NumberTexture2D
{
public:
    // 制限時間の各桁の位
    enum TIME_DIGIT_NAME
    {
        MINUTE_DIGIT1,       // 分1桁目
        MINUTE_DIGIT2,       // 分2桁目
        SECOND_DIGIT1,       // 秒1桁目
        SECOND_DIGIT2,       // 秒2桁目
        MILLISECONND_DIGIT1, // ミリ秒1桁目
        MILLISECONND_DIGIT2, // ミリ秒2桁目
        OTHER,               // 主にドットなどに使用
    };

    const float ChangeColorSecond = 10.0f; // 色を変える残り秒数の目安

private:
    std::vector<std::unique_ptr<Object2D>> timeNums; // 制限時間UI
    std::map<int, TIME_DIGIT_NAME> timePlace;        // 制限時間の位
    Renderer::Model model;

public:
    TimeUI();
    void Update(const float _time);
    void Draw();
    // Set関数
    void SetParentPosition(DirectX::XMFLOAT3 _pos) override;
    void SetParentScale(DirectX::XMFLOAT3 _scale) override;
    void SetActive(bool _active);
};