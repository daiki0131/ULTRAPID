#pragma once
#include "Player.h"
#include "Input.h"
#include "XInputGamePad.h"

//===========================================//
// �v���C���[�𑀍삷�邽�߂̃N���X
//===========================================//
class PlayerController : public Player
{
public:
    static constexpr int AccelerationsNum = 10;                // �����x�̑���
    static constexpr float MaxAcceleration = 10.0f;            // �ő�����x
    static constexpr float MinDeceleration = 3.0f;             // �ŏ������x
    static constexpr float WallDecelerationStartRatio = 0.05f; // �ǂŌ������J�n���鑬�x�̊���
    static constexpr float DriftingAngle = 10.0f;              // �h���t�g�̃X���C�h����p�x  
    static constexpr float MaxMoveSpeed = 300.0f;              // �ő呬�x
    static constexpr float MaxDriftAddSpeed = 50.0f;           // �ő�h���t�g�����x
    static constexpr float MaxMass = 5.0f;                     // �ő县��
    static constexpr float MaxJumpPower = 100.0f;              // �ő�W�����v��
    static constexpr float TurnRateMaxNum = 10.0f;             // �ő�����]������
    static constexpr float MaxFrictionFactor = 1.0f;           // �ő匸���W��
    static constexpr float SonicBoomMaxSpeed = 3.0f;
    static constexpr float SonicBoomMaxScale = 20.0f;
    static constexpr float SonicBoomBaseScale = 1.5f;

    // �h���t�g���
    enum DRIFT_STATE
    {
        NO_DRIFTING,  // �h���t�g�\
        NOW_DRIFTING, // �h���t�g��
        AFTER,        // �h���t�g��
    };
    // �W�����v���
    enum JUMP_STATE
    {
        CAN_JUMP,     // �W�����v�\���
        JUMPING,      // �W�����v��
        FALLING,      // ������
    };
   
private:
    float dirAngle = 0.0f;                    // ���ɐi�ޕ����̊p�x
    float angle = 0.0f;                       // �v���C���[�̊p�x
    float moveSpeed = 0.0f;                   // ���̈ړ����x
    float realSpeed = 0.0f;                   // ���ۂɔ��f���鑬�x
    float maxMoveSpeed = 150.0f;              // �ő�����ړ����x
    std::vector<float> accelerations;         // �����x�i10�i�K�ɐݒ�j
    int accelerationsIndex = 0;               // ���݂̉����x�̃C���f�b�N�X�ԍ�
    float deceleration = 0.0f;                // �����x
    float gravityDot = 0.0f;                  // �d�͂̋���
    float mass = 2.5f;                        // �v���C���[�̎���
    float jumpPower = 30.0f;                  // �W�����v��
    float minTurnRate = 0.3f;                 // �ᑬ���̕����]������
    float maxTurnRate = 3.0f;                 // �������̕����]������
    float frictionFactor = 0.3f;              // �������̖��C�W��
    float wallFrictionFactor = 0.5f;          // �������i�ǁj�̖��C�W��
    float holeDecelerationRate = 0.5f;        // �R�[�X�A�E�g���̑��x��������
    bool isGround = false;                    // �n�ʔ���
    bool isHitStop = false;                   // �q�b�g�X�g�b�v����
    bool isWallHit = false;                   // �ǐڐG����
    bool isDriftFxPlay = true;                // �h���t�g�G�t�F�N�g�Đ���
    DirectX::XMFLOAT3 driftFxScale{};         // �h���t�g���̃G�t�F�N�g�̑傫��
    float driftFxAlpha = 0.2f;                // �h���t�g�G�t�F�N�g�̃��l
    float driftFxScaleRatio = 1.2f;           // �h���t�g�G�t�F�N�g�̑傫������
    float driftFxAddScaleSpeed = 0.01f;       // �h���t�g�G�t�F�N�g�̕ω����x
    DRIFT_STATE driftState = NO_DRIFTING;     // �h���t�g���
    bool isDrifting = false;                  // �h���t�g����
    float driftAngle = 0.0f;                  // �h���t�g���̊p�x
    float driftTargetAngle = 0.0f;            // �h���t�g�������ɐi�ޕ����̊p�x
    float preDriftSpeed = 0.0f;               // �h���t�g�O�̑��x
    float driftSpeedFactor = 0.5f;            // �h���t�g���̑��x�̖��C�W��
    float returnTime = 0.75f;                 // �h���t�g��̌��̑��x�ɖ߂�܂ł̎���
    float afterDriftingElapsedTime = 0.0f;    // �h���t�g����̌o�ߎ���
    float afterDriftingSpeed = 0.0f;          // �h���t�g����̑��x
    float driftingAddSpeed = 0.0f;            // �h���t�g��̉����x
    float nowDriftingElapsedTime = 0.0f;      // �h���t�g���̌o�ߎ���
    float maxAddSpeedTime = 0.0f;             // �h���t�g�̉�������
    float driftPossibleSpeed = 30.0f;         // �h���t�g�\�ɂȂ�ڈ����x
    float driftDeceleration = 0.1f;           // �h���t�g��̌����x
    float maxDriftTime = 2.0f;                // �h���t�g���ő�̎���
    float maxDriftAddSpeed = 50.0f;           // �h���t�g�ő�̉����x
    float maxDriftAddSpeedTime = 0.5f;        // �h���t�g����̌�������܂ł̎���
    float driftFrictionFactor = 0.1f;         // �h���t�g��̌����̖��C�W��
    DirectX::XMFLOAT3 driftDirection{};       // �h���t�g�������
    bool isInputJumpKey = false;              // �W�����v�L�[�t���O
    bool isInputDriftKey = false;             // �h���t�g�L�[�t���O
    XInputGamePad::AnalogStick analogStick{}; // �Q�[���p�b�h�A�i���O�X�e�B�b�N
    DirectX::XMFLOAT2 moveKey{};              // �L�[�{�[�h�����p�ړ��L�[
    DirectX::XMFLOAT3 velocity{};             // �ړ��������x
    DirectX::XMFLOAT3 collisionNormal{};      // ���������n�`�̖@������
    JUMP_STATE jumpState = CAN_JUMP;          // �W�����v���

    DirectX::XMFLOAT3 sonicBoomPos = { 0,0,0 };
    DirectX::XMFLOAT3 sonicBoomRotation = { 0,0,0 };
    DirectX::XMFLOAT3 sonicBoomScale = { 1,1,1 };
    Renderer::Model sonicBoomModel;
    float sonicBoomElapsedCount = 0.0f;


    float changeColorRatioPhase1 = 0.0f;      // ���x�����ɂ��F�ϊ���1�i�K��
    float changeColorRatioPhase2 = 0.0f;      // ���x�����ɂ��F�ϊ���2�i�K��
    float changeColorRatioPhase3 = 0.0f;      // ���x�����ɂ��F�ϊ���3�i�K��
    float changeColorRatioPhase4 = 0.0f;      // ���x�����ɂ��F�ϊ���4�i�K��

    DirectX::XMFLOAT3 speedRatioColor1{};     // ���x�����ɂ���1�i�K�ڂ̐F
    DirectX::XMFLOAT3 speedRatioColor2{};     // ���x�����ɂ���2�i�K�ڂ̐F
    DirectX::XMFLOAT3 speedRatioColor3{};     // ���x�����ɂ���3�i�K�ڂ̐F
    DirectX::XMFLOAT3 speedRatioColor4{};     // ���x�����ɂ���4�i�K�ڂ̐F

public:
    PlayerController(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    virtual void Update(float _deltaTime) override;
    // �h���t�g�G�t�F�N�g�`��
    void DriftingEffectDraw();
    void SonicBoomDraw();
    // �L�[����C�x���g
    void HandleKeyEvent(float _deltaTime);
    // �ړ�
    void Move(float _deltaTime);
    // �W�����v
    void Jump(float _deltaTime);
    // �R�[�X�A�E�g��̒l�̔��f
    void HoleAfterSet();
    // ���x�������̐F�X�V
    void UpdateSpeedRatioColor();
    // Set�֐�
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
    // Get�֐�
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