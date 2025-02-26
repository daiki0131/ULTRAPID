#pragma once
#include "NumberTexture2D.h"

//===========================================//
// �Q�[���X�^�[�g�J�E���g��p�N���X
//===========================================//
class CountNumberUI : public NumberTexture2D
{
private:
    std::unique_ptr<Object2D> numUI; // ����UI
    Renderer::Model model;

public:
    CountNumberUI();
    void Draw();
    void SetActive(bool _active) { numUI->SetIsActive(_active); }
    // Set�֐�
    void SetNum(int _num);
    void SetParentPosition(DirectX::XMFLOAT3 _pos) override {};
    void SetParentScale(DirectX::XMFLOAT3 _scale) override {};
};

