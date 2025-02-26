#pragma once
#include "GameObject.h"
#include "Collision.h"
#include <algorithm>

//===========================================//
// プレイヤー専用クラス
//===========================================//
class Player : public GameObject
{
public:
    static constexpr float MaxRollDistance = 1.3f;         // 最大の回転の長さ
    static constexpr int MaxMotionBlurImprintNum = 256;    // モーションブラー（残像）の描画数
    static constexpr float MaxMotionBlurAlphaValue = 3.0f; // モーションブラーの最大のα値
    static constexpr float MaxRadius = 2.0f;               // 最大のプレイヤーの半径
    static constexpr float MaxScale = 0.3f;                // プレイヤーの最大の大きさ
   
protected:
    Collision::BoundingSphere boundingSphere{};            // オブジェクトの球体情報
    Collision::BoundingSphere itemBounds{};                // アイテム専用の当たり判定の球体情報
    Renderer::Model model;
    Renderer::Material mat;                                // Renderer送信用マテリアル情報
    std::vector<Renderer::ObjectBuffer> instanceDatas;      // Renderer送信用オブジェクトデータ
    Shader::VS_NAME useVS = Shader::VS_TEXTURE;             // 使用する頂点シェーダー
    Shader::PS_NAME usePS = Shader::PS_TEXTURE;             // 使用するピクセルシェーダー
    Shader::VS_NAME useMotionBlurVS = Shader::VS_TEXTURE;   // モーションブラー専用頂点シェーダー
    Shader::PS_NAME useMotionBlurPS = Shader::PS_TEXTURE;   // モーションブラー専用ピクセルシェーダー
    DirectX::XMFLOAT3 startPosition = { 0,0,0 };           // 最初の座標
    DirectX::XMFLOAT3 newPosition = { 0,0,0 };             // 移動先の座標
    DirectX::XMFLOAT3 oldPosition = { 0,0,0 };             // 前の座標
    std::vector<DirectX::XMFLOAT3> oldPositionsMotionBlur; // 前の座標
    int oldPositionsCountMotionBlur = 0;                   // 前の座標コンテナのインデックス番号
    int motionBlurImprintNum = 128;                        // モーションブラー（残像）の描画数
    float motionBlurAlphaValue = 0.6f;                     // モーションブラー時のα値
    float defaultScale = 0.0f;                             // 通常のプレイヤー大きさ（プレイヤーの回転時に使用）
    float speedRatio = 0.0f;                               // 現在の速度の割合

public:
    Player(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    virtual ~Player() = default;
    virtual void Update(float _deltatime) override;        
    void Draw() override; 
    void DrawShadowMap();
    // モーションブラーの描画
    void MortionBlurDraw();
    // 座標更新
    void AddPosition(DirectX::XMFLOAT3 _addPos);      
    // 過去の座標更新処理
    void OldPositionsMotionBlurUpdate(const DirectX::XMFLOAT3 _pos, const bool isAllSet);
    // プレイヤーの回転処理
    void RotationPlayer();                               
    // Set関数
    void SetPosition(DirectX::XMFLOAT3 _pos) override { position = _pos; }
    void SetRotation(DirectX::XMFLOAT3 _rot) { rotation = _rot; }
    void SetBoundingSphere(DirectX::XMFLOAT3 _center, float _radius);
    void SetItemBoundingSphere(DirectX::XMFLOAT3 _center, float _radius);
    void SetModel(Renderer::Model& m) { model = m; }
    void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; };
    void SetOldPosition(DirectX::XMFLOAT3 _pos) { oldPosition = _pos; }
    void SetMotionBlurShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useMotionBlurVS = _vs, useMotionBlurPS = _ps; }
    void SetMotionBlurImprintNum(int _num) { motionBlurImprintNum = _num; }
    void SetMotionBlurAlphaValue(float _value) { motionBlurAlphaValue = _value; }
    void SetScaleValue(float _value) { scaling.x = _value, scaling.y = _value, scaling.z = _value; }
    void SetRadiusValue(float _value) { SetBoundingSphere(position, _value); }
    void SetNewPosition(DirectX::XMFLOAT3 _pos) { newPosition = _pos; }
    void SetSpeedRatio(float _ratio) { speedRatio = std::clamp(_ratio, 0.0f, 1.0f); }
    // Get関数
    Collision::BoundingSphere& GetBoundingSphere() { return boundingSphere; };
    Collision::BoundingSphere& GetItemBoundingSphere() { return itemBounds; };
    Renderer::Model& GetModel() { return model; }
    DirectX::XMFLOAT3 GetOldPosition() { return oldPosition; };
    DirectX::XMFLOAT3 GetStartPosition() const { return startPosition; }
    int GetMotionBlurImprintNum() const { return motionBlurImprintNum; }
    float GetMotionBlurAlphaValue() const { return motionBlurAlphaValue; }
    float GetScaleValue() const { return scaling.x; }
    float GetRadiusValue() const { return boundingSphere.radius; }
    DirectX::XMFLOAT3 GetNewPosition() { return newPosition; }
    float GetSpeedRatio() { return speedRatio; };

};