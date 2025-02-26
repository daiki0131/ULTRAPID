#include "GameScene.h"
#include "GameManager.h"
#include "CSVManager.h"
#include "Geometry.h"
#include "DebugUI.h"
#include "Fps.h"
#include "TitleScene.h"
#include "xa2.h"

using namespace DirectX;

// ����������
void GameScene::Init()
{
	// CSV����ǂݍ��񂾃f�[�^���i�[����z��
	std::vector<GameScene::MapObject> mapObjs;
	
	// �I���X�e�[�W�ɉ����ēǂݍ���CSV��ς���
	switch (GameManager::GetSelectStage())
	{
	case GameManager::STAGE_1: // �X�e�[�W1
		CSVManager::LoadCSVMapData(mapObjs, "Assets/MapData/Stage01.csv");
		break;

	case GameManager::STAGE_2: // �X�e�[�W2
		CSVManager::LoadCSVMapData(mapObjs, "Assets/MapData/Stage02.csv");
		break;

	case GameManager::STAGE_3: // �X�e�[�W3
		CSVManager::LoadCSVMapData(mapObjs, "Assets/MapData/Stage03.csv");
		break;

	default:
		break;
	}

	// �I���X�e�[�W�̐������Ԃ��擾
	stageTime = GameManager::GetStageTimeData();

	//=================================================
	//  �I�u�W�F�N�g����
	//=================================================

	// �ǂݍ��񂾃��f�����Ǘ�����z��
	modelManager = std::make_unique<ModelHash>();

	// �X�e�[�W�̊e�^�C�v�̃I�u�W�F�N�g
	std::vector<Boxs::BoxData> groundData;     // �n�ʃI�u�W�F�N�g�f�[�^���i�[����z��
	std::vector<Boxs::BoxData> wallData;       // �ǃI�u�W�F�N�g�f�[�^���i�[����z��
	std::vector<Slopes::SlopeData> slopeData;  // �⓹�I�u�W�F�N�g�f�[�^���i�[����z��
	std::vector<Items::ItemData> itemData;     // �A�C�e���I�u�W�F�N�g�f�[�^���i�[����z��

	// �I�u�W�F�N�g�ǉ���p
	Boxs::BoxData bd;
	Slopes::SlopeData sd;
	Items::ItemData id;

	XMFLOAT3 plyPosition{ 0,0,0 };
	XMFLOAT3 plyRotation{ 0,0,0 };
	XMFLOAT3 plyScaling{ 0,0,0 };

	// CSV����ǂݍ��񂾃f�[�^���^�C�v���Ƃɏ���
	for (auto& obj : mapObjs)
	{
		switch (obj.objType)
		{
		case GROUND:      // �n�ʃI�u�W�F�N�g
			bd.position = obj.position;
			bd.rotation = obj.rotation;
			bd.scaling = obj.scale;
			bd.color = XMFLOAT4(0.2f, 0.2f, 0.9f, 1.0f);
			groundData.push_back(bd);
			break;

		case WALL:        // �ǃI�u�W�F�N�g
			bd.position = obj.position;
			bd.rotation = obj.rotation;
			bd.scaling = obj.scale;
			bd.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f);
			wallData.push_back(bd);
			break;

		case SLOPE:       // �⓹�I�u�W�F�N�g
			sd.position = obj.position;
			sd.rotation = obj.rotation;
			sd.scaling = obj.scale;
			sd.color = XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f);
			slopeData.push_back(sd);
			break;

		case PLAYER:      // �v���C���[�I�u�W�F�N�g
			plyPosition = obj.position;
			plyRotation = obj.rotation;
			plyScaling = obj.scale;
			break;

		case ITEM_SMALL:    // �A�C�e����
			id.position = obj.position;
			id.rotation = obj.rotation;
			id.scaling = obj.scale;
			id.color = XMFLOAT4(colorPhase1.x, colorPhase1.y, colorPhase1.z, 1.0f);
			id.type = ItemObject::SMALL;
			itemData.push_back(id);
			break;
		case ITEM_MIDDLE:   // �A�C�e����
			id.position = obj.position;
			id.rotation = obj.rotation;
			id.scaling = obj.scale;
			id.color = XMFLOAT4(colorPhase2.x, colorPhase2.y, colorPhase2.z, 1.0f);
			id.type = ItemObject::MIDDLE;
			itemData.push_back(id);

			break;
		case ITEM_BIG:      // �A�C�e����
			id.position = obj.position;
			id.rotation = obj.rotation;
			id.scaling = obj.scale;
			id.color = XMFLOAT4(colorPhase3.x, colorPhase3.y, colorPhase3.z, 1.0f);
			id.type = ItemObject::BIG;
			itemData.push_back(id);

			break;
		case ITEM_MEGA_BIG: // �A�C�e������
			id.position = obj.position;
			id.rotation = obj.rotation;
			id.scaling = obj.scale;
			id.color = XMFLOAT4(colorPhase4.x, colorPhase4.y, colorPhase4.z, 1.0f);
			id.type = ItemObject::MEGA_BIG;
			itemData.push_back(id);

			break;
		default:
			break;
		}
	}

	// �v���C���[�I�u�W�F�N�g
	player = std::make_unique<PlayerController>(
		plyPosition,
		plyRotation,
		plyScaling,
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// 3D�J�����I�u�W�F�N�g
	camera3D = std::make_unique<GameSceneCamera>();

#ifdef _DEBUG
	// �f�o�b�O�J����
	debugCamera3D = std::make_unique<DebugCamera>();
#endif

	// ���C�g
	sceneLight = std::make_unique<Light>(XMFLOAT3(1000.0f, 800.0f, 0.0f));

	// �n�ʃI�u�W�F�N�g
	ground = std::make_unique<Boxs>(groundData);

	// �ǃI�u�W�F�N�g
	wall = std::make_unique<Boxs>(wallData);

	// �⓹�I�u�W�F�N�g
	slope = std::make_unique<Slopes>(slopeData);

	// �A�C�e���I�u�W�F�N�g
	item = std::make_unique<Items>(itemData);

	// �A�C�e���G�t�F�N�g
	itemFxs.resize(3);
	for (auto& fx : itemFxs)
	{
		fx = std::make_unique<ItemEffect>();
	}

	// �ΉԃG�t�F�N�g
	fireFlowerFx = std::make_unique<FireFlowerEffect>();

	// �R�[�X�A�E�g�̃t�F�[�h
	holeFade = std::make_unique<FadeManager>(
		XMFLOAT3(0.0f, 0.0f, -0.25f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(16.0f, 9.0f, 1.0f),
		XMFLOAT4(1, 1, 1, 0));
	
	// ScoreUI
	scoreUI = std::make_unique<Object2D>(
		XMFLOAT3(1.14f, 0.955f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.3f, 0.13125f, 0.1f),
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// �X�R�A���lUI
	scoreNumUI = std::make_unique<ScoreUI>();

	// TimeUI
	timeUI = std::make_unique<Object2D>(
		XMFLOAT3(1.15f, 1.145f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.3f, 0.13125f, 0.1f),
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// TimeUI
	timeNumUI = std::make_unique<TimeUI>();

	// ���x���lUI
	speedNumUI = std::make_unique<SpeedUI>();

	// ���x�Q�[�WUI
	speedGaugeUI = std::make_unique<SpeedGaugeUI>(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.1f, 0.1f),
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// StartUI
	startUI = std::make_unique<Object2D>(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(2.5f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �J�n�J�E���g���lUI
	startCountUI = std::make_unique<CountNumberUI>();

	// ���؂艉�o�G�t�F�N�g
	speedAnim = std::make_unique<UVAnimation2D>(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(12.0f, 7.0f, 1.0f),
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// TimeUpUI
	timeUpUI = std::make_unique<Object2D>(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(2.5f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ResultUI
	resultUI = std::make_unique<Object2D>(
		XMFLOAT3(-1.6f, 1.00f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.95f, 0.8f, 1),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ���U���g�w�i
	resultBg = std::make_unique<Object2D>(
		XMFLOAT3(0.0f, 0.0f, -0.19f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(16.0f, 9.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f));

	// ScoreUI�i���U���g�j
	resultScoreUI = std::make_unique<Object2D>(
		XMFLOAT3(-1.7f, 0.02f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ���U���g�X�R�A���lUI
	resultScoreNumUI = std::make_unique<ScoreUI>();

	// PressEnterKeyUI�i���U���g�j
	pressEnterKeyUI = std::make_unique<Object2D>(
		XMFLOAT3(1.4f, -1.0f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.7f, 0.35f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// NewBestScoreUI
	newBestScoreUI = std::make_unique<Object2D>(
		XMFLOAT3(1.4f, 0.55f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, -20.0f),
		XMFLOAT3(1.0f, 0.3f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	
	//=================================================
	//  ���f���E�e�N�X�`���ǂݍ���
	//=================================================

	// �摜�t�@�C��
	const wchar_t* textureFileName[] =
	{
		L"Assets/Texture/Time.png",
		L"Assets/Texture/floor.png",
		L"Assets/Texture/fade.png",
		L"Assets/Texture/Score.png",
		L"Assets/Texture/StartLogo.png",
		L"Assets/Texture/TimeUpLogo.png",
		L"Assets/Texture/Result.png",
		L"Assets/Texture/PressEnterKey.png",
		L"Assets/Texture/NewBestScore.png",
	};

	// �摜�ǂݍ���
	for (int i = 0; i < ARRAYSIZE(textureFileName); i++)
	{
		model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1,1 });
		model.texture = Renderer::LoadTexture(textureFileName[i]);
		modelManager->emplace(textureFileName[i], model);
	}

	// ���f���t�@�C��
	const wchar_t* modelFileName[] =
	{
		L"Assets/Model/Player.obj",
	};

	// ���f���ǂݍ���
	for (int i = 0; i < ARRAYSIZE(modelFileName); i++)
	{
		model = Renderer::LoadObjModel(modelFileName[i]);
		modelManager->emplace(modelFileName[i], model);
	}

	//=================================================
	//  ���f���Z�b�g
	//=================================================

	// �v���C���[�I�u�W�F�N�g
	model = modelManager->at(L"Assets/Model/Player.obj");
	if (model.texture)
	{
		model.texture.Reset();
	}
	player->SetModel(model);

	// ScoreUI
	model = modelManager->at(L"Assets/Texture/Score.png");
	scoreUI->SetModel(model);

	// TimeUI
	model = modelManager->at(L"Assets/Texture/Time.png");
	timeUI->SetModel(model);

	// StartUI
	model = modelManager->at(L"Assets/Texture/StartLogo.png");
	startUI->SetModel(model);

	// TimeUpUI
	model = modelManager->at(L"Assets/Texture/TimeUpLogo.png");
	timeUpUI->SetModel(model);

	// ���x�Q�[�WUI
	if (model.texture)
	{
		model.texture.Reset();
	}
	speedGaugeUI->SetModel(model);

	// ���؂艉�o�G�t�F�N�g
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1.0f, 0.166667f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/fx02.png");
	speedAnim->SetModel(model);
	
	// �R�[�X�A�E�g�t�F�[�h
	model = modelManager->at(L"Assets/Texture/fade.png");
	holeFade->SetModel(model);
	
	// ���U���g�w�i
	model = modelManager->at(L"Assets/Texture/fade.png");
	resultBg->SetModel(model);

	// ResultUI
	model = modelManager->at(L"Assets/Texture/Result.png");
	resultUI->SetModel(model);

	// ScoreUI�i���U���g�j
	model = modelManager->at(L"Assets/Texture/Score.png");
	resultScoreUI->SetModel(model);

	// PressEnterKeyUI�i���U���g�j
	model = modelManager->at(L"Assets/Texture/PressEnterKey.png");
	pressEnterKeyUI->SetModel(model);

	// NewBestScoreUI
	model = modelManager->at(L"Assets/Texture/NewBestScore.png");
	newBestScoreUI->SetModel(model);
	
	//=================================================
	//  �����ݒ�
	//=================================================

	// �v���C���[�I�u�W�F�N�g
	player->SetShader(Shader::VS_OBJECT, Shader::PS_SPECULER);
	player->SetMotionBlurShader(Shader::VS_INSTANCING, Shader::PS_SPECULER);
	player->SetBoundingSphere(player->GetPosition(), 1.0f);
	player->SetItemBoundingSphere(player->GetPosition(), 1.0f);
	player->SetSpeedRatioColorPhase(ratioPhase1, ratioPhase2, ratioPhase3, ratioPhase4);
	player->SetSpeedRatioColor(colorPhase1, colorPhase2, colorPhase3, colorPhase4);

	// �J����3D
	camera3D->SetEye({ player->GetPosition().x ,player->GetPosition().y,player->GetPosition().z - 10.0f });
	camera3D->SetRevoAngle(D2R(plyRotation.y));

	// �n�ʃI�u�W�F�N�g
	ground->SetShader(Shader::VS_INSTANCING_TEXTURE, Shader::PS_SPECULER_TEXTURE);
	ground->SetTexture(modelManager->find(L"Assets/Texture/floor.png")->second.texture);
	Renderer::Material mat;
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 10.0f);
	mat.other = XMFLOAT4(1.25f, 0.0f, 0.0f, 0.0f);
	ground->SetMaterial(mat);

	// �ǃI�u�W�F�N�g
	wall->SetShader(Shader::VS_AMBIENT_INSTANCING, Shader::PS_AMBIENT);

	// �⓹�I�u�W�F�N�g
	slope->SetShader(Shader::VS_INSTANCING_TEXTURE, Shader::PS_SPECULER);
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mat.other = XMFLOAT4(0.02f, 0.0f, 0.0f, 0.0f);
	slope->SetMaterial(mat);

	// �A�C�e���I�u�W�F�N�g
	item->SetShader(Shader::VS_INSTANCING, Shader::PS_SPECULER);
	item->SetAnimationShader(Shader::VS_ITEM_ANIMATION, Shader::PS_COLOR, Shader::GS_ITEM_ANIMATION);
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mat.other = XMFLOAT4(0.02f, 0.0f, 0.0f, 0.0f);
	item->SetMaterial(mat);
	item->SetBreakAllItemRatio(ratioPhase1, ratioPhase2, ratioPhase3, ratioPhase4);

	// �A�C�e���G�t�F�N�g
	for (auto& fx : itemFxs)
	{
		fx->SetUpParticle();
		fx->SetShader(Shader::VS_INSTANCING, Shader::PS_COLOR);
		fx->SetGravity(0.469f);
		fx->SetSize(0.05f);
		fx->SetMaxParticleNum(3000);
		fx->SetMaxLifeTime(2.416f);
		fx->SetMinLifeTime(0.6f);
		fx->SetMaxSpeed(25.0f);
		fx->SetMinSpeed(16.0f);
		fx->SetMaxAlpha(0.5f);
	}

	// �ΉԃG�t�F�N�g
	fireFlowerFx->SetUpParticle();
	fireFlowerFx->SetShader(Shader::VS_INSTANCING, Shader::PS_COLOR);
	fireFlowerFx->SetColor(XMFLOAT3(1, 1, 0));
	fireFlowerFx->SetMaxLifeTime(3.0f);
	fireFlowerFx->SetMinLifeTime(1.0f);
	fireFlowerFx->SetMaxSpeed(16.471f);
	fireFlowerFx->SetMinSpeed(9.283f);
	fireFlowerFx->SetSize(0.05f);
	fireFlowerFx->SetMaxAlpha(0.7f);
	fireFlowerFx->SetMaxParticleNum(7500);
	fireFlowerFx->SetIsPlaying(true);

	// �R�[�X�A�E�g�t�F�[�h
	holeFade->SetSpeed(1.0f);

	// ScoreUI
	scoreUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// �X�R�A���lUI
	scoreNumUI->Update(0);
	speedNumUI->Update(0.0f);
	speedNumUI->SetSpeedRatioColorPhase(ratioPhase1, ratioPhase2, ratioPhase3, ratioPhase4);
	speedNumUI->SetSpeedRatioColor(colorPhase1, colorPhase2, colorPhase3, colorPhase4);
	speedNumUI->UpdateSpeedRatioColor(0.0f);
	
	// TimeUI
	timeUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// �^�C�����lUI
	timeNumUI->Update(stageTime);

	// StartUI
	startUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	startUI->SetIsActive(false);

	// �J�n�J�E���g�_�E�����lUI
	startCountUI->SetActive(false);

	// TimeUpUI
	timeUpUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	timeUpUI->SetIsActive(false);
	
	// ���؂艉�o�G�t�F�N�g
	speedAnim->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	speedAnim->SetAnimationSpeed(speedAnimSpeed);
	speedAnim->SetAnimationTable(6);
	speedAnim->SetIsLoop(true);
	speedAnim->SetDivision(1, 6);
	speedAnim->SetColorAlpha(0.0f);
	speedAnim->Play();

	// ���x�Q�[�WUI
	speedGaugeUI->SetShader(Shader::VS_SPEED_GAUGE, Shader::PS_SPEED_GAUGE);
	speedGaugeUI->SetSpeedRatioColorPhase(ratioPhase1, ratioPhase2, ratioPhase3, ratioPhase4);
	speedGaugeUI->SetSpeedRatioColor(colorPhase1, colorPhase2, colorPhase3, colorPhase4);
	speedGaugeUI->InitGaugeColor();

	// ���U���g�w�i
	resultBg->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	resultBg->SetIsActive(false);

	// ResultUI
	resultUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	resultUI->SetIsActive(false);

	// ���U���g�X�R�A���lUI
	resultScoreNumUI->Update(0);
	resultScoreNumUI->SetActive(false);

	// ScoreUI�i���U���g�j
	resultScoreUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	resultScoreUI->SetIsActive(false);

	// PressEnterKeyUI�i���U���g�j
	pressEnterKeyUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	pressEnterKeyUI->SetIsActive(false);

	// NewBestScoreUI�i���U���g�j
	newBestScoreUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	newBestScoreUI->SetIsActive(false);

	// �R�[�X�A�E�g���g�p�̃v���C���[�f�[�^
	playerRecordData.reserve(MaxRecordDataSize);

	// �R�[�X�A�E�g���g�p�̃J����3D�f�[�^
	cameraRecordData.reserve(MaxRecordDataSize);
	SetRecordDataSize(recordDataSize);

	// �t���X�^���J�����O���Ɏg�p����
	// �I�u�W�F�N�g�̃A�N�e�B�u��Ԃ̎Q�Ƃ�ێ�����z��
	activeBoundsData.reserve(mapObjs.size());

	// �n�ʃI�u�W�F�N�g
	for (auto& ground : ground->boxs)
	{
		activeBoundsData.push_back({ ground->GetBoundingSphere(),ground->GetIsActive() });
	}

	// �ǃI�u�W�F�N�g
	for (auto& wall : wall->boxs)
	{
		activeBoundsData.push_back({ wall->GetBoundingSphere(),wall->GetIsActive() });
	}

	// �⓹�I�u�W�F�N�g
	for (auto& slope : slope->slopes)
	{
		activeBoundsData.push_back({ slope->GetBoundingSphere(),slope->GetIsActive() });
	}

	// �A�C�e���I�u�W�F�N�g
	for (auto& item : item->items)
	{
		activeBoundsData.push_back({ item->GetBoundingSphere(),item->GetIsActive() });
	}
	
	// �u���[�h�t�F�[�Y���Ɏg�p����
	// �I�u�W�F�N�g�̃A�N�e�B�u��Ԃ̎Q�Ƃ�ێ�����z��
	broadPhaseBoundsData.reserve(mapObjs.size());

	// �X�e�[�W�̃X�R�A���������i0�ɂ���j
	GameManager::InitStageScore();

	// �J��������X�V
	camera3D->Update(FPS::Get()->GetFixedDeltaTime(), *this);
}

// �������
void GameScene::UnInit()
{
	// BGM��~
	XA_Stop(RESULT_BGM);

#ifdef _DEBUG
	SaveParameters();
#endif
}

// �X�V����
void GameScene::Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm)
{
	// �Q�[���V�[���̐i�s�x�ɉ���������
	switch (gameState)
	{
	case GameScene::NO_PLAY:     // �Q�[���J�n�O
		StartGameUpdate(_deltaTime);
		break;

	case GameScene::PLAY:        // �Q�[���v���C��
		PlayGameUpdate(_deltaTime, gm);
		break;

	case GameScene::TIME_UP:     // �^�C���A�b�v��
		TimeUpUpdate(_deltaTime);
		break;

	case GameScene::RESULT:      // ���U���g
		ResultUpdate(_deltaTime, sceneManager, gm);
		break;

	case GameScene::SCENE_END:   // �Q�[���V�[���I��
		break;

	default:
		break;
	}

	sceneLight->CalculateShadowMatrix(player->GetPosition(),camera3D->GetViewMatrix());

	// �t���X�^���J�����O�`�F�b�N
	DrawFrustumCheck();


#ifdef _DEBUG
	
	// C�L�[�������ꂽ��
	// ���C���J�����ƃf�o�b�O�J������؂�ւ���
	if (Input::Get().GetKeyTrigger(DIK_C))
	{
		isDebugCameraChange = !isDebugCameraChange;
		if (isDebugCameraChange)
		{
			debugCamera3D->SetEye(camera3D->GetAfterEye());
			debugCamera3D->SetFocus(camera3D->GetFocus());
		}
	}

	// U�L�[�������ꂽ��
	// UI�\���E��\���؂�ւ�
	if (Input::Get().GetKeyTrigger(DIK_U))
	{
		isDebugDrawUI = !isDebugDrawUI;
	}

	// P�L�[�������ꂽ��
	// �v���C���̈ꎞ��~
	if (Input::Get().GetKeyTrigger(DIK_P))
	{
		isDebugPause = !isDebugPause;
	}
	// �ꎞ��~����Enter�L�[��1�t���[���i�߂�
	if (isDebugPause)
	{
		if (Input::Get().GetKeyTrigger(DIK_RETURN))
		{
			isDebugStepFrame = true;
		}
	}

	// F�L�[�������ꂽ��
	// �t���X�^���J�����OON�EOFF�؂�ւ�
	if (Input::Get().GetKeyTrigger(DIK_F))
	{
		isDebugFrustum = !isDebugFrustum;
	}

	// �f�o�b�O�J�����ɐ؂�ւ����̂�
	// �X�V���������s
	if (isDebugCameraChange)
	{
		debugCamera3D->Update(_deltaTime);
	}
#endif
}

// �Q�[���J�n�O�̍X�V����
void GameScene::StartGameUpdate(float _deltaTime)
{
	// �J�����̑��얳���ɂ���
	camera3D->SetIsInputCamera(false);

	// �t�F�[�h���I�������
	if (!SceneManager::GetFade().GetIsFade())
	{
		// StartUI��\��������
		startCountUI->SetActive(true);

		// �J�E���g�_�E���̂��߂̃^�C�����Z
		startCount -= _deltaTime;

		// 1�b���Ƃ�SE��炷���߂̉��Z
		seTimer += _deltaTime;

		// 1�b�o������
		if (seTimer > 1.0f)
		{
			// SE�Đ�
			XA_Play(COUNT_SE, CountSEVol * volRatio);
			XA_SetSpeed(COUNT_SE, StartCountSESpeed);
			seTimer = 0.0f;
		}

		// �J�E���g�_�E��UI�̍X�V
		startCountUI->SetNum(static_cast<int>(startCount) + 1);

		// �J�E���g��0�ɂȂ�����
		if (startCount < 0.0f)
		{
			// �v���C��Ԃɐ؂�ւ�
			gameState = GameScene::PLAY;

			// ������BGM���Đ�����
			XA_Play(STAGE_BGM, StageBGMVol * volRatio);

			// �X�^�[�gSE�Đ�
			XA_Play(START_SE, StartSEVol * volRatio);

			startCountUI->SetActive(false);
			startUI->SetIsActive(true);

			// �J�����𑀍�\�ɂ���
			camera3D->SetIsInputCamera(true);
		}
	}
}

// �Q�[���v���C���̍X�V����
void GameScene::PlayGameUpdate(float _deltaTime, GameManager& gm)
{
#ifdef _DEBUG
	// �ꎞ��~���ł���isDebugStepFrame��false�̂Ƃ���
	// return����
	if (isDebugPause)
	{
		if (!isDebugStepFrame)
		{
			return;
		}
		else
		{
			// false�ɂ��Ĉ�x����Update��ʂ�
			isDebugStepFrame = false;
		}
	}

#endif

	// �Q�[�����̑��x���������ꂼ��ɔ��f
	UpdateStageByRatio(_deltaTime);

	// �v���C���[�ƃJ�����̏����L�^����i�R�[�X�A�E�g���g�p�j
	UpdateRecordData();

	// �R�[�X�A�E�g������s��
	PlayerHoleCheck();

	// �q�b�g�X�g�b�v���ɍX�V����
	UpdateHitStop(_deltaTime);

	//==================================================
	// �Q�[�����I�u�W�F�N�g�̍X�V����
	//==================================================

	// �v���C���[�̑O�����ɐi�ނ��߂̕�����ݒ�
	player->SetDirAngle(camera3D->GetRevoAngle());

	// �v���C���[�X�V
	player->Update(_deltaTime);

	// �q�b�g�X�g�b�v���ł͂Ȃ��Ƃ�
	if (!isHitStop)
	{
		// �A�C�e���X�V
		item->Update(_deltaTime);

		Particle::SetCameraPosition(camera3D->GetAfterEye());
		Particle::SetFrustumPlanes(frustum.planes);

		// �A�C�e���G�t�F�N�g�X�V
		for (auto& fx : itemFxs)
		{
			fx->Update(_deltaTime);
		}

		// �ΉԃG�t�F�N�g�̐����ʒu���擾
		XMFLOAT3 spawnPos = player->GetPosition();
		spawnPos.y -= player->GetBoundingSphere().radius; // �v���C���[�̉����甭������悤�ɐݒ�
		fireFlowerFx->SetParticleSpawnPosition(spawnPos);

		// �ΉԂ̔��˂�����������߂邽��
		// �v���C���[�̐V�������W�Ɖߋ��̍��W�����ꂼ��擾
		fireFlowerFx->SetTargetNewPosition(player->GetNewPosition());
		fireFlowerFx->SetTargetOldPosition(player->GetOldPosition());

		// �ΉԃG�t�F�N�g�X�V
		fireFlowerFx->Update(_deltaTime);
	}

	// �J�����̃|�W�V�����擾
	XMFLOAT3 camPos = camera3D->GetAfterEye();
	// �������ǂ��J�������牓�����Ƀ\�[�g
	std::sort(wall->boxs.begin(), wall->boxs.end(), [&](const std::unique_ptr<BoxObject>& a, const std::unique_ptr<BoxObject>& b)
	{
		return a->DistanceFromCamera(camPos) > b->DistanceFromCamera(camPos);
	});

	// �R�[�X�A�E�g�t�F�[�h�X�V
	holeFade->Update(_deltaTime);

	// �n�ʁE�ǁE�⓹�Ƃ̓����蔻������s
	CheckOtherObjectCollision();

	// �A�C�e���I�u�W�F�N�g�Ƃ̓����蔻������s
	CheckItemObjectCollision(gm);

	// �X�R�AUI�X�V
	scoreNumUI->Update(gm.GetStageScore());

	// ���xUI�X�V
	speedNumUI->Update(player->GetRealSpeed());

	// ���؂�G�t�F�N�g�X�V
	speedAnim->Update(_deltaTime);

	//==================================================
	//  �X�e�[�W�̐������Ԃ��Ǘ�
	//==================================================
	if (!isHitStop)
	{
		stageTime -= _deltaTime; // �������Ԃ����Z
	}
	
	// �������Ԃ�0�ɂȂ�����
	if (stageTime < 0.0f)
	{
		stageTime = 0.0f;
		gameState = GameScene::TIME_UP;  // �i�s��Ԃ��^�C���A�b�v�ɐ؂�ւ�
		timeUpUI->SetIsActive(true);

		// BGM�ESE�̍Đ��E��~
		XA_Play(TIME_UP_SE, TimeUpSEVol* volRatio);
		XA_Stop(STAGE_BGM);
		XA_Stop(CHARGE_SE);
		XA_Stop(MOVE_SE);

		// �h���t�g�̃G�t�F�N�g���~�߂�
		player->SetIsDriftingEffectPlay(false);
	}
	// �������Ԃ��c��10�b�ɂȂ�����
	else if (stageTime <= countDownSeStartTime)
	{
		// 1�b���Ƃ�SE���Đ�
		seTimer += _deltaTime;
		if (seTimer > 1.0f)
		{
			XA_Play(COUNT_SE, CountSEVol * volRatio);
			seTimer = 0;
		}
	}

	// �^�C��UI�X�V
	timeNumUI->Update(stageTime);

	//==================================================
	//  �Q�[���X�^�[�g����̏���
	//==================================================
	if (startUI->GetIsActive())
	{
		// StartUI�̃��l�����X�ɉ�����
		startUI->SetColorAlpha(startUIAlpha);
		startUIAlpha -= changeAlphaSpeed;
		
		// ���S��0�ɂȂ�����
		if (startUIAlpha < 0.0f)
		{
			startUI->SetIsActive(false);
		}
	}

	// �J�����̍X�V
	camera3D->Update(_deltaTime, *this);
}

// �^�C���A�b�v���̍X�V����
void GameScene::TimeUpUpdate(float _deltaTime)
{
	// TimeUpUI�̕`�掞�Ԃ�0�ȏ�̏ꍇ
	if (timeUpUIDrawTime > 0.0f)
	{
		timeUpUIDrawTime -= _deltaTime; // ���Ԃ����Z

		// ���S��0�ɂȂ�����
		if (timeUpUIDrawTime < 0.0f)
		{
			// �eUI�̔�\���ݒ�
			timeUI->SetIsActive(false);
			scoreUI->SetIsActive(false);
			timeNumUI->SetActive(false);
			scoreNumUI->SetActive(false);
			speedNumUI->SetActive(false);
			speedGaugeUI->SetIsActive(false);
		}
	}
	// TimeUpUI�̃��l�����X�ɉ�����
	else if (timeUpUI->GetColor().w > 0.0f)
	{
		timeUpUIAlpha -= changeAlphaSpeed;
		timeUpUI->SetColorAlpha(timeUpUIAlpha);
	}
	// ���l�����S��0�ɂȂ�����
	else
	{
		// ���U���g��ʂ̕\��
		resultUI->SetIsActive(true);
		resultBg->SetIsActive(true);
		resultScoreUI->SetIsActive(true);
		resultScoreNumUI->SetActive(true);

		// �i�s��Ԃ����U���g�ɐ؂�ւ�
		gameState = GameScene::RESULT;

		// ���U���g��BGM�Đ�
		XA_Play(RESULT_BGM, ResultBGMVol * volRatio);
	}
}

// ���U���g�̍X�V
void GameScene::ResultUpdate(float _deltaTime, SceneManager& sceneManager, GameManager& gm)
{
	// �l���X�R�A���擾����
	float resultScore = static_cast<float>(gm.GetStageScore());

	// �X�R�A�����Z����܂ł̑ҋ@����
	if (resultStartWaitTime > 0.0f)
	{
		resultStartWaitTime -= _deltaTime;
	}
	// �X�R�A���Z���̉��o
	else if(resultDrawScore < resultScore && !isResultEnd)
	{
		// �ڕW�l�������قǉ��Z���x���㏸������
		float speedFactor = 1.0f - resultProgress;

		// ���x���x���Ȃ肷���Ȃ��悤�ɒ�������
		if (speedFactor < resultScoreMinSpeedFactor)
		{
			speedFactor = resultScoreMinSpeedFactor;
		}

		// �ڕW�l�܂ł̌o�ߊ��������Z����
		resultProgress += resultScoreSpeedRaito * _deltaTime * speedFactor;
		// 1.0�𒴂��Ȃ��悤�ɒ���
		resultProgress = std::clamp(resultProgress, 0.0f, 1.0f);
		
		// ���ݕ`�撆�̃X�R�A���i�[
		float nowResultScore = resultDrawScore;

		// �\������X�R�A���X�V
		resultDrawScore = resultScore * resultProgress;


		bool isPlaySE = false; // SE���Đ�����

		// ���t���[����̂�h�����߂�
		// ���̊Ԋu���Ƃ�SE���Đ�����
		if (seTimer < resultSeInterval)
		{
			seTimer += _deltaTime;
		}
		else
		{
			isPlaySE = true;
		}

		// �O��ƃX�R�A���Ⴄ�ꍇ����SE���Đ��ł���ꍇ
		if ((static_cast<int>(nowResultScore) != static_cast<int>(resultDrawScore)) && isPlaySE)
		{
			XA_Play(ADD_SCORE_SE, AddScoreSEVol * volRatio);
			seTimer = 0;
		}

		// Enter�L�[�܂��̓Q�[���p�b�hA�������ꂽ��
		// ���Z���̃X�R�A����C�ɖڕW�X�R�A�ɂ���
		if (Input::Get().GetKeyTrigger(DIK_RETURN) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_A))
		{
			XA_Play(DECISION_SE, DecisionSEVol * volRatio);
			resultDrawScore = resultScore;
		}
		
		// �ڕW�X�R�A�ɒB������
		if (resultDrawScore >= resultScore)
		{
			resultDrawScore = resultScore;
			pressEnterKeyUI->SetIsActive(true);
			
			// �����̃X�R�A���X�V���ꂽ������
			if (gm.IsHighScoreUpdated())
			{
				// �X�R�A���X�V���ꂽ�ꍇUI��\��
				newBestScoreUI->SetIsActive(true);
			}

			isResultEnd = true; // ���U���g�I��
		}

		// ���U���g�X�R�AUI�̍X�V
		resultScoreNumUI->Update(static_cast<int>(resultDrawScore));
		
	}
	else
	{
		// Enter�L�[�܂��̓Q�[���p�b�hA�������ꂽ��
		if (Input::Get().GetKeyTrigger(DIK_RETURN) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_A))
		{
			XA_Play(DECISION_SE, DecisionSEVol * volRatio);

			// TitleScene�ɐ؂�ւ���
			sceneManager.SetScene(std::make_unique<TitleScene>());

			// �i�s��Ԃ��Q�[���I���ɂ���
			gameState = GameScene::SCENE_END;
		}
	}
}

// �v���C���[�̑��x���������ꂼ��ɔ��f������
void GameScene::UpdateStageByRatio(float _deltaTime)
{
	// �v���C���[�̑��x�������擾
	playerSpeedRatio = player->GetSpeedRatio();

	// �X�R�A�̉��Z�ʂ̒���
	scoreUpRatio = baseScoreUpRatio * (1.0f + playerSpeedRatio);
	
	// �J�����̎���p�𒲐�
	float cameraFovValue = 
		GameSceneCamera::MinFov 
		+ (GameSceneCamera::MaxFov - GameSceneCamera::MinFov) 
		* playerSpeedRatio;

	// ���݂̎���p���擾
	float nowFov = camera3D->GetFovAngle();

	// �}�����ɂ��}�Ȏ���p�̕ω���h�����ߕ⊮����
	float interpSpeed = fovInterpSpeed * _deltaTime; // ��ԑ��x
	float newFov = nowFov + (cameraFovValue - nowFov) * interpSpeed;

	camera3D->SetFovAngle(newFov);

	// �q�b�g�X�g�b�v�̎��Ԃ𒲐�
	// �ő��t�߂ɂȂ�΂Ȃ�قǂق�0�ɂȂ�
	if (playerSpeedRatio < ratioPhase4)
	{
		hitStopTime = baseHitStopTime * (ratioPhase4 - playerSpeedRatio);
	}
	else
	{
		hitStopTime = 0.0f;
	}
	
	// �A�C�e���I�u�W�F�N�g�̔j��\�E�s��Ԃ�؂�ւ���
	item->SetRealRatio(playerSpeedRatio);

	// �ΉԃG�t�F�N�g�̗��q�̐��𒲐�
	fireFlowerFx->SetTargetSpeedRatio(playerSpeedRatio);
	
	// �A�C�e���I�u�W�F�N�g�Ɠ����蔻�莞�̔���̈�𒲐�
	Collision::BoundingSphere playerBs1 = player->GetBoundingSphere();
	Collision::BoundingSphere& playerBs2 = player->GetItemBoundingSphere();

	float collisionRatio = 0.0f;
	if (playerSpeedRatio > ratioPhase3)
	{
		collisionRatio = (playerSpeedRatio - ratioPhase3) / (1.0f - ratioPhase3);
	}
	playerBs2.radius = playerBs1.radius + (10.0f * collisionRatio);

	// ���xUI�̐F��ς���
	speedNumUI->UpdateSpeedRatioColor(playerSpeedRatio);

	// �����ɉ������Q�[�W�𔽉f������
	speedGaugeUI->SetRatio(playerSpeedRatio);

	// ���؂�G�t�F�N�g�̃��l�𑬓x�𒲐�����
	float targetAnimAlpha = (playerSpeedRatio - speedAnimStartRatio) / (1.0f - speedAnimStartRatio) * speedAnimMaxAlpha;
	targetAnimAlpha = std::clamp(targetAnimAlpha, 0.0f, speedAnimMaxAlpha);
	float nowAlpha = speedAnim->GetColor().w;
	nowAlpha = nowAlpha + (targetAnimAlpha - nowAlpha) * smoothingAnimFactor;
	// ���l�Z�b�g
	speedAnim->SetColorAlpha(nowAlpha);
	// ���x�Z�b�g
	speedAnim->SetAnimationSpeed(speedAnimSpeed * (1.0f + playerSpeedRatio));
}

// �L�^�p�z��̑傫���ύX
void GameScene::SetRecordDataSize(const int _size)
{
	playerRecordData.resize(_size);
	cameraRecordData.resize(_size);
}

// �v���C���[�ƃJ�����̋L�^���s��
void GameScene::UpdateRecordData()
{
	// �v���C���[���n�ʂɂ���Ƃ��ɋL�^���s��
	if (player->GetIsGround())
	{
		// �v���C���[���
		PlayerRecordData prd;
		prd.position = player->GetPosition();        // �ʒu
		prd.oldPosition = player->GetOldPosition();  // �ߋ��̍��W
		prd.angle = player->GetAngle();              // �p�x

		// �J�������
		CameraRecordData crd;
		crd.eye = camera3D->GetEye();                // �ʒu
		crd.angle = camera3D->GetRevoAngle();        // ��]�p�x

		// �e�z��Ɋi�[����
		if (recordDataIndex < recordDataSize)
		{
			playerRecordData[recordDataIndex] = prd;
			cameraRecordData[recordDataIndex] = crd;
		}
		recordDataIndex++; // �C���f�b�N�X�J�E���g��i�߂�

		// �z��̃T�C�Y�ɒB������C���f�b�N�X�J�E���g��0�ɂ���
		if (recordDataIndex >= recordDataSize)
		{
			recordDataIndex = 0;
		}
	}
}

// �L�^�f�[�^�𔽉f������
void GameScene::ApplyRecordData()
{
	// ���x�ɂ���Ăǂ��܂Ŗ߂����𒲐�
	// ���A���Ă����ɗ����Ă��܂��̂�h��
	float clampedValue = 
		restoreLocationIndexCount
		+ (recordDataSize - restoreLocationIndexCount)
		* playerSpeedRatio;

	// �ڕW�̃J�E���g���v�Z
	int targetIndex = (recordDataIndex - static_cast<int>(clampedValue) + recordDataSize) % recordDataSize;

	// �������v���C���[��������̂��畜������
	// ���Ԃ��k���Ă���悤�Ɍ����Ȃ�����
	playerRecordData[recordDataIndex].position.y += holeReturnAddPositionY;
	playerRecordData[recordDataIndex].oldPosition.y += holeReturnAddPositionY;

	// �v���C���[�̉ߋ��̏��𔽉f
	player->SetPosition(playerRecordData[recordDataIndex].position);
	player->SetOldPosition(playerRecordData[recordDataIndex].oldPosition);
	player->SetAngle(playerRecordData[recordDataIndex].angle);

	// �R�[�X�A�E�g��̌�������
	player->HoleAfterSet();
	
	// �J�����̉ߋ��̏��𔽉f
	cameraRecordData[recordDataIndex].eye.y += holeReturnAddPositionY;
	camera3D->SetEye(cameraRecordData[recordDataIndex].eye);
	camera3D->SetRevoAngle(cameraRecordData[recordDataIndex].angle);
}

// �v���C���[�̃R�[�X�A�E�g�`�F�b�N
void GameScene::PlayerHoleCheck()
{
	// �R�[�X�A�E�g���Ă��Ȃ��Ƃ�
	if (!isCourseOut)
	{
		// ���̍��W��艺��������
		if (player->GetPosition().y < holeJudgePositionY)
		{
			// �J�����̒Ǐ]����߂�
			camera3D->SetIsChase(false);

			// �R�[�X�A�E�g��p�̃t�F�[�h�J�n
			holeFade->StartFadeOut();
			isCourseOut = true;
		}
	}
	else
	{
		// �t�F�[�h�A�E�g���I��������
		if (!holeFade->GetIsFade())
		{
			// �t�F�[�h�C���J�n
			holeFade->StartFadeIn();

			// �J�����̒Ǐ]�ĊJ
			camera3D->SetIsChase(true);

			// �߂��ʒu�̏����v���C���[�ƃJ�����ɔ��f
			ApplyRecordData();
			isCourseOut = false;
		}
	}
}

// �q�b�g�X�g�b�v�J�n
void GameScene::StartHitStop()
{
	if (playerSpeedRatio <= ratioPhase4)
	{
		isHitStop = true;
		player->SetIsHitStop(true);
		camera3D->SetIsHitStop(true);

		// �J������h�炷
		camera3D->StartCameraShake(hitStopTime, playerSpeedRatio);
	}
	else
	{
		ProcessItem();
	}
	
}

// �q�b�g�X�g�b�v���̍X�V����
void GameScene::UpdateHitStop(float _deltaTime)
{
	if (!isHitStop) return;

	// �o�ߎ��Ԃ����Z
	hitStopElapsedTime += _deltaTime;

	// ��莞�Ԃ��o�߂�����
	// �q�b�g�X�g�b�v��~����
	if (hitStopElapsedTime > hitStopTime)
	{
		hitStopElapsedTime = 0;
		isHitStop = false;
		player->SetIsHitStop(false);
		camera3D->SetIsHitStop(false);

		ProcessItem();
	}
}

// �`�揈��
void GameScene::Draw(Camera2D& _camera2D)
{

	// 3D�J�����Z�b�g
	Renderer::SetCameraViewProjection(camera3D->GetViewMatrix(), camera3D->GetProjectionMatrix());

#ifdef _DEBUG
	// �f�o�b�O�J�����؂�ւ���
	if (isDebugCameraChange)
	{
		Renderer::SetCameraViewProjection(debugCamera3D->GetViewMatrix(), debugCamera3D->GetProjectionMatrix());
	}
#endif

	// �f�t�H���g�̐[�x�X�e���V���X�e�[�g
	Renderer::GetContext()->OMSetDepthStencilState(nullptr, 1);

#ifdef _DEBUG
	if (isDebugObjectSingleDraw)
	{
		ground->DebugSingleDraw();
		item->DebugSingleDraw();
	}
	else
	{
		// �n�ʃI�u�W�F�N�g
		ground->InstancingWithShadowsDraw();
		// �A�C�e���I�u�W�F�N�g
		item->InstancingDraw();
	}

#else
	// �n�ʃI�u�W�F�N�g
	ground->InstancingWithShadowsDraw();
	// �A�C�e���I�u�W�F�N�g
	item->InstancingDraw();
#endif
	
	// �⓹�I�u�W�F�N�g
	slope->InstancingDraw();

	// �v���C���[
	player->Draw();

	// �J�X�^���̐[�x�X�e���V���X�e�[�g
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState().Get(), 1);

#ifdef _DEBUG
	if (isDebugObjectSingleDraw)
	{
		wall->DebugSingleDraw();
	}
	else
	{
		// �ǃI�u�W�F�N�g
		wall->InstancingWithoutShadowsDraw();
	}

#else
	// �ǃI�u�W�F�N�g
	wall->InstancingWithoutShadowsDraw();
#endif
	

	// �v���C���[�i���[�V�����u���[�j
	player->MortionBlurDraw();

	// �v���C���[�i�h���t�g�G�t�F�N�g�j
	player->DriftingEffectDraw();

	// �v���C���[�i�\�j�b�N�u�[���j
	player->SonicBoomDraw();

	// �A�C�e���i���U�A�j���[�V�����j
	item->ExplosionAnimation();

	// �p�[�e�B�N���G�t�F�N�g
	for (auto& fx : itemFxs)
	{
		fx->Draw();
	}
	fireFlowerFx->Draw();

#ifdef _DEBUG
	if (isDebugCollisionSphereDraw)
	{
		ground->DebugSphereDraw();
		wall->DebugSphereDraw();
		slope->DebugSphereDraw();
	}


	if (!isDebugDrawUI) return;
#endif

	// 2D�J�����Z�b�g
	Renderer::SetCameraViewProjection(_camera2D.GetViewMatrix(), _camera2D.GetProjectionMatrix());
	
	// ���؂�G�t�F�N�g
	speedAnim->Draw();

	// �R�[�X�A�E�g�t�F�[�h
	holeFade->Draw();

	// �X�R�A���lUI
	scoreNumUI->Draw();

	// ScoreUI
	scoreUI->Draw();

	// ���x���lUI
	speedNumUI->Draw();

	// ���x�Q�[�WUI
	speedGaugeUI->Draw();

	// �������Ԑ��lUI
	timeNumUI->Draw();

	// TimeUI
	timeUI->Draw();

	// �J�n���̃J�E���g���lUI
	startCountUI->Draw();

	// StartUI
	startUI->Draw();

	// TimeUpUI
	timeUpUI->Draw();

	// ���U���g�w�i
	resultBg->Draw();

	// ResultUI
	resultUI->Draw();

	// ScoreUI�i���U���g�j
	resultScoreUI->Draw();

	// �X�R�A���lUI�i���U���g�j
	resultScoreNumUI->Draw();

	// NewBestScoreUI
	newBestScoreUI->Draw();

	// PressEnterKeyUI
	pressEnterKeyUI->Draw();
}

// �V���h�E�}�b�v�쐬�p�̕`�揈��
void GameScene::DrawShadowMap()
{
	// ���C�g�̃r���[�E�v���W�F�N�V�����s����v�Z
	Renderer::SetLightViewProjection(sceneLight->GetViewProjection());

	// �J�X�^���̐[�x�X�e���V���X�e�[�g
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetShadowDepthStencilState().Get(), 1);
	
	// �v���C���[
	player->DrawShadowMap();

	// �A�C�e��
	item->DrawInstancingShadowMap();

	// ��
	wall->DrawInstancingShadowMap();

	// �n��
	ground->DrawInstancingShadowMap();
}

// �t���X�^���J�����O���菈��
void GameScene::DrawFrustumCheck()
{
	// �r���[�E�v���W�F�N�V�����s�񂩂畽�ʂ��v�Z
	frustum.ConstructFrustum(camera3D->GetViewProjectionMatrix());
	
	// �t���X�^���ƕ`��Ώۂ̑S�I�u�W�F�N�g��
	// �R���s���[�g�V�F�[�_�[�Ōv�Z����
	Collision::FrustumCheck(frustum.planes, activeBoundsData);

#ifdef _DEBUG
	// �f�o�b�O���̂݃t���X�^���J�����O��؂�ւ��\
	if (!isDebugFrustum)
	{
		for (auto& obj : activeBoundsData)
		{
			obj.activeState.get() = true;
		}
	}

	debugDrawObjectCount = 0;
	for (auto& obj : activeBoundsData)
	{
		if (obj.activeState)
		{
			debugDrawObjectCount++;
		}
	}
#endif
}

// �A�C�e���I�u�W�F�N�g�Ƃ̓����蔻�菈��
void GameScene::CheckItemObjectCollision(GameManager& gm)
{
	// �A�C�e���I�u�W�F�N�g�̃o�E���f�B���O�X�t�B�A�̔z��
	std::vector<Collision::BoundingSphere> itemBS;

	// �v���C���[�̎Q�Ƃ��擾
	auto playerPtr = dynamic_cast<Player*>(player.get());

	// �q�b�g�X�g�b�v���ł͂Ȃ��Ƃ�
	if (isHitStop && !playerPtr) return;
	
	for (auto& item : item->items)
	{
		// ����Ώۂ܂��͐G��邱�Ƃ��\�ȏꍇ
		if (!item->GetIsHitCheke() || !item->GetIsTouched()) continue;

		// �����蔻����s
		if (collisionManager.SphereCollisionPlayerAndItem(*playerPtr, *item))
		{
			// �Q�Ƃ����Ă����i�q�b�g�X�g�b�v��ɃA�C�e���̏��������s���邽�߁j
			refTempItem = *item;

			// �X�R�A���Z
			gm.AddStageScore(static_cast<int>(static_cast<float>(item->GetScore()) * scoreUpRatio));

			// �q�b�g�X�g�b�v�J�n
			StartHitStop();
		}
	}
}

// �n�ʁE�ǁE�⓹�I�u�W�F�N�g�Ƃׂ̍��������蔻�菈��
void GameScene::CheckOtherObjectCollision()
{
	//=============================================================//
	// �u���[�h�t�F�[�Y�ŋ��̓��m�Ŕ���
	//=============================================================//
	Collision::BoundingSphere pbs = player->GetBoundingSphere();
	// �v���C���[�̍ŐV���W��ݒ�
	pbs.center = player->GetNewPosition();

	broadPhaseBoundsData.clear();

	// �n�ʃI�u�W�F�N�g
	for (auto& ground : ground->boxs)
	{
		if (!ground->GetIsActive()) continue;

		broadPhaseBoundsData.push_back({ ground->GetBoundingSphere(), ground->GetIsCollisionCheck() });
	}

	// �ǃI�u�W�F�N�g
	for (auto& wall : wall->boxs)
	{
		if (!wall->GetIsActive()) continue;

		wall->SetFirstColor();

		broadPhaseBoundsData.push_back({ wall->GetBoundingSphere(), wall->GetIsCollisionCheck() });
	}

	// �⓹�I�u�W�F�N�g
	for (auto& slope : slope->slopes)
	{
		if (!slope->GetIsActive()) continue;

		broadPhaseBoundsData.push_back({ slope->GetBoundingSphere(), slope->GetIsCollisionCheck() });
	}

	Collision::BroadPhaseCheck(pbs, broadPhaseBoundsData);


	//=============================================================//
	// �i���[�t�F�[�Y�ōׂ�������
	//=============================================================//

	std::vector<Collision::AABB> groundsAABB;               // �n�ʐ�pAABB
	std::vector<Collision::AABB> wallsAABB;                 // �ǐ�pAABB
	std::vector<Collision::Triangle> slopesTriangle;        // �⓹��p�O�p�`����
	// �ǂ̂ݐڐG���Ă���ƐԂ��F��ς���̂ŎQ�ƂƂ��ēn��
	std::vector<std::reference_wrapper<BoxObject>> walls;  // �ǐ�p�̎Q�ƃR���e�i
	
	// �n�ʃI�u�W�F�N�g
	for (auto& ground : ground->boxs)
	{
		if (!ground->GetIsCollisionCheck()) continue;

		groundsAABB.push_back(ground->GetAABB());
	}
	// �ǃI�u�W�F�N�g
	for (auto& wall : wall->boxs)
	{
		if (!wall->GetIsCollisionCheck()) continue;

		wallsAABB.push_back(wall->GetAABB());
		walls.push_back(*wall);
	}
	// �⓹�I�u�W�F�N�g
	for (auto& slope : slope->slopes)
	{
		if (!slope->GetIsCollisionCheck()) continue;

		std::vector<Collision::Triangle> triangles = slope->GetPlanes();
		slopesTriangle.insert(slopesTriangle.end(), triangles.begin(), triangles.end());
	}

	// �v���C���[���ړ���̍��W�ɏՓ˂��Ă��Ȃ��ꍇ
	// �v���C���[�𐳎��ȍ��W�Ɉړ������邽�߂̓����蔻��
	collisionManager.MovePlayerCollisionCheck(player, groundsAABB, wallsAABB, slopesTriangle);

	// �v���C���[���W�����v�ł���悤�ɂ��邽�߂̓����蔻��
	if (collisionManager.JumpPlayerCollisionCheck(player, groundsAABB, wallsAABB, slopesTriangle))
	{
		// 1�ȏ�̒n�ʂɂ��Ă���ꍇ�W�����v�\�ɂ���
		player->SetIsGround(true);
		player->SetJumpState(PlayerController::CAN_JUMP);
		player->SetVelocityY(0.0f);
		fireFlowerFx->SetIsCanSpawn(true);
	}
	else
	{
		// 1�ł��G��Ă��Ȃ���΃W�����v���Ȃ�
		player->SetIsGround(false);
		fireFlowerFx->SetIsCanSpawn(false);
	}

	// �v���C���[���߂荞�񂾏ꍇ�̔��菈���E����
	collisionManager.ExecuteCollisionResponse(player, groundsAABB, walls, slopesTriangle);
}

// �A�C�e���G�t�F�N�g�Đ�
void GameScene::ItemEffectPlay(DirectX::XMFLOAT3 _playPos)
{
	// �z��̃C���f�b�N�X�����Ԃɐi�߂čĐ�����
	if (itemFxIndex < itemFxs.size())
	{
		itemFxs[itemFxIndex]->ParticlePlay(_playPos);
	}
	else
	{
		itemFxIndex = 0;
		itemFxs[0]->ParticlePlay(_playPos);
	}
	itemFxIndex++;

}

void GameScene::ProcessItem()
{
	// �Փ˂����A�C�e���̏���������
	refTempItem->get().SetState(ItemObject::EXPLOSION);
	refTempItem->get().SetFragmentAlpha(0.5f);
	ItemEffectPlay(refTempItem->get().GetPosition()); // �G�t�F�N�g�Đ�
	refTempItem.reset(); // �Q�Ƃ�����

	// SE�Đ�
	XA_Play(BALLOON_SE, ItemSEVol * volRatio);
}

// IMGUI�̐ݒ�
void GameScene::SetIMGUI()
{
	float fNum;
	XMFLOAT3 fNum3;
	//=================================================
	//  GameScene
	//=================================================
	ImGui::Begin("ScenePrameter");
	ImGui::Text("HitStopTime (%.7f)", hitStopTime);

#ifdef _DEBUG
	ImGui::Text("DrawObjectCount (%d)", debugDrawObjectCount);
	ImGui::Checkbox("CollisionSphereDraw", &isDebugCollisionSphereDraw);
	if (ImGui::Checkbox("DebugCameraChange", &isDebugCameraChange))
	{
		if (isDebugCameraChange)
		{
			debugCamera3D->SetEye(camera3D->GetAfterEye());
			debugCamera3D->SetFocus(camera3D->GetFocus());
		}
	}
	ImGui::Checkbox("DebugDrawUI", &isDebugDrawUI);
	ImGui::Checkbox("DebugPause", &isDebugPause);
	ImGui::Checkbox("DebugFrustum", &isDebugFrustum);
	ImGui::Checkbox("DebugObjectSingleDraw", &isDebugObjectSingleDraw);

#endif
	
	ImGui::End();

	//=================================================
	//  FPS
	//=================================================
	ImGui::Begin("FPS");
	ImGui::Text("FrameRate (%.3f FPS)", FPS::Get()->GetFPS());
	ImGui::End();

	//=================================================
	//  �v���C���[
	//=================================================
	ImGui::Begin("PlayerParameter");
	ImGui::Text("SpeedPerHour (%.3f km/h)", player->GetRealSpeed() * 3.60f);
	ImGui::Text("AccelerationPhase %d", player->GetAccelerationsIndex() + 1);
	ImGui::Text("PlayerPosition  x: %.1f y: %.1f z: %.1f", player->GetPosition().x, player->GetPosition().y, player->GetPosition().z);
	ImGui::Text("ItemBoundingRadius %.2f", player->GetItemBoundingSphere().radius);

	std::vector<float> accelerations = player->GetAccelerations();

	for (int i = 0; i < accelerations.size(); i++)
	{
		std::string str = "Accelerations";
		str += std::to_string(i + 1);

		fNum = accelerations[i];

		if (ImGui::SliderFloat(str.c_str(), &fNum, 0.0f, PlayerController::MaxAcceleration))
		{
			accelerations[i] = fNum;

			player->SetAccelerations(accelerations);
		}
	}

	fNum = player->GetMaxMoveSpeed();
	if (ImGui::SliderFloat("MaxMoveSpeed", &fNum, 0.01f, PlayerController::MaxMoveSpeed))
	{
		player->SetMaxMoveSpeed(fNum);
		speedNumUI->SetMaxSpeed(fNum);
	}

	fNum = player->GetMaxDriftAddSpeed();
	if (ImGui::SliderFloat("MaxDriftAddSpeed", &fNum, 0.01f, PlayerController::MaxDriftAddSpeed))
	{
		player->SetMaxDriftAddSpeed(fNum);
	}

	fNum = player->GetMass();
	if (ImGui::SliderFloat("Mass", &fNum, 1.0f, PlayerController::MaxMass))
	{
		player->SetMass(fNum);
	}

	fNum = player->GetJumpPower();
	if (ImGui::SliderFloat("JumpPower", &fNum, 1.0f, PlayerController::MaxJumpPower))
	{
		player->SetJumpPower(fNum);
	}

	fNum = player->GetMinTurnRate();
	if (ImGui::SliderFloat("MinTurnRate", &fNum, 0.0f, PlayerController::TurnRateMaxNum))
	{
		if (player->GetMaxTurnRate() < fNum) fNum = player->GetMaxTurnRate() - 0.01f;

		player->SetMinTurnRate(fNum);
	}

	fNum = player->GetMaxTurnRate();
	if (ImGui::SliderFloat("MaxTurnRate", &fNum, 0.0f, PlayerController::TurnRateMaxNum))
	{
		if (player->GetMinTurnRate() > fNum) fNum = player->GetMinTurnRate() + 0.01f;

		player->SetMaxTurnRate(fNum);
	}

	fNum = player->GetFrictionFactor();
	if (ImGui::SliderFloat("FrictionFactor", &fNum, 0.0f, PlayerController::MaxFrictionFactor))
	{
		player->SetFrictionFactor(fNum);
	}

	fNum = player->GetWallFrictionFactor();
	if (ImGui::SliderFloat("WallFrictionFactor", &fNum, 0.0f, PlayerController::MaxFrictionFactor))
	{
		player->SetWallFrictionFactor(fNum);
	}

	ImGui::End();

	//=================================================
	//  �J����
	//=================================================
	ImGui::Begin("Camera3DParameter");
	ImGui::Text("Eye  x: %.1f y: %.1f z: %.1f", camera3D->GetEye().x, camera3D->GetEye().y, camera3D->GetEye().z);
	ImGui::End();

	//=================================================
	//  �p�[�e�B�N��
	//=================================================
	ImGui::Begin("ParticleParameter");
	ImGui::Text("FireFlowerParticleNum  %d / %d", fireFlowerFx->GetRealParticleNum(),fireFlowerFx->GetMaxParticleNum());
	ImGui::End();

	//=================================================
	//  UI
	//=================================================

	ImGui::Begin("UIParameter");

	// �X�R�A���lUI
	fNum3 = scoreNumUI->GetParentPosition();
	if (ImGui::SliderFloat2("ScoreNumUIPosition", &fNum3.x, -3.0f, 3.0f))
	{
		scoreNumUI->SetParentPosition(fNum3);
	}

	fNum3 = scoreNumUI->GetParentScale();
	if (ImGui::SliderFloat2("ScoreNumUIScale", &fNum3.x, 0.1f, 3.0f))
	{
		scoreNumUI->SetParentScale(fNum3);
	}

	// ScoreUI
	fNum3 = scoreUI->GetPosition();
	if (ImGui::SliderFloat2("ScoreUITexturePosition", &fNum3.x, -Object2D::MaxTexPos, Object2D::MaxTexPos))
	{
		scoreUI->SetPosition(fNum3);
		scoreUI->ReCalculateWorldMatrix();
	}

	fNum3 = scoreUI->GetScale();
	if (ImGui::SliderFloat2("ScoreUITextureScale", &fNum3.x, 0.1f, Object2D::MaxTexScale))
	{
		scoreUI->SetScale(fNum3);
		scoreUI->ReCalculateWorldMatrix();
	}


	// ���x���lUI
	fNum3 = speedNumUI->GetParentPosition();
	if (ImGui::SliderFloat2("SpeedNumUIPosition", &fNum3.x, -3.0f, 3.0f))
	{
		speedNumUI->SetParentPosition(fNum3);
	}

	fNum3 = speedNumUI->GetParentScale();
	if (ImGui::SliderFloat2("SpeedNumUIScale", &fNum3.x, 0.1f, 3.0f))
	{
		speedNumUI->SetParentScale(fNum3);
	}

	// ���x�Q�[�WUI
	fNum3 = speedGaugeUI->GetPosition();
	if (ImGui::SliderFloat2("SpeedGaugeUIPosition", &fNum3.x, -3.0f, 3.0f))
	{
		speedGaugeUI->SetPosition(fNum3);
		speedGaugeUI->ReCalculateWorldMatrix();
	}

	fNum3 = speedGaugeUI->GetScale();
	if (ImGui::SliderFloat2("SpeedGaugeUIScale", &fNum3.x, 0.1f, 3.0f))
	{
		speedGaugeUI->SetScale(fNum3);
		speedGaugeUI->ReCalculateWorldMatrix();
	}


	// �������Ԑ��lUI
	fNum3 = timeNumUI->GetParentPosition();
	if (ImGui::SliderFloat2("TimeNumUIPosition", &fNum3.x, -3.0f, 3.0f))
	{
		timeNumUI->SetParentPosition(fNum3);
	}

	fNum3 = timeNumUI->GetParentScale();
	if (ImGui::SliderFloat2("TimeNumUIScale", &fNum3.x, 0.1f, 3.0f))
	{
		timeNumUI->SetParentScale(fNum3);
	}

	// TimeUI
	fNum3 = timeUI->GetPosition();
	if (ImGui::SliderFloat2("TimeUITexturePosition", &fNum3.x, -Object2D::MaxTexPos, Object2D::MaxTexPos))
	{
		timeUI->SetPosition(fNum3);
		timeUI->ReCalculateWorldMatrix();
	}

	fNum3 = timeUI->GetScale();
	if (ImGui::SliderFloat2("TimeUITextureScale", &fNum3.x, 0.1f, Object2D::MaxTexScale))
	{
		timeUI->SetScale(fNum3);
		timeUI->ReCalculateWorldMatrix();
	}

	// ���U���g�X�R�A���lUI
	fNum3 = resultScoreNumUI->GetParentPosition();
	if (ImGui::SliderFloat2("ResultScoreNumUIPosition", &fNum3.x, -3.0f, 3.0f))
	{
		resultScoreNumUI->SetParentPosition(fNum3);
	}

	fNum3 = resultScoreNumUI->GetParentScale();
	if (ImGui::SliderFloat2("ResultScoreNumUIScale", &fNum3.x, 0.1f, 3.0f))
	{
		resultScoreNumUI->SetParentScale(fNum3);
	}

	
	ImGui::End();

}

// IMGUI�Őݒ肵���p�����[�^��CSV�ɕۑ�����
void GameScene::SaveParameters()
{
	// �p�����[�^���Ɛ��l�Ƃ��̌^map�ł܂Ƃ߂�
	std::map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	//=================================================
	//  UI
	//=================================================
	params["ScoreNumUIPosition"] = { scoreNumUI->GetParentPosition(),CSVManager::TypeName::FLOAT3 };
	params["ScoreNumUIScale"] = { scoreNumUI->GetParentScale(),CSVManager::TypeName::FLOAT3 };
	params["SpeedNumUIPosition"] = { speedNumUI->GetParentPosition(),CSVManager::TypeName::FLOAT3 };
	params["SpeedNumUIScale"] = { speedNumUI->GetParentScale(),CSVManager::TypeName::FLOAT3 };
	params["TimeNumUIPosition"] = { timeNumUI->GetParentPosition(),CSVManager::TypeName::FLOAT3 };
	params["TimeNumUIScale"] = { timeNumUI->GetParentScale(),CSVManager::TypeName::FLOAT3 };
	params["ScoreUITexturePosition"] = { scoreUI->GetPosition(),CSVManager::TypeName::FLOAT3};
	params["ScoreUITextureScale"] = { scoreUI->GetScale(),CSVManager::TypeName::FLOAT3 };
	params["TimeUITexturePosition"] = { timeUI->GetPosition(),CSVManager::TypeName::FLOAT3 };
	params["TimeUITextureScale"] = { timeUI->GetScale(),CSVManager::TypeName::FLOAT3 };
	params["ResultScoreNumUIPosition"] = { resultScoreNumUI->GetParentPosition(),CSVManager::TypeName::FLOAT3 };
	params["ResultScoreNumUIScale"] = { resultScoreNumUI->GetParentScale(),CSVManager::TypeName::FLOAT3 };
	params["SpeedGaugeUIPosition"] = { speedGaugeUI->GetPosition(),CSVManager::TypeName::FLOAT3 };
	params["SpeedGaugeUIScale"] = { speedGaugeUI->GetScale(),CSVManager::TypeName::FLOAT3 };

	//=================================================
	//  Player
	//=================================================
	std::vector<float> accelerations = player->GetAccelerations();

	for (int i = 0; i < accelerations.size(); i++)
	{
		std::string str = "PlayerAccelerations";
		str += (i + 1 < 10 ? "0" : "") + std::to_string(i + 1);
		params[str] = { accelerations[i],CSVManager::TypeName::FLOAT };
	}
	params["PlayerMaxMoveSpeed"] = { player->GetMaxMoveSpeed(),CSVManager::TypeName::FLOAT};
	params["PlayerMaxDriftAddSpeed"] = { player->GetMaxDriftAddSpeed(),CSVManager::TypeName::FLOAT };
	params["PlayerMass"] = { player->GetMass(),CSVManager::TypeName::FLOAT };
	params["PlayerJumpPower"] = { player->GetJumpPower(),CSVManager::TypeName::FLOAT };
	params["PlayerMinTurnRate"] = { player->GetMinTurnRate(),CSVManager::TypeName::FLOAT};
	params["PlayerMaxTurnRate"] = { player->GetMaxTurnRate(),CSVManager::TypeName::FLOAT };
	params["PlayerFrictionFactor"] = { player->GetFrictionFactor(),CSVManager::TypeName::FLOAT };
	params["PlayerWallFrictionFactor"] = { player->GetWallFrictionFactor(),CSVManager::TypeName::FLOAT };

	// CSV�ۑ�
	CSVManager::SaveCSVPrameters(params, "Assets/SceneParameterList/GameSceneParameters.csv");
}

// CSV����ǂݍ��񂾃p�����[�^�𔽉f������
void GameScene::LoadParameters()
{
	// �p�����[�^���Ɛ��l�Ƃ��̌^map�ł܂Ƃ߂�
	std::unordered_map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	// �t�@�C���������݂��Ȃ��ꍇ�̓f�t�H���g�̒l�ŕۑ�����
	if (!CSVManager::LoadCSVPrameters(params, "Assets/SceneParameterList/GameSceneParameters.csv"))
	{
		SaveParameters();
		return;
	}

	std::vector<float> accelerations(PlayerController::AccelerationsNum);

	// map�̃p�����[�^�����Ԃɓǂݍ���
	for (const auto& entry : params)
	{
		//=================================================
		//  UI
		//=================================================
		if (entry.first == "ScoreNumUIPosition")
		{
			scoreNumUI->SetParentPosition(std::get<XMFLOAT3>(entry.second.first));

		}
		else if (entry.first == "ScoreNumUIScale")
		{
			scoreNumUI->SetParentScale(std::get<XMFLOAT3>(entry.second.first));
		}
		else if (entry.first == "SpeedNumUIPosition")
		{
			speedNumUI->SetParentPosition(std::get<XMFLOAT3>(entry.second.first));
		}
		else if (entry.first == "SpeedNumUIScale")
		{
			speedNumUI->SetParentScale(std::get<XMFLOAT3>(entry.second.first));
		}
		else if (entry.first == "TimeNumUIPosition")
		{
			timeNumUI->SetParentPosition(std::get<XMFLOAT3>(entry.second.first));
		}
		else if (entry.first == "TimeNumUIScale")
		{
			timeNumUI->SetParentScale(std::get<XMFLOAT3>(entry.second.first));
		}
		else if (entry.first == "ScoreUITexturePosition")
		{
			scoreUI->SetPosition(std::get<XMFLOAT3>(entry.second.first));
			scoreUI->ReCalculateWorldMatrix();
		}
		else if (entry.first == "ScoreUITextureScale")
		{
			scoreUI->SetScale(std::get<XMFLOAT3>(entry.second.first));
			scoreUI->ReCalculateWorldMatrix();
		}
		else if (entry.first == "TimeUITexturePosition")
		{
			timeUI->SetPosition(std::get<XMFLOAT3>(entry.second.first));
			timeUI->ReCalculateWorldMatrix();
		}
		else if (entry.first == "TimeUITextureScale")
		{
			timeUI->SetScale(std::get<XMFLOAT3>(entry.second.first));
			timeUI->ReCalculateWorldMatrix();
		}
		else if (entry.first == "ResultScoreNumUIPosition")
		{
			resultScoreNumUI->SetParentPosition(std::get<XMFLOAT3>(entry.second.first));
		}
		else if (entry.first == "ResultScoreNumUIScale")
		{
			resultScoreNumUI->SetParentScale(std::get<XMFLOAT3>(entry.second.first));
		}
		else if (entry.first == "SpeedGaugeUIPosition")
		{
			speedGaugeUI->SetPosition(std::get<XMFLOAT3>(entry.second.first));
			speedGaugeUI->ReCalculateWorldMatrix();
		}
		else if (entry.first == "SpeedGaugeUIScale")
		{
			speedGaugeUI->SetScale(std::get<XMFLOAT3>(entry.second.first));
			speedGaugeUI->ReCalculateWorldMatrix();
		}
		//=================================================
		//  Player
		//=================================================
		else if (entry.first == "PlayerAccelerations01")
		{
			accelerations[0] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerAccelerations02")
		{
			accelerations[1] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerAccelerations03")
		{
			accelerations[2] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerAccelerations04")
		{
			accelerations[3] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerAccelerations05")
		{
			accelerations[4] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerAccelerations06")
		{
			accelerations[5] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerAccelerations07")
		{
			accelerations[6] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerAccelerations08")
		{
			accelerations[7] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerAccelerations09")
		{
			accelerations[8] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerAccelerations10")
		{
			accelerations[9] = std::get<float>(entry.second.first);
		}
		else if (entry.first == "PlayerMaxMoveSpeed")
		{
			player->SetMaxMoveSpeed(std::get<float>(entry.second.first));
			speedNumUI->SetMaxSpeed(std::get<float>(entry.second.first));
			speedNumUI->Update(0.0f);
		}
		else if (entry.first == "PlayerMaxDriftAddSpeed")
		{
			player->SetMaxDriftAddSpeed(std::get<float>(entry.second.first));
		}
		else if (entry.first == "PlayerMass")
		{
			player->SetMass(std::get<float>(entry.second.first));
		}
		else if (entry.first == "PlayerJumpPower")
		{
			player->SetJumpPower(std::get<float>(entry.second.first));
		}
		else if (entry.first == "PlayerMinTurnRate")
		{
			player->SetMinTurnRate(std::get<float>(entry.second.first));
		}
		else if (entry.first == "PlayerMaxTurnRate")
		{
			player->SetMaxTurnRate(std::get<float>(entry.second.first));
		}
		else if (entry.first == "PlayerFrictionFactor")
		{
			player->SetFrictionFactor(std::get<float>(entry.second.first));
		}
		else if (entry.first == "PlayerWallFrictionFactor")
		{
			player->SetWallFrictionFactor(std::get<float>(entry.second.first));
		}

		// �p�����[�^�����o�^����Ă��Ȃ��ꍇ
		else
		{
			std::string message = entry.first + " :�l�ǂݍ��݃G���[";
			MessageBoxA(NULL, message.c_str(), "ErrorMesseage", MB_OK | MB_ICONERROR);
		}
	}

	player->SetAccelerations(accelerations);
}