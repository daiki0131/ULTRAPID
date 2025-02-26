#define _CRT_SECURE_NO_WARNINGS
#include "xa2.h"
#include <vector>
#include "Geometry.h"
#include "Input.h"
#include "SceneManager.h"
#include "SceneBase.h"
#include "DebugUI.h"
#include "DebugConsole.h"
#include "GameObject.h"
#include "Camera.h"

using namespace DirectX;

std::unique_ptr<SceneBase> SceneManager::nowScene;
std::unique_ptr<FadeManager> SceneManager::sceneFade;

// �����������i�v���O�������s���j
void SceneManager::SetInitScene(std::unique_ptr<SceneBase> _initScene)
{
	// BGM�ESE������
	HRESULT hr = XA_Initialize();
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("�T�E���h���������s���܂����FSceneManager::SetInitScene���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// ������ǂݍ��ރV�[��������
	nowScene = std::move(_initScene);
	// �V�[��������
	nowScene->Init();

	// IMGUI�֐��z�񏉊���
	DebugUI::ClearDebugFunction();

	// IMGUI�֐��̓o�^
	DebugUI::RedistDebugFunction(SetSceneIMGUI);

	// �t�F�[�h�I�u�W�F�N�g����
	sceneFade = std::make_unique<FadeManager>(
		XMFLOAT3(0.0f, 0.0f, -0.25f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(16.0f, 9.0f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));
	// �t�F�[�h�C���J�n
	sceneFade->StartFadeIn();

	// �V�[����IMGUI�p�����[�^��ǂݍ���
	nowScene->LoadParameters();
}

// �J�ڑO�̃V�[����ݒ�
void SceneManager::SetScene(std::unique_ptr<SceneBase> _nextScene)
{
	// ���̃V�[�������Ă���
	nextScene = std::move(_nextScene);

	// �t�F�[�h�A�E�g�J�n
	sceneFade->StartFadeOut();
	isChangingScene = true;
}

// �V�[���̐؂�ւ�
void SceneManager::ChangeScene()
{
	// �O�V�[���̉������
	if (nowScene)
	{
		nowScene->UnInit();
	}

	// �I�u�W�F�N�g���ڏ�
	nowScene = std::move(nextScene);

	// �t�F�[�h�C���J�n
	sceneFade->StartFadeIn();
	isChangingScene = false;

	// ���̃V�[���̏���������
	if (nowScene)
	{
		nowScene->Init();
		nowScene->LoadParameters();
		DebugUI::ClearDebugFunction();
		DebugUI::RedistDebugFunction(SetSceneIMGUI);
	}
}

// �X�V����
void SceneManager::Update(float _deltaTime, GameManager& gm)
{
	// �J����2D�X�V
	camera2D->Update(_deltaTime);

	// �V�[�����`�F���W����ꍇ�̂�
	if (isChangingScene)
	{
		// �t�F�[�h���I������珈����ʂ�
		if (!sceneFade->GetIsFade())
		{
			ChangeScene(); // �V�����V�[���ɐ؂�ւ�
		}
	}

	if (nowScene)
	{
		// ���݂̃V�[���̍X�V����
		nowScene->Update(_deltaTime, *this, gm);
	}

	// �t�F�[�h�X�V
	sceneFade->Update(_deltaTime);
}

// �`�揈��
void SceneManager::Draw()
{
	if (nowScene)
	{
		// ���݂̃V�[���̕`�揈��
		nowScene->Draw(*camera2D);
	}

	// 2D�J�����̃r���[�E�v���W�F�N�V�����s����Z�b�g
	Renderer::SetCameraViewProjection(camera2D->GetViewMatrix(), camera2D->GetProjectionMatrix());

	// �t�F�[�h�`��
	sceneFade->Draw();
}

void SceneManager::DrawShadowMap()
{
	if (nowScene)
	{
		nowScene->DrawShadowMap();
	}
}

// �V�[���}�l�[�W���[�̏���������
void SceneManager::Init()
{
	//=================================================
	//  �I�u�W�F�N�g����
	//=================================================
	modelManager = std::make_unique<ModelHash>();
	camera2D = std::make_unique<Camera2D>();
	
	//=================================================
	//  �e�N�X�`���ǂݍ���
	//=================================================
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1.0f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/fade.png");
	modelManager->emplace(L"Assets/Texture/fade.png", model);

	//=================================================
	//  ���f���Z�b�g
	//=================================================
	model = modelManager->at(L"Assets/Texture/fade.png");
	sceneFade->SetModel(model);

	//=================================================
	//  �����ݒ�
	//=================================================

	// 2D�J����
	camera2D->SetEye({ 0,0,-1 });
	camera2D->SetFocus({ 0,0,1 });
}

// �������
void SceneManager::UnInit()
{
	//�T�E���h�̉������
	XA_Release();

#ifdef _DEBUG
	// �f�o�b�O���̂�IMGUI�ő��삵���p�����[�^��CSV�ɕۑ�����
	nowScene->SaveParameters();
#endif

}

// �V�[�����Ƃ�IMGUI�֐������s
void SceneManager::SetSceneIMGUI()
{
#ifdef _DEBUG
	nowScene->SetIMGUI();
#endif
	
}