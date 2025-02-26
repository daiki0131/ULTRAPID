#pragma once
#include "GameObject.h"
#include "Collision.h"

class CollisionManager;
class Items;

//===========================================//
// ゲーム内アイテムを扱うクラス
//===========================================//
class ItemObject : public GameObject
{
public:
    // アイテムのタイプ
    enum ITEM_TYPE
    {
        SMALL,     // 小
        MIDDLE,    // 中
        BIG,       // 大
        MEGA_BIG,  // 特大
    };
    // アイテムの状態
    enum ITEM_STATE
    {
        TOUCH_ITEM,  // 触れることが可能な状態
        WAIT,        // 再生成待ち時間
        EXPLOSION,   // 爆散
    };

protected:
    Collision::BoundingSphere boundingSphere{};    // オブジェクトの球体情報
    ITEM_TYPE type = ItemObject::SMALL;            // タイプ
    ITEM_STATE state = ItemObject::TOUCH_ITEM;       // 状態
    Renderer::ShaderAnimationGSBuffer gsb;         // 消滅アニメーション用バッファ
    DirectX::XMFLOAT4 defaultColor = { 1,1,1,1 };  // 通常カラー
    int score = 0;                                 // 獲得スコア  
    bool isBreakable = true;                       // 獲得可能可否
    bool isHitCheck = true;                        // プレイヤーとの判定可否
    float fragmentAlpha = 0.0f;                    // 消滅アニメーション時のα値
    float breakRatio = 0;                          // 獲得可能の倍率
    float gettableTime = 0;                        // 再取得時間
    float returnCount = 0.0f;                      // 再取得時間のカウント数
public:
    ItemObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    void Update(float _deltaTime, float _realRatio, float _alphaDecayRate);
    void Draw() override {};
    // Set関数
    void SetBoundingSphere(DirectX::XMFLOAT3 _center, float _radius);
    void SetDefaultColor(DirectX::XMFLOAT4 _color) { defaultColor = _color; }
    void SetType(ITEM_TYPE _type) { type = _type; }
    void SetState(ITEM_STATE _state);
    void SetFragmentAlpha(float _alpha) { fragmentAlpha = _alpha; }
    void SetScore(int _score) { score = _score; }
    void SetBreakRatio(float _ratio) { breakRatio = _ratio; }
    void SetGettableTime(float _time) { gettableTime = _time; }
   // Get関数
    Collision::BoundingSphere& GetBoundingSphere() { return boundingSphere; }
    ITEM_TYPE GetType() { return type; }
    ITEM_STATE GetState() { return state; }
    float GetFragmentAlpha() { return fragmentAlpha; }
    int GetScore() { return score; }
    bool GetIsHitCheke() const { return isHitCheck; }
    bool GetIsTouched() { return state == ITEM_STATE::TOUCH_ITEM; }
    DirectX::XMFLOAT3 GetPosition() { return position; }
    Renderer::ShaderAnimationGSBuffer GetGSBuffer() { return gsb; }
    bool GetIsBreakable() { return isBreakable; }

};