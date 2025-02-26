#pragma once
#include "Object2D.h"

//===========================================//
// 2D�ɂ��UV�A�j���[�V�����N���X
//===========================================//
class UVAnimation2D : public Object2D
{
private:
    bool isAnimation = false;      // �A�j���[�V������
    bool isLoopAnimation = false;  // ���[�v��
    float animCounter = 0.0f;      // ���݂̃R�}�̃J�E���g��
    float animSpeed = 0.1f;        // �A�j���[�V�������x
    int divVertical = 1;           // �摜�c�̕�����
    int divSide = 1;               // �摜���̕�����
    int animID = 0;                // �R�}�̔ԍ�
    std::vector<int> animTable;    // �R�}����̃e�[�u��

public:
    UVAnimation2D(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
    void Update(float _deltaTime) override;
    // Set�֐�
    void SetAnimationSpeed(float _speed) { animSpeed = _speed; }
    void SetIsLoop(bool _isLoop) { isLoopAnimation = _isLoop; }
    void SetDivision(int side, int vertical) { divSide = side, divVertical = vertical; }
    void SetAnimationTable(int tableNum);
    // �Đ�
    void Play();
    // ��~
    void Stop();
};