#pragma once
#include "Object2D.h"

//===========================================//
// 2DによるUVアニメーションクラス
//===========================================//
class UVAnimation2D : public Object2D
{
private:
    bool isAnimation = false;      // アニメーション可否
    bool isLoopAnimation = false;  // ループ可否
    float animCounter = 0.0f;      // 現在のコマのカウント数
    float animSpeed = 0.1f;        // アニメーション速度
    int divVertical = 1;           // 画像縦の分割数
    int divSide = 1;               // 画像横の分割数
    int animID = 0;                // コマの番号
    std::vector<int> animTable;    // コマ送りのテーブル

public:
    UVAnimation2D(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    void Update(float _deltaTime) override;
    // Set関数
    void SetAnimationSpeed(float _speed) { animSpeed = _speed; }
    void SetIsLoop(bool _isLoop) { isLoopAnimation = _isLoop; }
    void SetDivision(int side, int vertical) { divSide = side, divVertical = vertical; }
    void SetAnimationTable(int tableNum);
    // 再生
    void Play();
    // 停止
    void Stop();
};