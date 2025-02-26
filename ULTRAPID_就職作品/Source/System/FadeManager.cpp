#include "FadeManager.h"
#include "DebugUI.h"

// コンストラクタ
FadeManager::FadeManager(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
    :GameObject(_pos, _rot, _scale, _color)
{
}

// 更新処理
void FadeManager::Update(float _deltaTime)
{
    // フェード状態のとき
    if (isFade) 
    {
        // フェードイン中のとき
        if (isFadeIn) 
        {
            // α値を下げていく
            color.w -= fadeSpeed * _deltaTime;

            // α値が0になったらフェードイン終了
            if (color.w <= 0.0f)
            {
                color.w = 0.0f;
                isFade = false;
            }
        }
        // フェードアウト中のとき
        else {

            // α値を上げていく
            color.w += fadeSpeed * _deltaTime;

            // α値が1になったらフェードアウト終了
            if (color.w >= 1.0f)
            {
                color.w = 1.0f;
                isFade = false;
            }
        }
    }
}

// 描画処理
void FadeManager::Draw()
{
    // Renderer送信用オブジェクトデータ
    Renderer::ObjectBuffer ob;
    ob.matrixWorld = XMMatrixTranspose(matrixWorld);
    ob.color = color;

    // 通常描画
    Renderer::DrawObject(model, ob, std::nullopt, useVS, usePS, true, false, true);
}

// フェードインを開始させる
void FadeManager::StartFadeIn()
{
    color.w = 1.0f;
    isFadeIn = true;
    isFade = true;
}

// フェードアウトを開始させる
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