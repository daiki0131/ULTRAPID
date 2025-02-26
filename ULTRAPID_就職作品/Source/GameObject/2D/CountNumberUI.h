#pragma once
#include "NumberTexture2D.h"

//===========================================//
// ゲームスタートカウント専用クラス
//===========================================//
class CountNumberUI : public NumberTexture2D
{
private:
    std::unique_ptr<Object2D> numUI; // 数字UI
    Renderer::Model model;

public:
    CountNumberUI();
    void Draw();
    void SetActive(bool _active) { numUI->SetIsActive(_active); }
    // Set関数
    void SetNum(int _num);
    void SetParentPosition(DirectX::XMFLOAT3 _pos) override {};
    void SetParentScale(DirectX::XMFLOAT3 _scale) override {};
};

