#pragma once

#include <d3d11.h>
#include <vector>
#include <functional>
#include "imGui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

class DebugUI {
    static std::vector<std::function<void(void)>> m_debugfunction;
public:
    static void Init(ID3D11Device* device, ID3D11DeviceContext* context, HWND hWnd);
    
    // �f�o�b�O�\���֐��̓o�^
    static void RedistDebugFunction(std::function<void(void)> f);

    static void ClearDebugFunction();

    static void Render(HWND hWnd);

    static void DisposeUI();
};