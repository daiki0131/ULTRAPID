#pragma once
#include <memory>
#include <unordered_map>
#include <map>
#include "FadeManager.h"
#include "Renderer.h"
#include "GameManager.h"

class SceneBase;
class Camera2D;

//===========================================//
// �V�[���S�̂��Ǘ�����N���X
//===========================================//
class SceneManager
{
private:
	static std::unique_ptr<SceneBase> nowScene;     // ���݂̃V�[��
	std::unique_ptr<SceneBase> nextScene;			// �O�̃V�[��
	static std::unique_ptr<FadeManager> sceneFade;  // �V�[���J�ڐ�p�t�F�[�h
	std::unique_ptr<Camera2D> camera2D;             // ���ׂẴV�[����ʂ��Ďg���J����
	bool isChangingScene = false;                   // �V�[���J�ڒ���
	// ���f���ǂݍ��ݐ�p
	typedef std::unordered_map<std::wstring, Renderer::Model> ModelHash;
	std::unique_ptr<ModelHash> modelManager;
	Renderer::Model model;

public:
	void Update(float _deltaTime, GameManager& gm);
	void Draw();
	void DrawShadowMap();
	void Init();
	void UnInit();
	// �ŏ��̃V�[���̏������iInit�֐����O�ɍs���j
	void SetInitScene(std::unique_ptr<SceneBase> _initScene);
	// �V�����V�[�����Z�b�g
	void SetScene(std::unique_ptr<SceneBase> _nextScene);
	// Set�֐�
	static FadeManager& GetFade() { return *sceneFade; }
	static void SetSceneIMGUI();
	// Get�֐�
	bool GetIsChangeScene() { return isChangingScene; };
	bool GetIsSceneFade() { return sceneFade->GetIsFade(); }
	
private:
	// �V�[���؂�ւ�
	void ChangeScene();
};