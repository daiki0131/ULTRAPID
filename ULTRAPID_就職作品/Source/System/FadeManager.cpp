#include "FadeManager.h"
#include "DebugUI.h"

// �R���X�g���N�^
FadeManager::FadeManager(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
    :GameObject(_pos, _rot, _scale, _color)
{
}

// �X�V����
void FadeManager::Update(float _deltaTime)
{
    // �t�F�[�h��Ԃ̂Ƃ�
    if (isFade) 
    {
        // �t�F�[�h�C�����̂Ƃ�
        if (isFadeIn) 
        {
            // ���l�������Ă���
            color.w -= fadeSpeed * _deltaTime;

            // ���l��0�ɂȂ�����t�F�[�h�C���I��
            if (color.w <= 0.0f)
            {
                color.w = 0.0f;
                isFade = false;
            }
        }
        // �t�F�[�h�A�E�g���̂Ƃ�
        else {

            // ���l���グ�Ă���
            color.w += fadeSpeed * _deltaTime;

            // ���l��1�ɂȂ�����t�F�[�h�A�E�g�I��
            if (color.w >= 1.0f)
            {
                color.w = 1.0f;
                isFade = false;
            }
        }
    }
}

// �`�揈��
void FadeManager::Draw()
{
    // Renderer���M�p�I�u�W�F�N�g�f�[�^
    Renderer::ObjectBuffer ob;
    ob.matrixWorld = XMMatrixTranspose(matrixWorld);
    ob.color = color;

    // �ʏ�`��
    Renderer::DrawObject(model, ob, std::nullopt, useVS, usePS, true, false, true);
}

// �t�F�[�h�C�����J�n������
void FadeManager::StartFadeIn()
{
    color.w = 1.0f;
    isFadeIn = true;
    isFade = true;
}

// �t�F�[�h�A�E�g���J�n������
void FadeManager::StartFadeOut()
{
    color.w = 0.0f;
    isFadeIn = false;
    isFade = true;
}

void FadeManager::SetIMGUI()
{
    ImGui::Begin("SceneFadeParam");

    ImGui::SliderFloat("FadeSpeed", &fadeSpeed, 1.0f, 3.0f);

    ImGui::End();
}