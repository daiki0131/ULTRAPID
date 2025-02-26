#pragma once
#include "GameObject.h"
#include "Collision.h"

class CollisionManager;
class Items;

//===========================================//
// �Q�[�����A�C�e���������N���X
//===========================================//
class ItemObject : public GameObject
{
public:
    // �A�C�e���̃^�C�v
    enum ITEM_TYPE
    {
        SMALL,     // ��
        MIDDLE,    // ��
        BIG,       // ��
        MEGA_BIG,  // ����
    };
    // �A�C�e���̏��
    enum ITEM_STATE
    {
        TOUCH_ITEM,  // �G��邱�Ƃ��\�ȏ��
        WAIT,        // �Đ����҂�����
        EXPLOSION,   // ���U
    };

protected:
    Collision::BoundingSphere boundingSphere{};    // �I�u�W�F�N�g�̋��̏��
    ITEM_TYPE type = ItemObject::SMALL;            // �^�C�v
    ITEM_STATE state = ItemObject::TOUCH_ITEM;       // ���
    Renderer::ShaderAnimationGSBuffer gsb;         // ���ŃA�j���[�V�����p�o�b�t�@
    DirectX::XMFLOAT4 defaultColor = { 1,1,1,1 };  // �ʏ�J���[
    int score = 0;                                 // �l���X�R�A  
    bool isBreakable = true;                       // �l���\��
    bool isHitCheck = true;                        // �v���C���[�Ƃ̔����
    float fragmentAlpha = 0.0f;                    // ���ŃA�j���[�V�������̃��l
    float breakRatio = 0;                          // �l���\�̔{��
    float gettableTime = 0;                        // �Ď擾����
    float returnCount = 0.0f;                      // �Ď擾���Ԃ̃J�E���g��
public:
    ItemObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    void Update(float _deltaTime, float _realRatio, float _alphaDecayRate);
    void Draw() override {};
    // Set�֐�
    void SetBoundingSphere(DirectX::XMFLOAT3 _center, float _radius);
    void SetDefaultColor(DirectX::XMFLOAT4 _color) { defaultColor = _color; }
    void SetType(ITEM_TYPE _type) { type = _type; }
    void SetState(ITEM_STATE _state);
    void SetFragmentAlpha(float _alpha) { fragmentAlpha = _alpha; }
    void SetScore(int _score) { score = _score; }
    void SetBreakRatio(float _ratio) { breakRatio = _ratio; }
    void SetGettableTime(float _time) { gettableTime = _time; }
   // Get�֐�
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