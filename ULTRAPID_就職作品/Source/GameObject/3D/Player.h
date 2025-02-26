#pragma once
#include "GameObject.h"
#include "Collision.h"
#include <algorithm>

//===========================================//
// �v���C���[��p�N���X
//===========================================//
class Player : public GameObject
{
public:
    static constexpr float MaxRollDistance = 1.3f;         // �ő�̉�]�̒���
    static constexpr int MaxMotionBlurImprintNum = 256;    // ���[�V�����u���[�i�c���j�̕`�搔
    static constexpr float MaxMotionBlurAlphaValue = 3.0f; // ���[�V�����u���[�̍ő�̃��l
    static constexpr float MaxRadius = 2.0f;               // �ő�̃v���C���[�̔��a
    static constexpr float MaxScale = 0.3f;                // �v���C���[�̍ő�̑傫��
   
protected:
    Collision::BoundingSphere boundingSphere{};            // �I�u�W�F�N�g�̋��̏��
    Collision::BoundingSphere itemBounds{};                // �A�C�e����p�̓����蔻��̋��̏��
    Renderer::Model model;
    Renderer::Material mat;                                // Renderer���M�p�}�e���A�����
    std::vector<Renderer::ObjectBuffer> instanceDatas;      // Renderer���M�p�I�u�W�F�N�g�f�[�^
    Shader::VS_NAME useVS = Shader::VS_TEXTURE;             // �g�p���钸�_�V�F�[�_�[
    Shader::PS_NAME usePS = Shader::PS_TEXTURE;             // �g�p����s�N�Z���V�F�[�_�[
    Shader::VS_NAME useMotionBlurVS = Shader::VS_TEXTURE;   // ���[�V�����u���[��p���_�V�F�[�_�[
    Shader::PS_NAME useMotionBlurPS = Shader::PS_TEXTURE;   // ���[�V�����u���[��p�s�N�Z���V�F�[�_�[
    DirectX::XMFLOAT3 startPosition = { 0,0,0 };           // �ŏ��̍��W
    DirectX::XMFLOAT3 newPosition = { 0,0,0 };             // �ړ���̍��W
    DirectX::XMFLOAT3 oldPosition = { 0,0,0 };             // �O�̍��W
    std::vector<DirectX::XMFLOAT3> oldPositionsMotionBlur; // �O�̍��W
    int oldPositionsCountMotionBlur = 0;                   // �O�̍��W�R���e�i�̃C���f�b�N�X�ԍ�
    int motionBlurImprintNum = 128;                        // ���[�V�����u���[�i�c���j�̕`�搔
    float motionBlurAlphaValue = 0.6f;                     // ���[�V�����u���[���̃��l
    float defaultScale = 0.0f;                             // �ʏ�̃v���C���[�傫���i�v���C���[�̉�]���Ɏg�p�j
    float speedRatio = 0.0f;                               // ���݂̑��x�̊���

public:
    Player(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    virtual ~Player() = default;
    virtual void Update(float _deltatime) override;        
    void Draw() override; 
    void DrawShadowMap();
    // ���[�V�����u���[�̕`��
    void MortionBlurDraw();
    // ���W�X�V
    void AddPosition(DirectX::XMFLOAT3 _addPos);      
    // �ߋ��̍��W�X�V����
    void OldPositionsMotionBlurUpdate(const DirectX::XMFLOAT3 _pos, const bool isAllSet);
    // �v���C���[�̉�]����
    void RotationPlayer();                               
    // Set�֐�
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
    // Get�֐�
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