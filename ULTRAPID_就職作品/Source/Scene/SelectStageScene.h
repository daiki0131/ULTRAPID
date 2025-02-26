#pragma once
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include "SceneBase.h"
#include "Object2D.h"
#include "ScoreUI.h"

class Camera2D;

//===========================================//
// �Z���N�g�X�e�[�W�V�[���N���X
//===========================================//
class SelectStageScene : public SceneBase
{

private:
	std::unique_ptr<Object2D> selectStageTexture;                                    // SelectStage�e�N�X�`��
	std::unique_ptr<Object2D> frame01;                                               // �t���[��1
	std::unique_ptr<Object2D> frame02;                                               // �t���[��2
	std::unique_ptr<Object2D> bestScoreTexture;                                      // BestScore�e�N�X�`��
	std::vector<std::unique_ptr<Object2D>> stageNames;                               // �X�e�[�W���e�N�X�`��
	std::vector<std::unique_ptr<Object2D>> stageMapPhoto;                            // �X�e�[�W�̎ʐ^
	std::unique_ptr<ScoreUI> scoreUI;                                                // �X�R�A����UI
	
	int selectStageCount = 0;                                                        // �I�𒆃X�e�[�W�̃J�E���g
	int selectStageScore = 0;                                                        // �I�𒆃X�e�[�W�̃X�R�A
	DirectX::XMFLOAT3 nameUpPosition = DirectX::XMFLOAT3(-1.4f, 0.5f, -0.2f);        // �X�e�[�W����̍��W
	DirectX::XMFLOAT3 nameMiddlePosition = DirectX::XMFLOAT3(-1.4f, 0.0f, -0.2f);    // �X�e�[�W�����̍��W
	DirectX::XMFLOAT3 nameDownPosition = DirectX::XMFLOAT3(-1.4f, -0.5f, -0.2f);     // �X�e�[�W�����̍��W
	DirectX::XMFLOAT3 nameUpDownScale = DirectX::XMFLOAT3(1.0f, 0.233f, 1.0f);       // �X�e�[�W���㉺�̃X�P�[��
	DirectX::XMFLOAT3 nameMiddleScale = DirectX::XMFLOAT3(1.5f, 0.35f, 1.0f);        // �X�e�[�W�����̃X�P�[��

	// ���f�����
	typedef std::unordered_map<std::wstring, Renderer::Model> ModelHash;
	std::unique_ptr<ModelHash> modelManager;
	Renderer::Model model;


public:
	void Init() override;
	void UnInit() override;
	void Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm) override;
	void Draw(Camera2D& _camera2D) override;
	void DrawShadowMap() override {};
	// �p�����[�^�ۑ�
	void SaveParameters() override;
	// �p�����[�^�ǂݍ���
	void LoadParameters()  override;
	// Set�֐�
	void SetIMGUI() override;
};