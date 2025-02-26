#pragma once
#include "Player.h"
#include "Input.h"
#include "XInputGamePad.h"

//===========================================//
// プレイヤーを操作するためのクラス
//===========================================//
class PlayerController : public Player
{
public:
    static constexpr int AccelerationsNum = 10;                // 加速度の総数
    static constexpr float MaxAcceleration = 10.0f;            // 最大加速度
    static constexpr float MinDeceleration = 3.0f;             // 最小減速度
    static constexpr float WallDecelerationStartRatio = 0.05f; // 壁で減速を開始する速度の割合
    static constexpr float DriftingAngle = 10.0f;              // ドリフトのスライドする角度  
    static constexpr float MaxMoveSpeed = 300.0f;              // 最大速度
    static constexpr float MaxDriftAddSpeed = 50.0f;           // 最大ドリフト加速度
    static constexpr float MaxMass = 5.0f;                     // 最大質量
    static constexpr float MaxJumpPower = 100.0f;              // 最大ジャンプ力
    static constexpr float TurnRateMaxNum = 10.0f;             // 最大方向転換割合
    static constexpr float MaxFrictionFactor = 1.0f;           // 最大減速係数
    static constexpr float SonicBoomMaxSpeed = 3.0f;
    static constexpr float SonicBoomMaxScale = 20.0f;
    static constexpr float SonicBoomBaseScale = 1.5f;

    // ドリフト状態
    enum DRIFT_STATE
    {
        NO_DRIFTING,  // ドリフト可能
        NOW_DRIFTING, // ドリフト中
        AFTER,        // ドリフト後
    };
    // ジャンプ状態
    enum JUMP_STATE
    {
        CAN_JUMP,     // ジャンプ可能状態
        JUMPING,      // ジャンプ中
        FALLING,      // 落下中
    };
   
private:
    float dirAngle = 0.0f;                    // 次に進む方向の角度
    float angle = 0.0f;                       // プレイヤーの角度
    float moveSpeed = 0.0f;                   // 仮の移動速度
    float realSpeed = 0.0f;                   // 実際に反映する速度
    float maxMoveSpeed = 150.0f;              // 最大加速移動速度
    std::vector<float> accelerations;         // 加速度（10段階に設定）
    int accelerationsIndex = 0;               // 現在の加速度のインデックス番号
    float deceleration = 0.0f;                // 減速度
    float gravityDot = 0.0f;                  // 重力の強さ
    float mass = 2.5f;                        // プレイヤーの質量
    float jumpPower = 30.0f;                  // ジャンプ力
    float minTurnRate = 0.3f;                 // 低速時の方向転換割合
    float maxTurnRate = 3.0f;                 // 高速時の方向転換割合
    float frictionFactor = 0.3f;              // 減速時の摩擦係数
    float wallFrictionFactor = 0.5f;          // 減速時（壁）の摩擦係数
    float holeDecelerationRate = 0.5f;        // コースアウト時の速度減速割合
    bool isGround = false;                    // 地面判定
    bool isHitStop = false;                   // ヒットストップ判定
    bool isWallHit = false;                   // 壁接触判定
    bool isDriftFxPlay = true;                // ドリフトエフェクト再生可否
    DirectX::XMFLOAT3 driftFxScale{};         // ドリフト時のエフェクトの大きさ
    float driftFxAlpha = 0.2f;                // ドリフトエフェクトのα値
    float driftFxScaleRatio = 1.2f;           // ドリフトエフェクトの大きさ割合
    float driftFxAddScaleSpeed = 0.01f;       // ドリフトエフェクトの変化速度
    DRIFT_STATE driftState = NO_DRIFTING;     // ドリフト状態
    bool isDrifting = false;                  // ドリフト判定
    float driftAngle = 0.0f;                  // ドリフト時の角度
    float driftTargetAngle = 0.0f;            // ドリフト解除時に進む方向の角度
    float preDriftSpeed = 0.0f;               // ドリフト前の速度
    float driftSpeedFactor = 0.5f;            // ドリフト時の速度の摩擦係数
    float returnTime = 0.75f;                 // ドリフト後の元の速度に戻るまでの時間
    float afterDriftingElapsedTime = 0.0f;    // ドリフト直後の経過時間
    float afterDriftingSpeed = 0.0f;          // ドリフト直後の速度
    float driftingAddSpeed = 0.0f;            // ドリフト後の加速度
    float nowDriftingElapsedTime = 0.0f;      // ドリフト中の経過時間
    float maxAddSpeedTime = 0.0f;             // ドリフトの加速時間
    float driftPossibleSpeed = 30.0f;         // ドリフト可能になる目安速度
    float driftDeceleration = 0.1f;           // ドリフト後の減速度
    float maxDriftTime = 2.0f;                // ドリフト中最大の時間
    float maxDriftAddSpeed = 50.0f;           // ドリフト最大の加速度
    float maxDriftAddSpeedTime = 0.5f;        // ドリフト直後の減速するまでの時間
    float driftFrictionFactor = 0.1f;         // ドリフト後の減速の摩擦係数
    DirectX::XMFLOAT3 driftDirection{};       // ドリフトする方向
    bool isInputJumpKey = false;              // ジャンプキーフラグ
    bool isInputDriftKey = false;             // ドリフトキーフラグ
    XInputGamePad::AnalogStick analogStick{}; // ゲームパッドアナログスティック
    DirectX::XMFLOAT2 moveKey{};              // キーボード操作専用移動キー
    DirectX::XMFLOAT3 velocity{};             // 移動方向速度
    DirectX::XMFLOAT3 collisionNormal{};      // 当たった地形の法線方向
    JUMP_STATE jumpState = CAN_JUMP;          // ジャンプ状態

    DirectX::XMFLOAT3 sonicBoomPos = { 0,0,0 };
    DirectX::XMFLOAT3 sonicBoomRotation = { 0,0,0 };
    DirectX::XMFLOAT3 sonicBoomScale = { 1,1,1 };
    Renderer::Model sonicBoomModel;
    float sonicBoomElapsedCount = 0.0f;


    float changeColorRatioPhase1 = 0.0f;      // 速度割合による色変換の1段階目
    float changeColorRatioPhase2 = 0.0f;      // 速度割合による色変換の2段階目
    float changeColorRatioPhase3 = 0.0f;      // 速度割合による色変換の3段階目
    float changeColorRatioPhase4 = 0.0f;      // 速度割合による色変換の4段階目

    DirectX::XMFLOAT3 speedRatioColor1{};     // 速度割合によるの1段階目の色
    DirectX::XMFLOAT3 speedRatioColor2{};     // 速度割合によるの2段階目の色
    DirectX::XMFLOAT3 speedRatioColor3{};     // 速度割合によるの3段階目の色
    DirectX::XMFLOAT3 speedRatioColor4{};     // 速度割合によるの4段階目の色

public:
    PlayerController(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    virtual void Update(float _deltaTime) override;
    // ドリフトエフェクト描画
    void DriftingEffectDraw();
    void SonicBoomDraw();
    // キー操作イベント
    void HandleKeyEvent(float _deltaTime);
    // 移動
    void Move(float _deltaTime);
    // ジャンプ
    void Jump(float _deltaTime);
    // コースアウト後の値の反映
    void HoleAfterSet();
    // 速度割合時の色更新
    void UpdateSpeedRatioColor();
    // Set関数
    void SetDirAngle(float _angle) { dirAngle = _angle; }
    void SetCollisionNormal(DirectX::XMFLOAT3 _normal) { collisionNormal = _normal; }
    void SetVelocityY(float _velocityY) { velocity.y = _velocityY; }
    void SetIsGround(bool _isGround) { isGround = _isGround; }
    void SetJumpState(JUMP_STATE _state) { jumpState = _state; }
    void SetAngle(float _angle) { angle = _angle; }
    void SetIsWallHit(bool _isHit) { isWallHit = _isHit; }
    void SetIsHitStop(bool _isHitStop) { isHitStop = _isHitStop; }
    void SetIsDriftingEffectPlay(bool _isPlay) { isDriftFxPlay = _isPlay; }
    void SetSpeedRatioColorPhase(float phase1, float phase2, float phase3, float phase4);
    void SetSpeedRatioColor(
        DirectX::XMFLOAT3 colorPhase1,
        DirectX::XMFLOAT3 colorPhase2,
        DirectX::XMFLOAT3 colorPhase3,
        DirectX::XMFLOAT3 colorPhase4);
    void SetAccelerations(std::vector<float> accelers);
    void SetMaxMoveSpeed(float _speed) { maxMoveSpeed = _speed; }
    void SetMaxDriftAddSpeed(float _speed) { maxDriftAddSpeed = _speed; }
    void SetMass(float _mass) { mass = _mass; }
    void SetJumpPower(float _power) { jumpPower = _power; }
    void SetMinTurnRate(float _rate) { minTurnRate = _rate; }
    void SetMaxTurnRate(float _rate) { maxTurnRate = _rate; }
    void SetFrictionFactor(float _factor) { frictionFactor = _factor; }
    void SetWallFrictionFactor(float _factor) { wallFrictionFactor = _factor; }
    // Get関数
    bool GetIsGround() { return isGround; }
    JUMP_STATE GetJumpState() { return jumpState; }
    float CalculateDeceleration(float frictionFactor, float _moveSpeed);
    float CalculateSpeedRatio(float _realSpeed, float _maxSpeed);
    float GetMaxMoveSpeed() { return maxMoveSpeed; }
    float GetAngle() { return angle; }
    float GetRealSpeed() { return realSpeed; }
    int GetAccelerationsIndex() { return accelerationsIndex; }
    std::vector<float> GetAccelerations() { return accelerations; }
    float GetMaxDriftAddSpeed() { return maxDriftAddSpeed; }
    float GetMass() { return mass; }
    float GetJumpPower() { return jumpPower; }
    float GetMinTurnRate() { return minTurnRate; }
    float GetMaxTurnRate() { return maxTurnRate; }
    float GetFrictionFactor() { return frictionFactor; }
    float GetWallFrictionFactor() { return wallFrictionFactor; }
    DirectX::XMFLOAT3 GetCollisionNormal() { return collisionNormal; }
};