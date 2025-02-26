#include "DebugUI.h"
#include "XInputGamePad.h"

std::vector<std::function<void(void)>> DebugUI::m_debugfunction;

void DebugUI::Init(ID3D11Device* device, ID3D11DeviceContext* context, HWND hWnd)
{

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(device, context);
}

void DebugUI::DisposeUI() {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// �f�o�b�O�\���֐��̓o�^
void DebugUI::RedistDebugFunction(std::function<void(void)> f) {
    m_debugfunction.push_back(std::move(f));
}

void DebugUI::ClearDebugFunction()
{
    m_debugfunction.clear();
}

void DebugUI::Render(HWND hWnd) {
    // ImGui�̐V�����t���[�����J�n
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hWnd, &p);  // hwnd�̓E�B���h�E�̃n���h��
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)p.x, (float)p.y);  // �}�E�X�ʒu
    io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;  // ���N���b�N

    // �f�o�b�O�֐��̎��s
    for (auto& f : m_debugfunction)
    {
        f();
    }

    // �t���[���̃����_�����O������
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}