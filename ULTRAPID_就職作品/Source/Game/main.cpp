#undef UNICODE
#include <Windows.h>
#include "Input.h"
#include "XInputGamePad.h"
#include "SceneManager.h"
#include "Fps.h"
#include "DebugUI.h"
#include "TitleScene.h"
#include "SelectStageScene.h"
#include "GameScene.h"
#include "Renderer.h"
#include "Geometry.h"
#include "Shader.h"
#include "Particle.h"
#include "GameManager.h"
#include "Collision.h"
#include "DebugConsole.h"

constexpr const char* ClassName = "HAL_AT13D432_13";// �E�C���h�E�N���X��
constexpr const char* WindowName = "ULTRAPID";      // �E�C���h�E�̖��O
constexpr int ScreenWidth = 1920;                    // �E�C���h�E�̕�
constexpr int ScreenHeight = 1080;                   // �E�C���h�E�̍���

// �v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//=================================
// �G���g���|�C���g
//=================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �f�o�b�O�E�C���h�E����
	DebugConsole::CreateDebugConsole();

	// �E�C���h�E�N���X WNDCLASSEX
	WNDCLASSEX wc;

	// �\���̂̃T�C�Y
	wc.cbSize = sizeof(WNDCLASSEX);
	// �N���X�̃X�^�C��
	wc.style = CS_CLASSDC;
	// �E�C���h�E�v���V�[�W���ւ̃|�C���^
	wc.lpfnWndProc = WndProc;
	// �E�C���h�E�N���X�\���̂̌�Ɋ��蓖�Ă�]���ȃo�C�g��
	wc.cbClsExtra = 0;
	// �E�C���h�E�C���X�^���X�̌�Ɋ��蓖�Ă�]���ȃo�C�g��
	wc.cbWndExtra = 0;
	// �N���X�̃E�B���h�E�v���V�[�W�����܂ރC���X�^���X�ւ̃n���h��
	wc.hInstance = hInstance;
	// �N���X�A�C�R���ւ̃n���h��
	wc.hIcon = NULL;
	// �N���X�J�[�\���ւ̃n���h��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// �N���X�̔w�i�u���V�ւ̃n���h��
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	// �N���X���j���[�̃��\�[�X�����w�肷��null�ŏI��镶����ւ̃|�C���^
	wc.lpszMenuName = NULL;
	// null�ŏI��镶����ւ̃|�C���^
	wc.lpszClassName = ClassName;
	// �E�C���h�E�N���X�Ɋ֘A�t�����Ă��鏬���ȃA�C�R���ւ̃n���h��
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);// ����:WNDCLASSEX�\����

	// �E�C���h�n���h�� HWND
	HWND hWnd;

	hWnd = CreateWindowEx(0,   // �g���E�C���h�E�X�^�C��
		ClassName,             // �E�C���h�E�N���X�̖��O
		WindowName,            // �E�C���h�E�̖��O
		WS_OVERLAPPEDWINDOW,   // �E�C���h�E�X�^�C��
		CW_USEDEFAULT,         // �E�C���h�E�̍���w���W
		CW_USEDEFAULT,         // �E�C���h�E�̍���x���W 
		ScreenWidth,           // �E�C���h�E�̕�
		ScreenHeight,          // �E�C���h�E�̍���
		NULL,                  // �e�E�C���h�E�̃n���h��
		NULL,                  // ���j���[�n���h���܂��͎q�E�C���h�EID
		hInstance,             // �C���X�^���X�n���h��
		NULL);                 // �E�C���h�E�쐬�f�[�^

	// �E�C���h�E��\��
	ShowWindow(hWnd, nCmdShow);
	// �E�C���h�E�̃N���C�A���g�̈���X�V
	UpdateWindow(hWnd);

	//==========����������=============//

	// �����_���[�N���X������
	Renderer::Init(hWnd);
	// �V�F�[�_�[�N���X������
	Shader::CreateShader();
	// ���b�V���쐬
	Geometry::Init();
	// �p�[�e�B�N���V�X�e��������
	Particle::Init();
	// �����蔻��N���X������
	Collision::Init();
	// IMGUI������
	DebugUI::Init(Renderer::GetDevice().Get(), Renderer::GetContext().Get(), hWnd);
	
	// �Q�[���}�l�[�W���[�N���X
	GameManager gameManager;
	gameManager.Init();

	// �V�[���}�l�[�W���[�N���X
	SceneManager sceneManager;
	sceneManager.SetInitScene(std::make_unique<TitleScene>());
	//sceneManager.SetInitScene(std::make_unique<SelectStageScene>());
	//sceneManager.SetInitScene(std::make_unique<GameScene>());

	sceneManager.Init();

	// ���̓N���X�̏�����
	Input::Get().Initialize(hWnd, hInstance);

	// FPS�������iFPS60�Őݒ�j
	FPS::Get()->Init(60.0f);

	// �f�o�b�O����IMGUI�\���E��\���؂�ւ�
#ifdef _DEBUG
	bool isRendererIMGUI = true;
#endif

	MSG msg;

	//===========�Q�[�����[�v==============//
	for (;;)
	{
		BOOL isAnyMessage = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

		// �������b�Z�[�W�����邩����
		if (isAnyMessage)           
		{
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) 
			{
				break;
			}
		}
		else
		{
			// Escape�L�[�������ꂽ�烁�b�Z�[�W�{�b�N�X���o��
			if (Input::Get().GetKeyTrigger(DIK_ESCAPE))
			{
				// �A�v���P�[�V�����I���{�b�N�X�\��
				if (MessageBoxA(NULL, "�A�v���P�[�V�������I�����܂����H", "ULTRAPID", MB_YESNO) == IDYES)
				{
					PostQuitMessage(0); // �A�v���I��
				}
			}

			// FPS�X�V
			FPS::Get()->Update();
			// ���͍X�V����
			Input::Get().Update();
			XInputGamePad::Get().Update();
			// �V�[���X�V����
			sceneManager.Update(FPS::Get()->GetFixedDeltaTime(), gameManager);
			// �V���h�E�}�b�v������
			Renderer::ClearShadowMapScreen();
			// �V���h�E�}�b�v�`��
			sceneManager.DrawShadowMap();
			// ��ʏ�����
			Renderer::ClearScreen();
			// �V�[���`��
			sceneManager.Draw();

#ifdef _DEBUG
			// �L�[�{�[�h��G�L�[��IMGUI�\���E��\���؂�ւ�
			if (Input::Get().GetKeyTrigger(DIK_G))
			{
				if (isRendererIMGUI)
				{
					isRendererIMGUI = false;
				}
				else
				{
					isRendererIMGUI =  true;
				}
			}
			if (isRendererIMGUI)
			{
				DebugUI::Render(hWnd); // IMGUI�X�V
			}
			
#endif
			// ��ʍX�V
			Renderer::UpdateScreen();
		}
	}
	// �������
	gameManager.UnInit();
	sceneManager.UnInit();
	DebugUI::DisposeUI();

#ifdef _DEBUG
	DebugConsole::SaveLogMesseage();
#endif

	UnregisterClass(ClassName, hInstance);

	return (int)msg.wParam; // �v���O�����I��
}

//========================================
//�R�[���o�b�N�֐�
//========================================
LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:         // �E�C���h�E�j���̃��b�Z�[�W
		
		PostQuitMessage(0);  // �A�v���I��
		break;

	case WM_CLOSE:           // �E�C���h�EX�{�^���������ꂽ��
		DestroyWindow(hWnd); //�gWM_DESTROY�h���b�Z�[�W�𑗂�
		break;

	case WM_KEYDOWN:         // �L�[�������ꂽ�C�x���g
		break;

	case WM_KEYUP:           // �L�[�������ꂽ�C�x���g
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}