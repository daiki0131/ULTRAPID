#pragma once
#include "NumberTexture2D.h"
#include <map>

//===========================================//
// ��������UI��p�N���X
//===========================================//
class TimeUI : public NumberTexture2D
{
public:
    // �������Ԃ̊e���̈�
    enum TIME_DIGIT_NAME
    {
        MINUTE_DIGIT1,       // ��1����
        MINUTE_DIGIT2,       // ��2����
        SECOND_DIGIT1,       // �b1����
        SECOND_DIGIT2,       // �b2����
        MILLISECONND_DIGIT1, // �~���b1����
        MILLISECONND_DIGIT2, // �~���b2����
        OTHER,               // ��Ƀh�b�g�ȂǂɎg�p
    };

    const float ChangeColorSecond = 10.0f; // �F��ς���c��b���̖ڈ�

private:
    std::vector<std::unique_ptr<Object2D>> timeNums; // ��������UI
    std::map<int, TIME_DIGIT_NAME> timePlace;        // �������Ԃ̈�
    Renderer::Model model;

public:
    TimeUI();
    void Update(const float _time);
    void Draw();
    // Set�֐�
    void SetParentPosition(DirectX::XMFLOAT3 _pos) override;
    void SetParentScale(DirectX::XMFLOAT3 _scale) override;
    void SetActive(bool _active);
};