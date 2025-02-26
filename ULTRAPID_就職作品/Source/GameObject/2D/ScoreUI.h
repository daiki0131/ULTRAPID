#pragma once
#include "NumberTexture2D.h"
#include <map>

//===========================================//
// �X�R�AUI��p�N���X
//===========================================//
class ScoreUI : public NumberTexture2D
{
public:
    
    // �X�R�A�̊e���̈�
    enum SCORE_PLACE
    {
        PLACE_1,     // 1�̈�
        PLACE_10,    // 10�̈�
        PLACE_100,   // 100�̈�
        PLACE_1000,  // 1000�̈�
        PLACE_10000, // 10000�̈�
        PLACE_MAX,
    };
private:
	std::vector<std::unique_ptr<Object2D>> scores; // �X�R�AUI
    std::map<int, SCORE_PLACE> scorePlace;         // �X�R�A�̈�
    Renderer::Model model;
    
public:
	ScoreUI();
	void Update(const int _score);
	void Draw();
    // Set�֐�
    void SetParentPosition(DirectX::XMFLOAT3 _pos) override;
    void SetParentScale(DirectX::XMFLOAT3 _scale) override;
    void SetActive(bool _active);
};

