#pragma once
#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include "SceneBase.h"
#include "CollisionManager.h"
#include "Particle.h"
#include "Camera.h"
#include "Object2D.h"
#include "Player.h"
#include "Boxs.h"
#include "Items.h"
#include "Light.h"
#include "Frustum.h"

//===========================================//
// �^�C�g���V�[���N���X
//===========================================//
class TitleScene : public SceneBase
{
public:
	const float MinLimitPos = 100.0f;                  // �`�悷��v���C���[�̍ŏ��̌��E���W
	const float MaxLimitPos = 200.0f;                  // �`�悷��v���C���[�̍ő�̌��E���W
	const float MinLimitSpeed = 0.1f;                  // �`�悷��v���C���[�̍ŏ����x
	const float MaxLimitSpeed = 1.5f;                  // �`�悷��v���C���[�̍ő呬�x
	const float MaxTexPos = 2.0f;                      // 2D�e�N�X�`���̍��W�����l
	const float MaxTexScale = 3.0f;                    // 2D�e�N�X�`���̃X�P�[�������l

private:
	std::unique_ptr<Object2D> titleTexture;            // �^�C�g�����e�N�X�`��
	std::unique_ptr<Object2D> pressEnterKeyTexture;    // PressEnterKey�e�N�X�`��
	std::unique_ptr<Boxs> ground;                      // �n�ʃI�u�W�F�N�g
	std::unique_ptr<Items> item;                       // �A�C�e���I�u�W�F�N�g
	std::unique_ptr<Player> player;                    // �v���C���[�I�u�W�F�N�g
	std::unique_ptr<TitleSceneCamera> camera3D;        // �J����3D
	std::unique_ptr<Light> sceneLight;                 // ���C�g
	std::unique_ptr<ItemEffect> itemFx;                // �A�C�e���G�t�F�N�g
	CollisionManager collisionManager;                 // CollisionManager�I�u�W�F�N�g
	Frustum frustum;

	float limitPlayerPositionX = MinLimitPos;          // �`�悷��v���C���[�̌��E���W
	float playerSpeed = MinLimitSpeed;                 // �`�悷��v���C���[�̑��x

#ifdef _DEBUG
	std::unique_ptr<DebugCamera> debugCamera3D;        // �f�o�b�O��p�J����
	bool isDebugCameraChange = false;                  // �f�o�b�O�J�����ƒʏ�J�����̐؂�ւ�
	bool isDebugDrawUI = true;                         // UI�̕\���E��\��
#endif

	// ���f�����
	typedef std::unordered_map<std::wstring, Renderer::Model> ModelHash;
	std::unique_ptr<ModelHash> modelManager;
	Renderer::Model model;

public:
	void Init() override;
	void UnInit() override;
	void Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm) override;
	void Draw(Camera2D& _camera2D) override;
	void DrawShadowMap() override;
	// �p�����[�^�ۑ�
	void SaveParameters() override;
	// �p�����[�^�ǂݍ���
	void LoadParameters()  override;
	// �����蔻����s�֐�
	void CheckObjectCollision();
	// Set�֐�
	void SetLimitPlayerPositionX(float value) { limitPlayerPositionX = value; }
	void SetPlayerSpeed(float value) { playerSpeed = value; }
	void SetIMGUI() override;
	// Get�֐�
	Player& GetPlayer() { return *player; }
	float GetLimitPlayerPositionX() const { return limitPlayerPositionX; }
	float GetPlayerSpeed() const { return playerSpeed; }
};