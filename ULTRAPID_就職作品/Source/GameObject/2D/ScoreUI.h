#pragma once
#include "NumberTexture2D.h"
#include <map>

//===========================================//
// スコアUI専用クラス
//===========================================//
class ScoreUI : public NumberTexture2D
{
public:
    
    // スコアの各桁の位
    enum SCORE_PLACE
    {
        PLACE_1,     // 1の位
        PLACE_10,    // 10の位
        PLACE_100,   // 100の位
        PLACE_1000,  // 1000の位
        PLACE_10000, // 10000の位
        PLACE_MAX,
    };
private:
	std::vector<std::unique_ptr<Object2D>> scores; // スコアUI
    std::map<int, SCORE_PLACE> scorePlace;         // スコアの位
    Renderer::Model model;
    
public:
	ScoreUI();
	void Update(const int _score);
	void Draw();
    // Set関数
    void SetParentPosition(DirectX::XMFLOAT3 _pos) override;
    void SetParentScale(DirectX::XMFLOAT3 _scale) override;
    void SetActive(bool _active);
};

