#include "GameScene.h"
#include "GameManager.h"
#include "CSVManager.h"
#include "Geometry.h"
#include "DebugUI.h"
#include "Fps.h"
#include "TitleScene.h"
#include "xa2.h"

using namespace DirectX;

// 初期化処理
void GameScene::Init()
{
	// CSVから読み込んだデータを格納する配列
	std::vector<GameScene::MapObject> mapObjs;
	
	// 選択ステージに応じて読み込むCSVを変える
	switch (GameManager::GetSelectStage())
	{
	case GameManager::STAGE_1: // ステージ1
		CSVManager::LoadCSVMapData(mapObjs, "Assets/MapData/Stage01.csv");
		break;

	case GameManager::STAGE_2: // ステージ2
		CSVManager::LoadCSVMapData(mapObjs, "Assets/MapData/Stage02.csv");
		break;

	case GameManager::STAGE_3: // ステージ3
		CSVManager::LoadCSVMapData(mapObjs, "Assets/MapData/Stage03.csv");
		break;

	default:
		break;
	}

	// 選択ステージの制限時間を取得
	stageTime = GameManager::GetStageTimeData();

	//=================================================
	//  オブジェクト生成
	//=================================================

	// 読み込んだモデルを管理する配列
	modelManager = std::make_unique<ModelHash>();

	// ステージの各タイプのオブジェクト
	std::vector<Boxs::BoxData> groundData;     // 地面オブジェクトデータを格納する配列
	std::vector<Boxs::BoxData> wallData;       // 壁オブジェクトデータを格納する配列
	std::vector<Slopes::SlopeData> slopeData;  // 坂道オブジェクトデータを格納する配列
	std::vector<Items::ItemData> itemData;     // アイテムオブジェクトデータを格納する配列

	// オブジェクト追加専用
	Boxs::BoxData bd;
	Slopes::SlopeData sd;
	Items::ItemData id;

	XMFLOAT3 plyPosition{ 0,0,0 };
	XMFLOAT3 plyRotation{ 0,0,0 };
	XMFLOAT3 plyScaling{ 0,0,0 };

	// CSVから読み込んだデータをタイプごとに処理
	for (auto& obj : mapObjs)
	{
		switch (obj.objType)
		{
		case GROUND:      // 地面オブジェクト
			bd.position = obj.position;
			bd.rotation = obj.rotation;
			bd.scaling = obj.scale;
			bd.color = XMFLOAT4(0.2f, 0.2f, 0.9f, 1.0f);
			groundData.push_back(bd);
			break;

		case WALL:        // 壁オブジェクト
			bd.position = obj.position;
			bd.rotation = obj.rotation;
			bd.scaling = obj.scale;
			bd.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f);
			wallData.push_back(bd);
			break;

		case SLOPE:       // 坂道オブジェクト
			sd.position = obj.position;
			sd.rotation = obj.rotation;
			sd.scaling = obj.scale;
			sd.color = XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f);
			slopeData.push_back(sd);
			break;

		case PLAYER:      // プレイヤーオブジェクト
			plyPosition = obj.position;
			plyRotation = obj.rotation;
			plyScaling = obj.scale;
			break;

		case ITEM_SMALL:    // アイテム小
			id.position = obj.position;
			id.rotation = obj.rotation;
			id.scaling = obj.scale;
			id.color = XMFLOAT4(colorPhase1.x, colorPhase1.y, colorPhase1.z, 1.0f);
			id.type = ItemObject::SMALL;
			itemData.push_back(id);
			break;
		case ITEM_MIDDLE:   // アイテム中
			id.position = obj.position;
			id.rotation = obj.rotation;
			id.scaling = obj.scale;
			id.color = XMFLOAT4(colorPhase2.x, colorPhase2.y, colorPhase2.z, 1.0f);
			id.type = ItemObject::MIDDLE;
			itemData.push_back(id);

			break;
		case ITEM_BIG:      // アイテム大
			id.position = obj.position;
			id.rotation = obj.rotation;
			id.scaling = obj.scale;
			id.color = XMFLOAT4(colorPhase3.x, colorPhase3.y, colorPhase3.z, 1.0f);
			id.type = ItemObject::BIG;
			itemData.push_back(id);

			break;
		case ITEM_MEGA_BIG: // アイテム特大
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

	// プレイヤーオブジェクト
	player = std::make_unique<PlayerController>(
		plyPosition,
		plyRotation,
		plyScaling,
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// 3Dカメラオブジェクト
	camera3D = std::make_unique<GameSceneCamera>();

#ifdef _DEBUG
	// デバッグカメラ
	debugCamera3D = std::make_unique<DebugCamera>();
#endif

	// ライト
	sceneLight = std::make_unique<Light>(XMFLOAT3(1000.0f, 800.0f, 0.0f));

	// 地面オブジェクト
	ground = std::make_unique<Boxs>(groundData);

	// 壁オブジェクト
	wall = std::make_unique<Boxs>(wallData);

	// 坂道オブジェクト
	slope = std::make_unique<Slopes>(slopeData);

	// アイテムオブジェクト
	item = std::make_unique<Items>(itemData);

	// アイテムエフェクト
	itemFxs.resize(3);
	for (auto& fx : itemFxs)
	{
		fx = std::make_unique<ItemEffect>();
	}

	// 火花エフェクト
	fireFlowerFx = std::make_unique<FireFlowerEffect>();

	// コースアウトのフェード
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

	// スコア数値UI
	scoreNumUI = std::make_unique<ScoreUI>();

	// TimeUI
	timeUI = std::make_unique<Object2D>(
		XMFLOAT3(1.15f, 1.145f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.3f, 0.13125f, 0.1f),
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// TimeUI
	timeNumUI = std::make_unique<TimeUI>();

	// 速度数値UI
	speedNumUI = std::make_unique<SpeedUI>();

	// 速度ゲージUI
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

	// 開始カウント数値UI
	startCountUI = std::make_unique<CountNumberUI>();

	// 風切り演出エフェクト
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

	// リザルト背景
	resultBg = std::make_unique<Object2D>(
		XMFLOAT3(0.0f, 0.0f, -0.19f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(16.0f, 9.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f));

	// ScoreUI（リザルト）
	resultScoreUI = std::make_unique<Object2D>(
		XMFLOAT3(-1.7f, 0.02f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// リザルトスコア数値UI
	resultScoreNumUI = std::make_unique<ScoreUI>();

	// PressEnterKeyUI（リザルト）
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
	//  モデル・テクスチャ読み込み
	//=================================================

	// 画像ファイル
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

	// 画像読み込み
	for (int i = 0; i < ARRAYSIZE(textureFileName); i++)
	{
		model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1,1 });
		model.texture = Renderer::LoadTexture(textureFileName[i]);
		modelManager->emplace(textureFileName[i], model);
	}

	// モデルファイル
	const wchar_t* modelFileName[] =
	{
		L"Assets/Model/Player.obj",
	};

	// モデル読み込み
	for (int i = 0; i < ARRAYSIZE(modelFileName); i++)
	{
		model = Renderer::LoadObjModel(modelFileName[i]);
		modelManager->emplace(modelFileName[i], model);
	}

	//=================================================
	//  モデルセット
	//=================================================

	// プレイヤーオブジェクト
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

	// 速度ゲージUI
	if (model.texture)
	{
		model.texture.Reset();
	}
	speedGaugeUI->SetModel(model);

	// 風切り演出エフェクト
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1.0f, 0.166667f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/fx02.png");
	speedAnim->SetModel(model);
	
	// コースアウトフェード
	model = modelManager->at(L"Assets/Texture/fade.png");
	holeFade->SetModel(model);
	
	// リザルト背景
	model = modelManager->at(L"Assets/Texture/fade.png");
	resultBg->SetModel(model);

	// ResultUI
	model = modelManager->at(L"Assets/Texture/Result.png");
	resultUI->SetModel(model);

	// ScoreUI（リザルト）
	model = modelManager->at(L"Assets/Texture/Score.png");
	resultScoreUI->SetModel(model);

	// PressEnterKeyUI（リザルト）
	model = modelManager->at(L"Assets/Texture/PressEnterKey.png");
	pressEnterKeyUI->SetModel(model);

	// NewBestScoreUI
	model = modelManager->at(L"Assets/Texture/NewBestScore.png");
	newBestScoreUI->SetModel(model);
	
	//=================================================
	//  初期設定
	//=================================================

	// プレイヤーオブジェクト
	player->SetShader(Shader::VS_OBJECT, Shader::PS_SPECULER);
	player->SetMotionBlurShader(Shader::VS_INSTANCING, Shader::PS_SPECULER);
	player->SetBoundingSphere(player->GetPosition(), 1.0f);
	player->SetItemBoundingSphere(player->GetPosition(), 1.0f);
	player->SetSpeedRatioColorPhase(ratioPhase1, ratioPhase2, ratioPhase3, ratioPhase4);
	player->SetSpeedRatioColor(colorPhase1, colorPhase2, colorPhase3, colorPhase4);

	// カメラ3D
	camera3D->SetEye({ player->GetPosition().x ,player->GetPosition().y,player->GetPosition().z - 10.0f });
	camera3D->SetRevoAngle(D2R(plyRotation.y));

	// 地面オブジェクト
	ground->SetShader(Shader::VS_INSTANCING_TEXTURE, Shader::PS_SPECULER_TEXTURE);
	ground->SetTexture(modelManager->find(L"Assets/Texture/floor.png")->second.texture);
	Renderer::Material mat;
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 10.0f);
	mat.other = XMFLOAT4(1.25f, 0.0f, 0.0f, 0.0f);
	ground->SetMaterial(mat);

	// 壁オブジェクト
	wall->SetShader(Shader::VS_AMBIENT_INSTANCING, Shader::PS_AMBIENT);

	// 坂道オブジェクト
	slope->SetShader(Shader::VS_INSTANCING_TEXTURE, Shader::PS_SPECULER);
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mat.other = XMFLOAT4(0.02f, 0.0f, 0.0f, 0.0f);
	slope->SetMaterial(mat);

	// アイテムオブジェクト
	item->SetShader(Shader::VS_INSTANCING, Shader::PS_SPECULER);
	item->SetAnimationShader(Shader::VS_ITEM_ANIMATION, Shader::PS_COLOR, Shader::GS_ITEM_ANIMATION);
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mat.other = XMFLOAT4(0.02f, 0.0f, 0.0f, 0.0f);
	item->SetMaterial(mat);
	item->SetBreakAllItemRatio(ratioPhase1, ratioPhase2, ratioPhase3, ratioPhase4);

	// アイテムエフェクト
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

	// 火花エフェクト
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

	// コースアウトフェード
	holeFade->SetSpeed(1.0f);

	// ScoreUI
	scoreUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// スコア数値UI
	scoreNumUI->Update(0);
	speedNumUI->Update(0.0f);
	speedNumUI->SetSpeedRatioColorPhase(ratioPhase1, ratioPhase2, ratioPhase3, ratioPhase4);
	speedNumUI->SetSpeedRatioColor(colorPhase1, colorPhase2, colorPhase3, colorPhase4);
	speedNumUI->UpdateSpeedRatioColor(0.0f);
	
	// TimeUI
	timeUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// タイム数値UI
	timeNumUI->Update(stageTime);

	// StartUI
	startUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	startUI->SetIsActive(false);

	// 開始カウントダウン数値UI
	startCountUI->SetActive(false);

	// TimeUpUI
	timeUpUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	timeUpUI->SetIsActive(false);
	
	// 風切り演出エフェクト
	speedAnim->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	speedAnim->SetAnimationSpeed(speedAnimSpeed);
	speedAnim->SetAnimationTable(6);
	speedAnim->SetIsLoop(true);
	speedAnim->SetDivision(1, 6);
	speedAnim->SetColorAlpha(0.0f);
	speedAnim->Play();

	// 速度ゲージUI
	speedGaugeUI->SetShader(Shader::VS_SPEED_GAUGE, Shader::PS_SPEED_GAUGE);
	speedGaugeUI->SetSpeedRatioColorPhase(ratioPhase1, ratioPhase2, ratioPhase3, ratioPhase4);
	speedGaugeUI->SetSpeedRatioColor(colorPhase1, colorPhase2, colorPhase3, colorPhase4);
	speedGaugeUI->InitGaugeColor();

	// リザルト背景
	resultBg->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	resultBg->SetIsActive(false);

	// ResultUI
	resultUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	resultUI->SetIsActive(false);

	// リザルトスコア数値UI
	resultScoreNumUI->Update(0);
	resultScoreNumUI->SetActive(false);

	// ScoreUI（リザルト）
	resultScoreUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	resultScoreUI->SetIsActive(false);

	// PressEnterKeyUI（リザルト）
	pressEnterKeyUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	pressEnterKeyUI->SetIsActive(false);

	// NewBestScoreUI（リザルト）
	newBestScoreUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	newBestScoreUI->SetIsActive(false);

	// コースアウト時使用のプレイヤーデータ
	playerRecordData.reserve(MaxRecordDataSize);

	// コースアウト時使用のカメラ3Dデータ
	cameraRecordData.reserve(MaxRecordDataSize);
	SetRecordDataSize(recordDataSize);

	// フラスタムカリング時に使用する
	// オブジェクトのアクティブ状態の参照を保持する配列
	activeBoundsData.reserve(mapObjs.size());

	// 地面オブジェクト
	for (auto& ground : ground->boxs)
	{
		activeBoundsData.push_back({ ground->GetBoundingSphere(),ground->GetIsActive() });
	}

	// 壁オブジェクト
	for (auto& wall : wall->boxs)
	{
		activeBoundsData.push_back({ wall->GetBoundingSphere(),wall->GetIsActive() });
	}

	// 坂道オブジェクト
	for (auto& slope : slope->slopes)
	{
		activeBoundsData.push_back({ slope->GetBoundingSphere(),slope->GetIsActive() });
	}

	// アイテムオブジェクト
	for (auto& item : item->items)
	{
		activeBoundsData.push_back({ item->GetBoundingSphere(),item->GetIsActive() });
	}
	
	// ブロードフェーズ時に使用する
	// オブジェクトのアクティブ状態の参照を保持する配列
	broadPhaseBoundsData.reserve(mapObjs.size());

	// ステージのスコアを初期化（0にする）
	GameManager::InitStageScore();

	// カメラ初回更新
	camera3D->Update(FPS::Get()->GetFixedDeltaTime(), *this);
}

// 解放処理
void GameScene::UnInit()
{
	// BGM停止
	XA_Stop(RESULT_BGM);

#ifdef _DEBUG
	SaveParameters();
#endif
}

// 更新処理
void GameScene::Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm)
{
	// ゲームシーンの進行度に応じた分岐
	switch (gameState)
	{
	case GameScene::NO_PLAY:     // ゲーム開始前
		StartGameUpdate(_deltaTime);
		break;

	case GameScene::PLAY:        // ゲームプレイ中
		PlayGameUpdate(_deltaTime, gm);
		break;

	case GameScene::TIME_UP:     // タイムアップ時
		TimeUpUpdate(_deltaTime);
		break;

	case GameScene::RESULT:      // リザルト
		ResultUpdate(_deltaTime, sceneManager, gm);
		break;

	case GameScene::SCENE_END:   // ゲームシーン終了
		break;

	default:
		break;
	}

	sceneLight->CalculateShadowMatrix(player->GetPosition(),camera3D->GetViewMatrix());

	// フラスタムカリングチェック
	DrawFrustumCheck();


#ifdef _DEBUG
	
	// Cキーが押されたら
	// メインカメラとデバッグカメラを切り替える
	if (Input::Get().GetKeyTrigger(DIK_C))
	{
		isDebugCameraChange = !isDebugCameraChange;
		if (isDebugCameraChange)
		{
			debugCamera3D->SetEye(camera3D->GetAfterEye());
			debugCamera3D->SetFocus(camera3D->GetFocus());
		}
	}

	// Uキーが押されたら
	// UI表示・非表示切り替え
	if (Input::Get().GetKeyTrigger(DIK_U))
	{
		isDebugDrawUI = !isDebugDrawUI;
	}

	// Pキーが押されたら
	// プレイ中の一時停止
	if (Input::Get().GetKeyTrigger(DIK_P))
	{
		isDebugPause = !isDebugPause;
	}
	// 一時停止中にEnterキーで1フレーム進める
	if (isDebugPause)
	{
		if (Input::Get().GetKeyTrigger(DIK_RETURN))
		{
			isDebugStepFrame = true;
		}
	}

	// Fキーが押されたら
	// フラスタムカリングON・OFF切り替え
	if (Input::Get().GetKeyTrigger(DIK_F))
	{
		isDebugFrustum = !isDebugFrustum;
	}

	// デバッグカメラに切り替え時のみ
	// 更新処理を実行
	if (isDebugCameraChange)
	{
		debugCamera3D->Update(_deltaTime);
	}
#endif
}

// ゲーム開始前の更新処理
void GameScene::StartGameUpdate(float _deltaTime)
{
	// カメラの操作無効にする
	camera3D->SetIsInputCamera(false);

	// フェードが終わったら
	if (!SceneManager::GetFade().GetIsFade())
	{
		// StartUIを表示させる
		startCountUI->SetActive(true);

		// カウントダウンのためのタイム減算
		startCount -= _deltaTime;

		// 1秒ごとにSEを鳴らすための加算
		seTimer += _deltaTime;

		// 1秒経ったら
		if (seTimer > 1.0f)
		{
			// SE再生
			XA_Play(COUNT_SE, CountSEVol * volRatio);
			XA_SetSpeed(COUNT_SE, StartCountSESpeed);
			seTimer = 0.0f;
		}

		// カウントダウンUIの更新
		startCountUI->SetNum(static_cast<int>(startCount) + 1);

		// カウントが0になったら
		if (startCount < 0.0f)
		{
			// プレイ状態に切り替え
			gameState = GameScene::PLAY;

			// ここでBGMを再生する
			XA_Play(STAGE_BGM, StageBGMVol * volRatio);

			// スタートSE再生
			XA_Play(START_SE, StartSEVol * volRatio);

			startCountUI->SetActive(false);
			startUI->SetIsActive(true);

			// カメラを操作可能にする
			camera3D->SetIsInputCamera(true);
		}
	}
}

// ゲームプレイ中の更新処理
void GameScene::PlayGameUpdate(float _deltaTime, GameManager& gm)
{
#ifdef _DEBUG
	// 一時停止中でかつisDebugStepFrameがfalseのときは
	// returnする
	if (isDebugPause)
	{
		if (!isDebugStepFrame)
		{
			return;
		}
		else
		{
			// falseにして一度だけUpdateを通す
			isDebugStepFrame = false;
		}
	}

#endif

	// ゲーム内の速度割合をそれぞれに反映
	UpdateStageByRatio(_deltaTime);

	// プレイヤーとカメラの情報を記録する（コースアウト時使用）
	UpdateRecordData();

	// コースアウト判定を行う
	PlayerHoleCheck();

	// ヒットストップ時に更新する
	UpdateHitStop(_deltaTime);

	//==================================================
	// ゲーム内オブジェクトの更新処理
	//==================================================

	// プレイヤーの前方向に進むための方向を設定
	player->SetDirAngle(camera3D->GetRevoAngle());

	// プレイヤー更新
	player->Update(_deltaTime);

	// ヒットストップ中ではないとき
	if (!isHitStop)
	{
		// アイテム更新
		item->Update(_deltaTime);

		Particle::SetCameraPosition(camera3D->GetAfterEye());
		Particle::SetFrustumPlanes(frustum.planes);

		// アイテムエフェクト更新
		for (auto& fx : itemFxs)
		{
			fx->Update(_deltaTime);
		}

		// 火花エフェクトの生成位置を取得
		XMFLOAT3 spawnPos = player->GetPosition();
		spawnPos.y -= player->GetBoundingSphere().radius; // プレイヤーの下から発生するように設定
		fireFlowerFx->SetParticleSpawnPosition(spawnPos);

		// 火花の発射する方向を決めるため
		// プレイヤーの新しい座標と過去の座標をそれぞれ取得
		fireFlowerFx->SetTargetNewPosition(player->GetNewPosition());
		fireFlowerFx->SetTargetOldPosition(player->GetOldPosition());

		// 火花エフェクト更新
		fireFlowerFx->Update(_deltaTime);
	}

	// カメラのポジション取得
	XMFLOAT3 camPos = camera3D->GetAfterEye();
	// 半透明壁をカメラから遠い順にソート
	std::sort(wall->boxs.begin(), wall->boxs.end(), [&](const std::unique_ptr<BoxObject>& a, const std::unique_ptr<BoxObject>& b)
	{
		return a->DistanceFromCamera(camPos) > b->DistanceFromCamera(camPos);
	});

	// コースアウトフェード更新
	holeFade->Update(_deltaTime);

	// 地面・壁・坂道との当たり判定を実行
	CheckOtherObjectCollision();

	// アイテムオブジェクトとの当たり判定を実行
	CheckItemObjectCollision(gm);

	// スコアUI更新
	scoreNumUI->Update(gm.GetStageScore());

	// 速度UI更新
	speedNumUI->Update(player->GetRealSpeed());

	// 風切りエフェクト更新
	speedAnim->Update(_deltaTime);

	//==================================================
	//  ステージの制限時間を管理
	//==================================================
	if (!isHitStop)
	{
		stageTime -= _deltaTime; // 制限時間を減算
	}
	
	// 制限時間が0になったら
	if (stageTime < 0.0f)
	{
		stageTime = 0.0f;
		gameState = GameScene::TIME_UP;  // 進行状態をタイムアップに切り替え
		timeUpUI->SetIsActive(true);

		// BGM・SEの再生・停止
		XA_Play(TIME_UP_SE, TimeUpSEVol* volRatio);
		XA_Stop(STAGE_BGM);
		XA_Stop(CHARGE_SE);
		XA_Stop(MOVE_SE);

		// ドリフトのエフェクトを止める
		player->SetIsDriftingEffectPlay(false);
	}
	// 制限時間が残り10秒になったら
	else if (stageTime <= countDownSeStartTime)
	{
		// 1秒ごとにSEを再生
		seTimer += _deltaTime;
		if (seTimer > 1.0f)
		{
			XA_Play(COUNT_SE, CountSEVol * volRatio);
			seTimer = 0;
		}
	}

	// タイムUI更新
	timeNumUI->Update(stageTime);

	//==================================================
	//  ゲームスタート直後の処理
	//==================================================
	if (startUI->GetIsActive())
	{
		// StartUIのα値を徐々に下げる
		startUI->SetColorAlpha(startUIAlpha);
		startUIAlpha -= changeAlphaSpeed;
		
		// 完全に0になったら
		if (startUIAlpha < 0.0f)
		{
			startUI->SetIsActive(false);
		}
	}

	// カメラの更新
	camera3D->Update(_deltaTime, *this);
}

// タイムアップ時の更新処理
void GameScene::TimeUpUpdate(float _deltaTime)
{
	// TimeUpUIの描画時間が0以上の場合
	if (timeUpUIDrawTime > 0.0f)
	{
		timeUpUIDrawTime -= _deltaTime; // 時間を減算

		// 完全に0になったら
		if (timeUpUIDrawTime < 0.0f)
		{
			// 各UIの非表示設定
			timeUI->SetIsActive(false);
			scoreUI->SetIsActive(false);
			timeNumUI->SetActive(false);
			scoreNumUI->SetActive(false);
			speedNumUI->SetActive(false);
			speedGaugeUI->SetIsActive(false);
		}
	}
	// TimeUpUIのα値を徐々に下げる
	else if (timeUpUI->GetColor().w > 0.0f)
	{
		timeUpUIAlpha -= changeAlphaSpeed;
		timeUpUI->SetColorAlpha(timeUpUIAlpha);
	}
	// α値が完全に0になったら
	else
	{
		// リザルト画面の表示
		resultUI->SetIsActive(true);
		resultBg->SetIsActive(true);
		resultScoreUI->SetIsActive(true);
		resultScoreNumUI->SetActive(true);

		// 進行状態をリザルトに切り替え
		gameState = GameScene::RESULT;

		// リザルトのBGM再生
		XA_Play(RESULT_BGM, ResultBGMVol * volRatio);
	}
}

// リザルトの更新
void GameScene::ResultUpdate(float _deltaTime, SceneManager& sceneManager, GameManager& gm)
{
	// 獲得スコアを取得する
	float resultScore = static_cast<float>(gm.GetStageScore());

	// スコアを加算するまでの待機時間
	if (resultStartWaitTime > 0.0f)
	{
		resultStartWaitTime -= _deltaTime;
	}
	// スコア加算中の演出
	else if(resultDrawScore < resultScore && !isResultEnd)
	{
		// 目標値が遠いほど加算速度を上昇させる
		float speedFactor = 1.0f - resultProgress;

		// 速度が遅くなりすぎないように調整する
		if (speedFactor < resultScoreMinSpeedFactor)
		{
			speedFactor = resultScoreMinSpeedFactor;
		}

		// 目標値までの経過割合を加算する
		resultProgress += resultScoreSpeedRaito * _deltaTime * speedFactor;
		// 1.0を超えないように調整
		resultProgress = std::clamp(resultProgress, 0.0f, 1.0f);
		
		// 現在描画中のスコアを格納
		float nowResultScore = resultDrawScore;

		// 表示するスコアを更新
		resultDrawScore = resultScore * resultProgress;


		bool isPlaySE = false; // SEを再生する

		// 毎フレーム鳴るのを防ぐために
		// 一定の間隔ごとにSEを再生する
		if (seTimer < resultSeInterval)
		{
			seTimer += _deltaTime;
		}
		else
		{
			isPlaySE = true;
		}

		// 前回とスコアが違う場合かつSEを再生できる場合
		if ((static_cast<int>(nowResultScore) != static_cast<int>(resultDrawScore)) && isPlaySE)
		{
			XA_Play(ADD_SCORE_SE, AddScoreSEVol * volRatio);
			seTimer = 0;
		}

		// EnterキーまたはゲームパッドAが押されたら
		// 加算中のスコアを一気に目標スコアにする
		if (Input::Get().GetKeyTrigger(DIK_RETURN) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_A))
		{
			XA_Play(DECISION_SE, DecisionSEVol * volRatio);
			resultDrawScore = resultScore;
		}
		
		// 目標スコアに達したら
		if (resultDrawScore >= resultScore)
		{
			resultDrawScore = resultScore;
			pressEnterKeyUI->SetIsActive(true);
			
			// 履歴のスコアが更新されたか判定
			if (gm.IsHighScoreUpdated())
			{
				// スコアが更新された場合UIを表示
				newBestScoreUI->SetIsActive(true);
			}

			isResultEnd = true; // リザルト終了
		}

		// リザルトスコアUIの更新
		resultScoreNumUI->Update(static_cast<int>(resultDrawScore));
		
	}
	else
	{
		// EnterキーまたはゲームパッドAが押されたら
		if (Input::Get().GetKeyTrigger(DIK_RETURN) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_A))
		{
			XA_Play(DECISION_SE, DecisionSEVol * volRatio);

			// TitleSceneに切り替える
			sceneManager.SetScene(std::make_unique<TitleScene>());

			// 進行状態をゲーム終了にする
			gameState = GameScene::SCENE_END;
		}
	}
}

// プレイヤーの速度割合をそれぞれに反映させる
void GameScene::UpdateStageByRatio(float _deltaTime)
{
	// プレイヤーの速度割合を取得
	playerSpeedRatio = player->GetSpeedRatio();

	// スコアの加算量の調整
	scoreUpRatio = baseScoreUpRatio * (1.0f + playerSpeedRatio);
	
	// カメラの視野角を調整
	float cameraFovValue = 
		GameSceneCamera::MinFov 
		+ (GameSceneCamera::MaxFov - GameSceneCamera::MinFov) 
		* playerSpeedRatio;

	// 現在の視野角を取得
	float nowFov = camera3D->GetFovAngle();

	// 急減速による急な視野角の変化を防ぐため補完する
	float interpSpeed = fovInterpSpeed * _deltaTime; // 補間速度
	float newFov = nowFov + (cameraFovValue - nowFov) * interpSpeed;

	camera3D->SetFovAngle(newFov);

	// ヒットストップの時間を調整
	// 最速付近になればなるほどほぼ0になる
	if (playerSpeedRatio < ratioPhase4)
	{
		hitStopTime = baseHitStopTime * (ratioPhase4 - playerSpeedRatio);
	}
	else
	{
		hitStopTime = 0.0f;
	}
	
	// アイテムオブジェクトの破壊可能・不可状態を切り替える
	item->SetRealRatio(playerSpeedRatio);

	// 火花エフェクトの粒子の数を調整
	fireFlowerFx->SetTargetSpeedRatio(playerSpeedRatio);
	
	// アイテムオブジェクトと当たり判定時の判定領域を調整
	Collision::BoundingSphere playerBs1 = player->GetBoundingSphere();
	Collision::BoundingSphere& playerBs2 = player->GetItemBoundingSphere();

	float collisionRatio = 0.0f;
	if (playerSpeedRatio > ratioPhase3)
	{
		collisionRatio = (playerSpeedRatio - ratioPhase3) / (1.0f - ratioPhase3);
	}
	playerBs2.radius = playerBs1.radius + (10.0f * collisionRatio);

	// 速度UIの色を変える
	speedNumUI->UpdateSpeedRatioColor(playerSpeedRatio);

	// 割合に応じたゲージを反映させる
	speedGaugeUI->SetRatio(playerSpeedRatio);

	// 風切りエフェクトのα値を速度を調整する
	float targetAnimAlpha = (playerSpeedRatio - speedAnimStartRatio) / (1.0f - speedAnimStartRatio) * speedAnimMaxAlpha;
	targetAnimAlpha = std::clamp(targetAnimAlpha, 0.0f, speedAnimMaxAlpha);
	float nowAlpha = speedAnim->GetColor().w;
	nowAlpha = nowAlpha + (targetAnimAlpha - nowAlpha) * smoothingAnimFactor;
	// α値セット
	speedAnim->SetColorAlpha(nowAlpha);
	// 速度セット
	speedAnim->SetAnimationSpeed(speedAnimSpeed * (1.0f + playerSpeedRatio));
}

// 記録用配列の大きさ変更
void GameScene::SetRecordDataSize(const int _size)
{
	playerRecordData.resize(_size);
	cameraRecordData.resize(_size);
}

// プレイヤーとカメラの記録を行う
void GameScene::UpdateRecordData()
{
	// プレイヤーが地面にいるときに記録を行う
	if (player->GetIsGround())
	{
		// プレイヤー情報
		PlayerRecordData prd;
		prd.position = player->GetPosition();        // 位置
		prd.oldPosition = player->GetOldPosition();  // 過去の座標
		prd.angle = player->GetAngle();              // 角度

		// カメラ情報
		CameraRecordData crd;
		crd.eye = camera3D->GetEye();                // 位置
		crd.angle = camera3D->GetRevoAngle();        // 回転角度

		// 各配列に格納する
		if (recordDataIndex < recordDataSize)
		{
			playerRecordData[recordDataIndex] = prd;
			cameraRecordData[recordDataIndex] = crd;
		}
		recordDataIndex++; // インデックスカウントを進める

		// 配列のサイズに達したらインデックスカウントを0にする
		if (recordDataIndex >= recordDataSize)
		{
			recordDataIndex = 0;
		}
	}
}

// 記録データを反映させる
void GameScene::ApplyRecordData()
{
	// 速度によってどこまで戻すかを調整
	// 復帰してすぐに落ちてしまうのを防ぐ
	float clampedValue = 
		restoreLocationIndexCount
		+ (recordDataSize - restoreLocationIndexCount)
		* playerSpeedRatio;

	// 目標のカウントを計算
	int targetIndex = (recordDataIndex - static_cast<int>(clampedValue) + recordDataSize) % recordDataSize;

	// 復活時プレイヤーを少し上のから復活する
	// 時間が遡っているように見せないため
	playerRecordData[recordDataIndex].position.y += holeReturnAddPositionY;
	playerRecordData[recordDataIndex].oldPosition.y += holeReturnAddPositionY;

	// プレイヤーの過去の情報を反映
	player->SetPosition(playerRecordData[recordDataIndex].position);
	player->SetOldPosition(playerRecordData[recordDataIndex].oldPosition);
	player->SetAngle(playerRecordData[recordDataIndex].angle);

	// コースアウト後の減速処理
	player->HoleAfterSet();
	
	// カメラの過去の情報を反映
	cameraRecordData[recordDataIndex].eye.y += holeReturnAddPositionY;
	camera3D->SetEye(cameraRecordData[recordDataIndex].eye);
	camera3D->SetRevoAngle(cameraRecordData[recordDataIndex].angle);
}

// プレイヤーのコースアウトチェック
void GameScene::PlayerHoleCheck()
{
	// コースアウトしていないとき
	if (!isCourseOut)
	{
		// 一定の座標より下がったら
		if (player->GetPosition().y < holeJudgePositionY)
		{
			// カメラの追従をやめる
			camera3D->SetIsChase(false);

			// コースアウト専用のフェード開始
			holeFade->StartFadeOut();
			isCourseOut = true;
		}
	}
	else
	{
		// フェードアウトが終了したら
		if (!holeFade->GetIsFade())
		{
			// フェードイン開始
			holeFade->StartFadeIn();

			// カメラの追従再開
			camera3D->SetIsChase(true);

			// 戻す位置の情報をプレイヤーとカメラに反映
			ApplyRecordData();
			isCourseOut = false;
		}
	}
}

// ヒットストップ開始
void GameScene::StartHitStop()
{
	if (playerSpeedRatio <= ratioPhase4)
	{
		isHitStop = true;
		player->SetIsHitStop(true);
		camera3D->SetIsHitStop(true);

		// カメラを揺らす
		camera3D->StartCameraShake(hitStopTime, playerSpeedRatio);
	}
	else
	{
		ProcessItem();
	}
	
}

// ヒットストップ中の更新処理
void GameScene::UpdateHitStop(float _deltaTime)
{
	if (!isHitStop) return;

	// 経過時間を加算
	hitStopElapsedTime += _deltaTime;

	// 一定時間が経過したら
	// ヒットストップ停止処理
	if (hitStopElapsedTime > hitStopTime)
	{
		hitStopElapsedTime = 0;
		isHitStop = false;
		player->SetIsHitStop(false);
		camera3D->SetIsHitStop(false);

		ProcessItem();
	}
}

// 描画処理
void GameScene::Draw(Camera2D& _camera2D)
{

	// 3Dカメラセット
	Renderer::SetCameraViewProjection(camera3D->GetViewMatrix(), camera3D->GetProjectionMatrix());

#ifdef _DEBUG
	// デバッグカメラ切り替え時
	if (isDebugCameraChange)
	{
		Renderer::SetCameraViewProjection(debugCamera3D->GetViewMatrix(), debugCamera3D->GetProjectionMatrix());
	}
#endif

	// デフォルトの深度ステンシルステート
	Renderer::GetContext()->OMSetDepthStencilState(nullptr, 1);

#ifdef _DEBUG
	if (isDebugObjectSingleDraw)
	{
		ground->DebugSingleDraw();
		item->DebugSingleDraw();
	}
	else
	{
		// 地面オブジェクト
		ground->InstancingWithShadowsDraw();
		// アイテムオブジェクト
		item->InstancingDraw();
	}

#else
	// 地面オブジェクト
	ground->InstancingWithShadowsDraw();
	// アイテムオブジェクト
	item->InstancingDraw();
#endif
	
	// 坂道オブジェクト
	slope->InstancingDraw();

	// プレイヤー
	player->Draw();

	// カスタムの深度ステンシルステート
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState().Get(), 1);

#ifdef _DEBUG
	if (isDebugObjectSingleDraw)
	{
		wall->DebugSingleDraw();
	}
	else
	{
		// 壁オブジェクト
		wall->InstancingWithoutShadowsDraw();
	}

#else
	// 壁オブジェクト
	wall->InstancingWithoutShadowsDraw();
#endif
	

	// プレイヤー（モーションブラー）
	player->MortionBlurDraw();

	// プレイヤー（ドリフトエフェクト）
	player->DriftingEffectDraw();

	// プレイヤー（ソニックブーム）
	player->SonicBoomDraw();

	// アイテム（爆散アニメーション）
	item->ExplosionAnimation();

	// パーティクルエフェクト
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

	// 2Dカメラセット
	Renderer::SetCameraViewProjection(_camera2D.GetViewMatrix(), _camera2D.GetProjectionMatrix());
	
	// 風切りエフェクト
	speedAnim->Draw();

	// コースアウトフェード
	holeFade->Draw();

	// スコア数値UI
	scoreNumUI->Draw();

	// ScoreUI
	scoreUI->Draw();

	// 速度数値UI
	speedNumUI->Draw();

	// 速度ゲージUI
	speedGaugeUI->Draw();

	// 制限時間数値UI
	timeNumUI->Draw();

	// TimeUI
	timeUI->Draw();

	// 開始時のカウント数値UI
	startCountUI->Draw();

	// StartUI
	startUI->Draw();

	// TimeUpUI
	timeUpUI->Draw();

	// リザルト背景
	resultBg->Draw();

	// ResultUI
	resultUI->Draw();

	// ScoreUI（リザルト）
	resultScoreUI->Draw();

	// スコア数値UI（リザルト）
	resultScoreNumUI->Draw();

	// NewBestScoreUI
	newBestScoreUI->Draw();

	// PressEnterKeyUI
	pressEnterKeyUI->Draw();
}

// シャドウマップ作成用の描画処理
void GameScene::DrawShadowMap()
{
	// ライトのビュー・プロジェクション行列を計算
	Renderer::SetLightViewProjection(sceneLight->GetViewProjection());

	// カスタムの深度ステンシルステート
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetShadowDepthStencilState().Get(), 1);
	
	// プレイヤー
	player->DrawShadowMap();

	// アイテム
	item->DrawInstancingShadowMap();

	// 壁
	wall->DrawInstancingShadowMap();

	// 地面
	ground->DrawInstancingShadowMap();
}

// フラスタムカリング判定処理
void GameScene::DrawFrustumCheck()
{
	// ビュー・プロジェクション行列から平面を計算
	frustum.ConstructFrustum(camera3D->GetViewProjectionMatrix());
	
	// フラスタムと描画対象の全オブジェクトを
	// コンピュートシェーダーで計算する
	Collision::FrustumCheck(frustum.planes, activeBoundsData);

#ifdef _DEBUG
	// デバッグ時のみフラスタムカリングを切り替え可能
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

// アイテムオブジェクトとの当たり判定処理
void GameScene::CheckItemObjectCollision(GameManager& gm)
{
	// アイテムオブジェクトのバウンディングスフィアの配列
	std::vector<Collision::BoundingSphere> itemBS;

	// プレイヤーの参照を取得
	auto playerPtr = dynamic_cast<Player*>(player.get());

	// ヒットストップ時ではないとき
	if (isHitStop && !playerPtr) return;
	
	for (auto& item : item->items)
	{
		// 判定対象または触れることが可能な場合
		if (!item->GetIsHitCheke() || !item->GetIsTouched()) continue;

		// 当たり判定実行
		if (collisionManager.SphereCollisionPlayerAndItem(*playerPtr, *item))
		{
			// 参照を入れておく（ヒットストップ後にアイテムの処理を実行するため）
			refTempItem = *item;

			// スコア加算
			gm.AddStageScore(static_cast<int>(static_cast<float>(item->GetScore()) * scoreUpRatio));

			// ヒットストップ開始
			StartHitStop();
		}
	}
}

// 地面・壁・坂道オブジェクトとの細かい当たり判定処理
void GameScene::CheckOtherObjectCollision()
{
	//=============================================================//
	// ブロードフェーズで球体同士で判定
	//=============================================================//
	Collision::BoundingSphere pbs = player->GetBoundingSphere();
	// プレイヤーの最新座標を設定
	pbs.center = player->GetNewPosition();

	broadPhaseBoundsData.clear();

	// 地面オブジェクト
	for (auto& ground : ground->boxs)
	{
		if (!ground->GetIsActive()) continue;

		broadPhaseBoundsData.push_back({ ground->GetBoundingSphere(), ground->GetIsCollisionCheck() });
	}

	// 壁オブジェクト
	for (auto& wall : wall->boxs)
	{
		if (!wall->GetIsActive()) continue;

		wall->SetFirstColor();

		broadPhaseBoundsData.push_back({ wall->GetBoundingSphere(), wall->GetIsCollisionCheck() });
	}

	// 坂道オブジェクト
	for (auto& slope : slope->slopes)
	{
		if (!slope->GetIsActive()) continue;

		broadPhaseBoundsData.push_back({ slope->GetBoundingSphere(), slope->GetIsCollisionCheck() });
	}

	Collision::BroadPhaseCheck(pbs, broadPhaseBoundsData);


	//=============================================================//
	// ナローフェーズで細かく判定
	//=============================================================//

	std::vector<Collision::AABB> groundsAABB;               // 地面専用AABB
	std::vector<Collision::AABB> wallsAABB;                 // 壁専用AABB
	std::vector<Collision::Triangle> slopesTriangle;        // 坂道専用三角形平面
	// 壁のみ接触していると赤く色を変えるので参照として渡す
	std::vector<std::reference_wrapper<BoxObject>> walls;  // 壁専用の参照コンテナ
	
	// 地面オブジェクト
	for (auto& ground : ground->boxs)
	{
		if (!ground->GetIsCollisionCheck()) continue;

		groundsAABB.push_back(ground->GetAABB());
	}
	// 壁オブジェクト
	for (auto& wall : wall->boxs)
	{
		if (!wall->GetIsCollisionCheck()) continue;

		wallsAABB.push_back(wall->GetAABB());
		walls.push_back(*wall);
	}
	// 坂道オブジェクト
	for (auto& slope : slope->slopes)
	{
		if (!slope->GetIsCollisionCheck()) continue;

		std::vector<Collision::Triangle> triangles = slope->GetPlanes();
		slopesTriangle.insert(slopesTriangle.end(), triangles.begin(), triangles.end());
	}

	// プレイヤーが移動後の座標に衝突していない場合
	// プレイヤーを正式な座標に移動させるための当たり判定
	collisionManager.MovePlayerCollisionCheck(player, groundsAABB, wallsAABB, slopesTriangle);

	// プレイヤーがジャンプできるようにするための当たり判定
	if (collisionManager.JumpPlayerCollisionCheck(player, groundsAABB, wallsAABB, slopesTriangle))
	{
		// 1つ以上の地面についている場合ジャンプ可能にする
		player->SetIsGround(true);
		player->SetJumpState(PlayerController::CAN_JUMP);
		player->SetVelocityY(0.0f);
		fireFlowerFx->SetIsCanSpawn(true);
	}
	else
	{
		// 1つでも触れていなければジャンプしない
		player->SetIsGround(false);
		fireFlowerFx->SetIsCanSpawn(false);
	}

	// プレイヤーがめり込んだ場合の判定処理・応答
	collisionManager.ExecuteCollisionResponse(player, groundsAABB, walls, slopesTriangle);
}

// アイテムエフェクト再生
void GameScene::ItemEffectPlay(DirectX::XMFLOAT3 _playPos)
{
	// 配列のインデックスを順番に進めて再生する
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
	// 衝突したアイテムの処理をする
	refTempItem->get().SetState(ItemObject::EXPLOSION);
	refTempItem->get().SetFragmentAlpha(0.5f);
	ItemEffectPlay(refTempItem->get().GetPosition()); // エフェクト再生
	refTempItem.reset(); // 参照を解除

	// SE再生
	XA_Play(BALLOON_SE, ItemSEVol * volRatio);
}

// IMGUIの設定
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
	//  プレイヤー
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
	//  カメラ
	//=================================================
	ImGui::Begin("Camera3DParameter");
	ImGui::Text("Eye  x: %.1f y: %.1f z: %.1f", camera3D->GetEye().x, camera3D->GetEye().y, camera3D->GetEye().z);
	ImGui::End();

	//=================================================
	//  パーティクル
	//=================================================
	ImGui::Begin("ParticleParameter");
	ImGui::Text("FireFlowerParticleNum  %d / %d", fireFlowerFx->GetRealParticleNum(),fireFlowerFx->GetMaxParticleNum());
	ImGui::End();

	//=================================================
	//  UI
	//=================================================

	ImGui::Begin("UIParameter");

	// スコア数値UI
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


	// 速度数値UI
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

	// 速度ゲージUI
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


	// 制限時間数値UI
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

	// リザルトスコア数値UI
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

// IMGUIで設定したパラメータをCSVに保存する
void GameScene::SaveParameters()
{
	// パラメータ名と数値とその型mapでまとめる
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

	// CSV保存
	CSVManager::SaveCSVPrameters(params, "Assets/SceneParameterList/GameSceneParameters.csv");
}

// CSVから読み込んだパラメータを反映させる
void GameScene::LoadParameters()
{
	// パラメータ名と数値とその型mapでまとめる
	std::unordered_map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	// ファイル名が存在しない場合はデフォルトの値で保存する
	if (!CSVManager::LoadCSVPrameters(params, "Assets/SceneParameterList/GameSceneParameters.csv"))
	{
		SaveParameters();
		return;
	}

	std::vector<float> accelerations(PlayerController::AccelerationsNum);

	// mapのパラメータを順番に読み込む
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

		// パラメータ名が登録されていない場合
		else
		{
			std::string message = entry.first + " :値読み込みエラー";
			MessageBoxA(NULL, message.c_str(), "ErrorMesseage", MB_OK | MB_ICONERROR);
		}
	}

	player->SetAccelerations(accelerations);
}