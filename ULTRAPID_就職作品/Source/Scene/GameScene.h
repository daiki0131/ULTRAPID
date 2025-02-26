#pragma once
#include "SceneBase.h"
#include "Boxs.h"
#include "PlayerController.h"
#include "Items.h"
#include "Slopes.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "Particle.h"
#include "Frustum.h"
#include "ScoreUI.h"
#include "SpeedUI.h"
#include "TimeUI.h"
#include "SpeedGaugeUI.h"
#include "UVAnimation2D.h"
#include "CountNumberUI.h"
#include "Light.h"

//===========================================//
// ���C���Q�[���V�[���N���X
//===========================================//
class GameScene : public SceneBase
{
public:
    static constexpr int MaxRecordDataSize = 300;  // �v���C���[�ƃJ�����̏���ۑ�����ő�̃R���e�i�T�C�Y
  
public:
    // �Q�[���V�[���̏��
    enum GAME_SCENE_STATE
    {
        NO_PLAY,            // �J�n�O
        PLAY,               // �v���C��
        TIME_UP,            // �^�C���A�b�v����
        RESULT,             // ���U���g�\��
        SCENE_END,          // �Q�[���I������
    };
    // �X�e�[�W��̃I�u�W�F�N�g�̎��
    enum MAP_OBJECT_TYPE
    {
       GROUND,              // �n��
       WALL,                // ��
       SLOPE,               // �⓹
       PLAYER         = 10, // �v���C���[
       ITEM_SMALL,          // �A�C�e����
       ITEM_MIDDLE,         // �A�C�e����
       ITEM_BIG,            // �A�C�e����
       ITEM_MEGA_BIG,       // �A�C�e������
    };
    // �}�b�v�I�u�W�F�N�g�̏��
    struct MapObject
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 rotation;
        DirectX::XMFLOAT3 scale;
        MAP_OBJECT_TYPE objType;
    };
    // �J�����ۑ��f�[�^�i�R�[�X�A�E�g���g�p�j
    struct CameraRecordData
    {
        DirectX::XMFLOAT3 eye;
        float angle;
    };
    // �v���C���[�ۑ��f�[�^�i�R�[�X�A�E�g���g�p�j
    struct PlayerRecordData
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 oldPosition;
        float angle;
    };

private:
    // �Q�[���I�u�W�F�N�g
    std::unique_ptr<PlayerController> player;           // �v���C���[
    std::unique_ptr<Boxs> ground;                       // �n��
    std::unique_ptr<Boxs> wall;                         // ��
    std::unique_ptr<Slopes> slope;                      // �⓹
    std::unique_ptr<Items> item;                        // �A�C�e��
    std::vector<std::unique_ptr<ItemEffect>> itemFxs;   // �A�C�e���̃G�t�F�N�g
    std::unique_ptr<FireFlowerEffect> fireFlowerFx;     // �v���C���[����o��ΉԃG�t�F�N�g
    std::unique_ptr<GameSceneCamera> camera3D;          // �J����3D
    std::unique_ptr<Light> sceneLight;                  // ���C�g
    std::unique_ptr<FadeManager> holeFade;              // �R�[�X�A�E�g���̃t�F�[�h
    std::unique_ptr<ScoreUI> scoreNumUI;                // �X�R�A����UI
    std::unique_ptr<SpeedUI> speedNumUI;                // ���x����UI
    std::unique_ptr<TimeUI> timeNumUI;                  // �������Ԑ���UI
    std::unique_ptr<SpeedGaugeUI> speedGaugeUI;         // ���x�Q�[�WUI
    std::unique_ptr<Object2D> scoreUI;                  // Score�e�N�X�`��
    std::unique_ptr<Object2D> timeUI;                   // Time�e�N�X�`��
    std::unique_ptr<UVAnimation2D> speedAnim;           // ���؂艉�o�G�t�F�N�g
    std::unique_ptr<CountNumberUI> startCountUI;        // �X�^�[�g���̐���UI
    std::unique_ptr<Object2D> startUI;                  // Start�e�N�X�`��
    std::unique_ptr<Object2D> timeUpUI;                 // TimeUp�e�N�X�`��
    std::unique_ptr<Object2D> resultBg;                 // ���U���g�w�i
    std::unique_ptr<Object2D> resultUI;                 // Result�e�N�X�`��
    std::unique_ptr<Object2D> resultScoreUI;            // ���U���g�uScore�v�e�N�X�`��
    std::unique_ptr<ScoreUI> resultScoreNumUI;          // ���U���g�X�R�A����UI
    std::unique_ptr<Object2D> pressEnterKeyUI;          // PressEnterKey�e�N�X�`��
    std::unique_ptr<Object2D> newBestScoreUI;           // NewBestScore�e�N�X�`��
    CollisionManager collisionManager;                  // �����蔻���p�I�u�W�F�N�g
    Frustum frustum;                                    // �t���X�^���J�����O�p

    GAME_SCENE_STATE gameState;                         // �V�[���̏��
    std::vector<PlayerRecordData> playerRecordData;     // �v���C���[�ۑ��f�[�^�i�R�[�X�A�E�g���g�p�j
    std::vector<CameraRecordData> cameraRecordData;     // �J�����ۑ��f�[�^�i�R�[�X�A�E�g���g�p�j
    std::optional<std::reference_wrapper<ItemObject>> refTempItem; // �擾�A�C�e���̎Q�Ƃ�ۑ��i�q�b�g�X�g�b�v���g�p�j
    
    bool isCourseOut = false;                           // �v���C���[�̃R�[�X�A�E�g�t���O
    bool isHitStop = false;                             // �q�b�g�X�g�b�v����

    int itemFxIndex = 0;                                // �g�p����A�C�e���G�t�F�N�g�̃C���f�b�N�X�ԍ�
    int recordDataIndex = 0;                            // �v���C���[�ƃJ�����̕ۑ��f�[�^�̃C���f�b�N�X�ԍ�
    int restoreLocationIndexCount = 30;                 // �v���C���[�ƃJ�����̃f�[�^�𕜌�����Ƃ��Ɏg�p
    int recordDataSize = 90;                            // �ۑ��p�f�[�^�̃R���e�i�T�C�Y

    float holeReturnAddPositionY = 15.0f;               // �R�[�X�A�E�g���畜�A�����Ƃ���Y���W�̒���
    float holeJudgePositionY = -50.0f;                  // �R�[�X�A�E�g�����Y���W
    float playerSpeedRatio = 0;                         // �v���C���[�̑��x�̊���
    float baseHitStopTime = 0.15f;                      // �W���q�b�g�X�g�b�v����
    float hitStopTime = 0.0f;                           // ���m�ȃq�b�g�X�g�b�v����
    float hitStopElapsedTime = 0.0f;                    // �q�b�g�X�g�b�v�o�ߎ���
    float scoreUpRatio = 0.0f;                          // �X�R�A���Z����
    float baseScoreUpRatio = 1.0f;                      // �X�R�A���Z�W������
    float speedAnimSpeed = 10.0f;                       // ���؂�A�j���[�V�����̃A�j���[�V�������x
    float speedAnimStartRatio = 0.3f;                   // ���؂�A�j���[�V�����̊J�n���̃v���C���[���x����
    float speedAnimMaxAlpha = 0.5f;                     // ���؂�A�j���[�V�����̍ő僿�l
    const float smoothingAnimFactor = 0.05f;            // �t���[�����Ƃ̃A�j���[�V�����̕⊮
    const float fovInterpSpeed = 5.0f;
    float stageTime = 0.0f;                             // �X�e�[�W�̐�������
    float startCount = 3.0f;                            // �Q�[���J�n���̃J�E���g��
    float seTimer = 1.0f;                               // SE���Đ�����Ԋu����
    float countDownSeStartTime = 10.0f;                 // �I�����b�O�̃J�E���g�_�E�����J�n����b��
    float resultSeInterval = 0.1f;                      // ���U���g�X�R�A���ZSE�̊Ԋu

    float startUIAlpha = 1.0f;                          // Start�e�N�X�`���̃��l
    float timeUpUIAlpha = 1.0f;                         // TimeUp�e�N�X�`���̂̃��l
    float changeAlphaSpeed = 0.01f;                     // Start��TimeUp�̃��l��ύX�����鑬�x
    float timeUpUIDrawTime = 1.5f;                      // TimeUp�e�N�X�`���̕`�掞�ԁi���l�����O�j
    float resultStartWaitTime = 0.5f;                   // ���U���g�X�R�A���Z�J�n�O�̑҂�����
    float resultScoreSpeedRaito = 1.0f;                 // ���U���g�X�R�A���Z���x����
    float resultScoreMinSpeedFactor = 0.05f;            // ���U���g�X�R�A���Z�ŏ����x�W��
    float resultDrawScore = 0.0f;                       // �\������X�R�A���l
    float resultProgress = 0.0f;                        // �ڕW�l�܂ł̌o�ߊ���
    bool isResultEnd = false;                           // ���U���g�I�����}

    float ratioPhase1 = 0.0f;                           // ���x��Ԋ�����1�i�K
    float ratioPhase2 = 0.3f;                           // ���x��Ԋ�����2�i�K
    float ratioPhase3 = 0.7f;                           // ���x��Ԋ�����3�i�K
    float ratioPhase4 = 0.9f;                           // ���x��Ԋ�����4�i�K

    DirectX::XMFLOAT3 colorPhase1 = { 0.0f,1.0f,1.0f }; // ���x�����̐F ��1�i�K
    DirectX::XMFLOAT3 colorPhase2 = { 0.0f,1.0f,0.0f }; // ���x�����̐F ��2�i�K
    DirectX::XMFLOAT3 colorPhase3 = { 1.0f,0.8f,0.0f }; // ���x�����̐F ��3�i�K
    DirectX::XMFLOAT3 colorPhase4 = { 1.0f,0.0f,0.0f }; // ���x�����̐F ��4�i�K

    std::vector<Collision::ActiveBoundsData> activeBoundsData;      // �t���X�^���J�����O�Ɏg�p����I�u�W�F�N�g�R���e�i
    std::vector<Collision::ActiveBoundsData> broadPhaseBoundsData;  // �u���[�h�t�F�[�Y���Ɏg�p����I�u�W�F�N�g�R���e�i

#ifdef _DEBUG
    std::unique_ptr<DebugCamera> debugCamera3D;        // �f�o�b�O��p�J����
    bool isDebugCameraChange = false;                  // �f�o�b�O�J�����ƒʏ�J�����̐؂�ւ�
    bool isDebugDrawUI = true;                         // UI�̕\���E��\��
    bool isDebugPause = false;                         // �f�o�b�O���̈ꎞ��~
    bool isDebugFrustum = true;                        // �f�o�b�O���̃t���X�^���J�����O�̉�
    bool isDebugStepFrame = false;                     // �ꎞ��~����1�t���[���i�߂�
    bool isDebugCollisionSphereDraw = false;
    bool isDebugObjectSingleDraw = false;
    int debugDrawObjectCount = 0;                      // �`�撆�̃I�u�W�F�N�g�̐�
#endif

    // ���f�����
    typedef std::unordered_map<std::wstring, Renderer::Model> ModelHash;
    std::unique_ptr<ModelHash> modelManager;
    Renderer::Model model;

public:
    void Init() override;
    void UnInit() override;
    void Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm) override;
    // �X�^�[�g���̍X�V����
    void StartGameUpdate(float _deltaTime);
    // �Q�[���v���C���̍X�V����
    void PlayGameUpdate(float _deltaTime, GameManager& gm);
    // �^�C���A�b�v���̍X�V����
    void TimeUpUpdate(float _deltaTime);
    // ���U���g���̍X�V����
    void ResultUpdate(float _deltaTime, SceneManager& sceneManager, GameManager& gm);
    void Draw(Camera2D& _camera2D) override;
    void DrawShadowMap() override;
    // Set�֐�
    void SetIMGUI() override;
    void SetRecordDataSize(const int _size);
    // Get�֐�
    bool GetIsHitStop() { return isHitStop; }
    PlayerController& GetPlayer() { return *player; }
    // �p�����[�^�ۑ�
    void SaveParameters() override;
    // �p�����[�^�ǂݍ���
    void LoadParameters()  override;
    // �q�b�g�X�g�b�v�J�n
    void StartHitStop();
    // �q�b�g�X�g�b�v�X�V
    void UpdateHitStop(float _deltaTime);
    // �����蔻����s�֐��i�A�C�e���j
    void CheckItemObjectCollision(GameManager& gm);
    // �����蔻����s�֐��i�n�ʁA�ǁA�⓹�j
    void CheckOtherObjectCollision();
    // �A�C�e���G�t�F�N�g�̍Đ�
    void ItemEffectPlay(DirectX::XMFLOAT3 _playPos);
    // �A�C�e�����������s
    void ProcessItem();
    // �Q�[�����̑��x�̊����𔽉f
    void UpdateStageByRatio(float _deltaTime);
    // �v���C���[�ƃJ�����̏����L�^�i�R�[�X�A�E�g���g�p�j
    void UpdateRecordData();
    // �v���C���[�ƃJ�������𕜌��i�R�[�X�A�E�g���g�p�j
    void ApplyRecordData();
    // �v���C���[�̃R�[�X�A�E�g�`�F�b�N
    void PlayerHoleCheck();
    // �t���X�^���J�����O�ɂ��`��`�F�b�N
    void DrawFrustumCheck();

};