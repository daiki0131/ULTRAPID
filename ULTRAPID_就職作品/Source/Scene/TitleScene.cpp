#include "TitleScene.h"
#include "Input.h"
#include "XInputGamePad.h"
#include "Fps.h"
#include "xa2.h"
#include "SelectStageScene.h"
#include "Geometry.h"
#include "DebugUI.h"
#include "CSVManager.h"
#include "DebugConsole.h"

using namespace DirectX;

// 初期化処理
void TitleScene::Init()
{
	//=================================================
	//  オブジェクト生成
	//=================================================

	// 読み込んだモデルを管理する配列
	modelManager = std::make_unique<ModelHash>();

	// カメラ3D
	camera3D = std::make_unique<TitleSceneCamera>();

#ifdef _DEBUG
	// デバッグ専用カメラ3D
	debugCamera3D = std::make_unique<DebugCamera>();
#endif

	// ライト
	sceneLight = std::make_unique<Light>(XMFLOAT3(1000.0f, 800.0f, 0.0f));

	// プレイヤー
	player = std::make_unique<Player>(
		XMFLOAT3(-5.0f, -2.0f, 10.0f),
		XMFLOAT3(0.0f, 90.0f, 0.0f),
		XMFLOAT3(0.1f, 0.1f, 0.1f),
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// タイトル名UI
	titleTexture = std::make_unique<Object2D>(
		XMFLOAT3(-1.2f, 0.8f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(2.3f, 0.75f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	// PressEnterKeyUI
	pressEnterKeyTexture = std::make_unique<Object2D>(
		XMFLOAT3(1.3f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.5f, 0.4f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	// 地面オブジェクト
	std::vector<Boxs::BoxData> groundData;
	groundData.resize(11);
	groundData[0].position = { -10.0f,-3.5f,10.0f };
	groundData[1].position = { 10.0f,-3.5f,10.0f };
	groundData[2].position = { 30.0f,-3.5f,10.0f };
	groundData[3].position = { 50.0f,-3.5f,10.0f };
	groundData[4].position = { 70.0f,-3.5f,10.0f };
	groundData[5].position = { 90.0f,-3.5f,10.0f };
	groundData[6].position = { 110.0f,-3.5f,10.0f };
	groundData[7].position = { 130.0f,-3.5f,10.0f };
	groundData[8].position = { 150.0f,-3.5f,10.0f };
	groundData[9].position = { 170.0f,-3.5f,10.0f };
	groundData[10].position = { 190.0f,-3.5f,10.0f };

	for (auto& data : groundData)
	{
		data.rotation = { 0.0f,0.0f,0.0f };
		data.scaling = { 20.0f,1.0f,20.0f };
		data.color = { 0.2f,0.2f,0.8f,1.0f };
	}

	ground = std::make_unique<Boxs>(groundData);

	// アイテムオブジェクト
	std::vector<Items::ItemData> itemData;
	itemData.resize(5);
	itemData[0].position = { 10.0f,-1.5f,10.0f };
	itemData[1].position = { 50.0f,-1.5f,10.0f };
	itemData[2].position = { 90.0f,-1.5f,10.0f };
	itemData[3].position = { 130.0f,-1.5f,10.0f };
	itemData[4].position = { 170.0f,-1.5f,10.0f };

	for (auto& data : itemData)
	{
		data.rotation = { 0.0f,0.0f,0.0f };
		data.scaling = { 3.0f,3.0f,3.0f };
		data.color = { 0.0f,1.0f,1.0f,1.0f };
	}

	item = std::make_unique<Items>(itemData);

	// アイテムエフェクト
	itemFx = std::make_unique<ItemEffect>();

	//=================================================
	//  モデル・テクスチャ読み込み
	//=================================================

	// 画像ファイル
	const wchar_t* textureFileName[] =
	{
		L"Assets/Texture/title.png",
		L"Assets/Texture/PressEnterKey.png",
		L"Assets/Texture/floor.png",
	};

	// テクスチャ読み込み
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

	// プレイヤー
	model = modelManager->at(L"Assets/Model/Player.obj");
	if (model.texture)
	{
		model.texture.Reset();
	}
	player->SetModel(model);

	// タイトル名UI
	model = modelManager->at(L"Assets/Texture/title.png");
	titleTexture->SetModel(model);

	// PressEnterKeyUI
	model = modelManager->at(L"Assets/Texture/PressEnterKey.png");
	pressEnterKeyTexture->SetModel(model);

	// 地面オブジェクト
	ground->SetTexture(modelManager->find(L"Assets/Texture/floor.png")->second.texture);

	//=================================================
	//  初期設定
	//=================================================

	// プレイヤー
	player->SetShader(Shader::VS_OBJECT, Shader::PS_SPECULER);
	player->SetMotionBlurShader(Shader::VS_INSTANCING, Shader::PS_SPECULER);
	player->SetBoundingSphere(player->GetPosition(), 1.0f);
	player->SetItemBoundingSphere(player->GetPosition(), 1.0f);
	player->SetSpeedRatio(1.0f);

	// タイトル名UI
	titleTexture->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// PressEnterKeyUI
	pressEnterKeyTexture->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// 地面
	ground->SetShader(Shader::VS_INSTANCING_TEXTURE, Shader::PS_SPECULER_TEXTURE);
	Renderer::Material mat;
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 10.0f);
	mat.other = XMFLOAT4(0.2f, 0.0f, 0.0f, 0.0f);
	ground->SetMaterial(mat);

	// アイテム
	item->SetShader(Shader::VS_INSTANCING, Shader::PS_SPECULER);
	item->SetAnimationShader(Shader::VS_ITEM_ANIMATION, Shader::PS_COLOR, Shader::GS_ITEM_ANIMATION);
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 0.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	mat.other = XMFLOAT4(0.02f, 0.0f, 0.0f, 0.0f);
	item->SetMaterial(mat);

	// アイテムエフェクト
	itemFx->SetUpParticle();
	itemFx->SetShader(Shader::VS_INSTANCING, Shader::PS_COLOR);

	// BGM再生
	XA_Play(TITLE_BGM, TitleBGMVol * volRatio);

}

// 解放処理
void TitleScene::UnInit()
{
	// BGM停止
	XA_Stop(TITLE_BGM);

#ifdef _DEBUG
	SaveParameters();
#endif
}

// 更新処理
void TitleScene::Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm)
{
	// アイテム
	item->Update(_deltaTime);

	Particle::SetCameraPosition(camera3D->GetEye());
	Particle::SetFrustumPlanes(frustum.planes);
	// アイテムエフェクト
	itemFx->Update(_deltaTime);

	// プレイヤー
	// 座標加算
	player->AddPosition(XMFLOAT3(playerSpeed, 0.0f, 0.0f));
	// 特定の位置まで来たら元の座標に戻す
	if (player->GetPosition().x >= limitPlayerPositionX)
	{
		player->SetPosition(player->GetStartPosition());
		player->OldPositionsMotionBlurUpdate(player->GetPosition(), true);
	}
	// 更新
	player->Update(_deltaTime);

	// EnterキーまたはゲームパッドAかつフェード中でない場合
	if ((Input::Get().GetKeyTrigger(DIK_RETURN) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_A)) && !sceneManager.GetIsSceneFade())
	{
		// SE再生
		XA_Play(DECISION_SE, DecisionSEVol * volRatio);

		// シーンをステージセレクトシーンに切り替える
		sceneManager.SetScene(std::make_unique<SelectStageScene>());
	}

	// 当たり判定処理
	CheckObjectCollision();

	// デバッグ時のみ
#ifdef _DEBUG

	// Cキーが押されたら
	// メインカメラとデバッグカメラを切り替える
	if (Input::Get().GetKeyTrigger(DIK_C))
	{
		isDebugCameraChange = !isDebugCameraChange;
	}

	// Uキーが押されたら
	// UI表示・非表示切り替える
	if (Input::Get().GetKeyTrigger(DIK_U))
	{
		isDebugDrawUI = !isDebugDrawUI;
	}

	// デバッグカメラに切り替え時のみ
	// 更新処理を実行
	if (isDebugCameraChange)
	{
		debugCamera3D->Update(_deltaTime);
	}
#endif

	frustum.ConstructFrustum(camera3D->GetViewProjectionMatrix());

	// カメラ3D更新
	camera3D->Update(_deltaTime, *this);

	sceneLight->CalculateShadowMatrix(player->GetPosition(), camera3D->GetViewMatrix());
}

// 描画処理
void TitleScene::Draw(Camera2D& _camera2D)
{
	// 3Dカメラセット
	Renderer::SetCameraViewProjection(camera3D->GetViewMatrix(), camera3D->GetProjectionMatrix());

	// デバッグ時のみ
#ifdef _DEBUG

	// デバッグカメラ時のみ
	// デバッグカメラのビュー・プロジェクション行列を使用
	if (isDebugCameraChange)
	{
		Renderer::SetCameraViewProjection(debugCamera3D->GetViewMatrix(), debugCamera3D->GetProjectionMatrix());
	}
#endif
	
	// デフォルトの深度ステンシルステート
	Renderer::GetContext()->OMSetDepthStencilState(nullptr, 1);

	// 地面インスタンシング描画
	ground->InstancingWithShadowsDraw();

	// プレイヤー
	player->Draw();

	// アイテムインスタンシング描画
	item->InstancingDraw();
	
	// カスタムの深度ステンシルステート
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState().Get(), 1);

	// プレイヤーのエフェクト描画
	player->MortionBlurDraw();

	// アイテムの爆散アニメーション描画
	item->ExplosionAnimation();

	// アイテムのパーティクルエフェクト描画
	itemFx->Draw();

#ifdef _DEBUG
	// UI非表示の場合ここでreturn
	if (!isDebugDrawUI) return;
#endif

	// 2Dカメラセット
	Renderer::SetCameraViewProjection(_camera2D.GetViewMatrix(), _camera2D.GetProjectionMatrix());

	// デフォルトの深度ステンシルステート
	Renderer::GetContext()->OMSetDepthStencilState(nullptr, 1);

	// タイトル名UI
	titleTexture->Draw();

	// PressEnterKeyUI
	pressEnterKeyTexture->Draw();
}

void TitleScene::DrawShadowMap()
{
	// ライトのビュー・プロジェクション行列を計算
	Renderer::SetLightViewProjection(sceneLight->GetViewProjection());

	// カスタムの深度ステンシルステート
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetShadowDepthStencilState().Get(), 1);

	player->DrawShadowMap();
	item->DrawInstancingShadowMap();
}

// シーン内の当たり判定を実行
void TitleScene::CheckObjectCollision()
{
	if (!player) return;

	for (auto& item : item->items)
	{
		if (!item->GetIsTouched()) continue;

		if (collisionManager.SphereCollisionPlayerAndItem(*player, *item))
		{
			item->SetState(ItemObject::EXPLOSION);
			item->SetFragmentAlpha(0.5f);
			itemFx->ParticlePlay(item->GetPosition());
		}
	}
}


// IMGUIの設定
void TitleScene::SetIMGUI()
{

	//=================================================
	//  FPS
	//=================================================
	ImGui::Begin("FPS");
	ImGui::Text("FrameRate (%.1f FPS)", FPS::Get()->GetFPS());
	ImGui::End();

	int nNum;    // int型の値を格納する
	float fNum;  // float型の値を格納する

	//=================================================
	//  フェード
	//=================================================
	ImGui::Begin("FadeParameter");

	// フェード速度設定
	fNum = SceneManager::GetFade().GetSpeed();
	ImGui::SliderFloat("FadeSpeed", &fNum, 1.0f, 3.0f);
	SceneManager::GetFade().SetSpeed(fNum);

	ImGui::End();

	//=================================================
	//  カメラ
	//=================================================
	ImGui::Begin("TitleSceneCameraParameter");

	// Focus調整
	XMFLOAT3 fNum3 = camera3D->GetAdjustFocus();
	ImGui::SliderFloat2("MaxAdjustFocus", &fNum3.x, -Camera3D::MaxAdjustFocus, Camera3D::MaxAdjustFocus);
	camera3D->SetAdjustFocus(fNum3);

	// Eye調整
	fNum3 = camera3D->GetEye();
	ImGui::SliderFloat("EyeZ", &fNum3.z, Camera3D::MinEyeZ, -1.0f);
	camera3D->SetEye(fNum3);

	ImGui::End();

	//=================================================
	//  プレイヤー
	//=================================================
	ImGui::Begin("PlayerParameter");

	// 限界座標X
	fNum = this->GetLimitPlayerPositionX();
	ImGui::SliderFloat("LimitPlayerPositionX", &fNum, this->MinLimitPos, this->MaxLimitPos);
	this->SetLimitPlayerPositionX(fNum);

	// 移動速度
	fNum = this->GetPlayerSpeed();
	ImGui::SliderFloat("PlayerSpeed", &fNum, 0.0f, this->MaxLimitSpeed);
	this->SetPlayerSpeed(fNum);

	// モーションブラーの残像数
	nNum = player->GetMotionBlurImprintNum();
	ImGui::SliderInt("MotionBlurImprintNum", &nNum, 0, Player::MaxMotionBlurImprintNum);
	player->SetMotionBlurImprintNum(nNum);

	// モーションブラーの最大α値
	fNum = player->GetMotionBlurAlphaValue();
	ImGui::SliderFloat("MotionBlurAlphaValue", &fNum, 0.0f, Player::MaxMotionBlurAlphaValue);
	player->SetMotionBlurAlphaValue(fNum);

	// スケール
	fNum = player->GetScaleValue();
	ImGui::SliderFloat("ScaleValue", &fNum, 0.01f, Player::MaxScale);
	player->SetScaleValue(fNum);

	ImGui::End();

	//=================================================
	//  アイテムエフェクト
	//=================================================
	ImGui::Begin("ItemFxParameter");

	// 重力
	fNum = itemFx->GetGravity();
	ImGui::SliderFloat("GravityScale", &fNum, 0.0f, Particle::MaxGravity);
	itemFx->SetGravity(fNum);

	// 粒子のサイズ
	fNum = itemFx->GetSize();
	ImGui::SliderFloat("ParticleSize", &fNum, 0.01f, Particle::MaxSize);
	itemFx->SetSize(fNum);

	// 最大速度
	fNum = itemFx->GetMaxSpeed();
	if (ImGui::SliderFloat("MaxSpeed", &fNum, 0.1f, Particle::MaxSpeed))
	{
		if (itemFx->GetMinSpeed() > fNum) fNum = itemFx->GetMinSpeed() + 0.01f;
	}
	itemFx->SetMaxSpeed(fNum);

	// 最小速度
	fNum = itemFx->GetMinSpeed();
	if (ImGui::SliderFloat("MinSpeed", &fNum, 0.01f, Particle::MinSpeed))
	{
		if (itemFx->GetMaxSpeed() < fNum) fNum = itemFx->GetMaxSpeed() - 0.01f;
	}
	itemFx->SetMinSpeed(fNum);

	// 最大生存時間
	fNum = itemFx->GetMaxLifeTime();
	if (ImGui::SliderFloat("MaxLifeTime", &fNum, 0.1f, Particle::MaxLifeTime))
	{
		if (itemFx->GetMinLifeTime() > fNum) fNum = itemFx->GetMinLifeTime() + 0.01f;
	}
	itemFx->SetMaxLifeTime(fNum);

	// 最小生存時間
	fNum = itemFx->GetMinLifeTime();
	if (ImGui::SliderFloat("MinLifeTime", &fNum, 0.01f, Particle::MinLifeTime))
	{
		if (itemFx->GetMaxLifeTime() < fNum) fNum = itemFx->GetMaxLifeTime() - 0.01f;
	}
	itemFx->SetMinLifeTime(fNum);

	// パーティクル数
	nNum = itemFx->GetMaxParticleNum();
	if (ImGui::SliderInt("MaxParticleNum", &nNum, 0, Particle::MaxParticleNum))
	{
		itemFx->ParticleStop();
	}
	itemFx->SetMaxParticleNum(nNum);

	// 最大α値
	fNum = itemFx->GetMaxAlpha();
	ImGui::SliderFloat("MaxAlpha", &fNum, 0.01f, Particle::MaxAlpha);
	itemFx->SetMaxAlpha(fNum);

	ImGui::End();

	//=================================================
	//  UI
	//=================================================
	ImGui::Begin("UIParameter");

	// タイトル名UI座標
	fNum3 = titleTexture->GetPosition();
	if (ImGui::SliderFloat2("TitleTexturePosition", &fNum3.x, -MaxTexPos, MaxTexPos))
	{
		titleTexture->SetPosition(fNum3);
		titleTexture->ReCalculateWorldMatrix();
	}

	// タイトル名UIスケール
	fNum3 = titleTexture->GetScale();
	if (ImGui::SliderFloat2("TitleTextureScale", &fNum3.x, 0.1f, MaxTexScale))
	{
		titleTexture->SetScale(fNum3);
		titleTexture->ReCalculateWorldMatrix();
	}

	// PressEnterKeyUI座標
	fNum3 = pressEnterKeyTexture->GetPosition();
	if (ImGui::SliderFloat2("PressEnterKeyTexturePosition", &fNum3.x, -MaxTexPos, MaxTexPos))
	{
		pressEnterKeyTexture->SetPosition(fNum3);
		pressEnterKeyTexture->ReCalculateWorldMatrix();
	}

	// PressEnterKeyUIスケール
	fNum3 = pressEnterKeyTexture->GetScale();
	if (ImGui::SliderFloat2("PressEnterKeyTextureScale", &fNum3.x, 0.1f, MaxTexScale))
	{
		pressEnterKeyTexture->SetScale(fNum3);
		pressEnterKeyTexture->ReCalculateWorldMatrix();
	}

	ImGui::End();
}

// IMGUIで設定したパラメータをCSVに保存する
void TitleScene::SaveParameters()
{
	// パラメータ名と数値とその型mapでまとめる
	std::map<std::string, std::pair<std::variant<int, float, bool,DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	//=================================================
	//  フェード
	//=================================================
	params["SceneFadeSpeed"] = { SceneManager::GetFade().GetSpeed(),CSVManager::TypeName::FLOAT};

	//=================================================
	//  カメラ
	//=================================================
	params["CameraMaxAdjustFocus"] = { camera3D->GetAdjustFocus(),CSVManager::TypeName::FLOAT3 };
	params["CameraEye"] = { camera3D->GetEye(),CSVManager::TypeName::FLOAT3};

	//=================================================
	//  プレイヤー
	//=================================================
	params["PlayerLimitPosX"] = { this->GetLimitPlayerPositionX(),CSVManager::TypeName::FLOAT };
	params["PlayerSpeed"] = { this->GetPlayerSpeed(),CSVManager::TypeName::FLOAT};
	params["PlayerMotionBlurImprintNum"] = { player->GetMotionBlurImprintNum(),CSVManager::TypeName::INT};
	params["PlayerMaxMotionBlurAlphaValue"] = { player->GetMotionBlurAlphaValue(),CSVManager::TypeName::FLOAT};
	params["PlayerScaleValue"] = { player->GetScaleValue(),CSVManager::TypeName::FLOAT};

	//=================================================
	//  アイテムエフェクト
	//=================================================
	params["ItemFxGravityScale"] = { itemFx->GetGravity(),CSVManager::TypeName::FLOAT};
	params["ItemFxParticleSize"] = { itemFx->GetSize(),CSVManager::TypeName::FLOAT};
	params["ItemFxMaxSpeed"] = { itemFx->GetMaxSpeed(),CSVManager::TypeName::FLOAT};
	params["ItemFxMinSpeed"] = { itemFx->GetMinSpeed(),CSVManager::TypeName::FLOAT };
	params["ItemFxMaxLifeTime"] = { itemFx->GetMaxLifeTime(),CSVManager::TypeName::FLOAT };
	params["ItemFxMinLifeTime"] = { itemFx->GetMaxLifeTime(),CSVManager::TypeName::FLOAT };
	params["ItemFxMaxParticleNum"] = { itemFx->GetMaxParticleNum(),CSVManager::TypeName::INT};
	params["ItemFxMaxAlpha"] = { itemFx->GetMaxAlpha(),CSVManager::TypeName::FLOAT};

	//=================================================
	//  UI
	//=================================================
	params["UITitleTexturePosition"] = { titleTexture->GetPosition(),CSVManager::TypeName::FLOAT3 };
	params["UITitleTextureScale"] = { titleTexture->GetScale(),CSVManager::TypeName::FLOAT3 };
	params["UIPressEnterKeyTexturePosition"] = { pressEnterKeyTexture->GetPosition(),CSVManager::TypeName::FLOAT3 };
	params["UIPressEnterKeyTextureScale"] = { pressEnterKeyTexture->GetScale(),CSVManager::TypeName::FLOAT3 };
	
	// CSV保存
	CSVManager::SaveCSVPrameters(params,"Assets/SceneParameterList/TitleSceneParameters.csv");
}

// CSVから読み込んだパラメータを反映させる
void TitleScene::LoadParameters()
{
	// パラメータ名と数値とその型mapでまとめる
	std::unordered_map<std::string, std::pair<std::variant<int, float, bool,DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	// ファイル名が存在しない場合はデフォルトの値で保存する
	if (!CSVManager::LoadCSVPrameters(params, "Assets/SceneParameterList/TitleSceneParameters.csv"))
	{
		SaveParameters();
		return;
	}

	// mapのパラメータを順番に読み込む
	for (const auto& entry : params)
	{
		//=================================================
		//  カメラ
		//=================================================
		if (entry.first == "CameraMaxAdjustFocus")
		{
			camera3D->SetAdjustFocus(std::get<XMFLOAT3>(entry.second.first));
		}
		else if(entry.first == "CameraEye")
		{
			camera3D->SetEye(std::get<XMFLOAT3>(entry.second.first));
		}
		//=================================================
		//  フェード
		//=================================================
		else if (entry.first == "SceneFadeSpeed")
		{
			SceneManager::GetFade().SetSpeed(std::get<float>(entry.second.first));
		}
		//=================================================
		//  プレイヤー
		//=================================================
		else if (entry.first == "PlayerLimitPosX")
		{
			this->SetLimitPlayerPositionX(std::get<float>(entry.second.first));
		}
		else if (entry.first == "PlayerSpeed")
		{
			this->SetPlayerSpeed(std::get<float>(entry.second.first));
		}
		else if (entry.first == "PlayerMotionBlurImprintNum")
		{
			player->SetMotionBlurImprintNum(std::get<int>(entry.second.first));
		}
		else if (entry.first == "PlayerMaxMotionBlurAlphaValue")
		{
			player->SetMotionBlurAlphaValue(std::get<float>(entry.second.first));
		}
		else if (entry.first == "PlayerScaleValue")
		{
			player->SetScaleValue(std::get<float>(entry.second.first));
		}
		//=================================================
		//  アイテムエフェクト
		//=================================================
		else if (entry.first == "ItemFxGravityScale")
		{
			itemFx->SetGravity(std::get<float>(entry.second.first));
		}
		else if (entry.first == "ItemFxParticleSize")
		{
			itemFx->SetSize(std::get<float>(entry.second.first));
		}
		else if (entry.first == "ItemFxMaxSpeed")
		{
			itemFx->SetMaxSpeed(std::get<float>(entry.second.first));
		}
		else if (entry.first == "ItemFxMinSpeed")
		{
			itemFx->SetMinSpeed(std::get<float>(entry.second.first));
		}
		else if (entry.first == "ItemFxMaxLifeTime")
		{
			itemFx->SetMaxLifeTime(std::get<float>(entry.second.first));
		}
		else if (entry.first == "ItemFxMinLifeTime")
		{
			itemFx->SetMinLifeTime(std::get<float>(entry.second.first));
		}
		else if (entry.first == "ItemFxMaxParticleNum")
		{
			itemFx->SetMaxParticleNum(std::get<int>(entry.second.first));
		}
		else if (entry.first == "ItemFxMaxAlpha")
		{
			itemFx->SetMaxAlpha(std::get<float>(entry.second.first));
		}
		//=================================================
		//  UI
		//=================================================
		else if (entry.first == "UITitleTexturePosition")
		{
			titleTexture->SetPosition(std::get<XMFLOAT3>(entry.second.first));
			titleTexture->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UITitleTextureScale")
		{
			titleTexture->SetScale(std::get<XMFLOAT3>(entry.second.first));
			titleTexture->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UIPressEnterKeyTexturePosition")
		{
			pressEnterKeyTexture->SetPosition(std::get<XMFLOAT3>(entry.second.first));
			pressEnterKeyTexture->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UIPressEnterKeyTextureScale")
		{
			pressEnterKeyTexture->SetScale(std::get<XMFLOAT3>(entry.second.first));
			pressEnterKeyTexture->ReCalculateWorldMatrix();
		}
		// パラメータ名が登録されていない場合
		else
		{
			std::string message = entry.first + " :パラメータ名が登録されていません。:TitleScene::LoadParameters内で発生";
			 DebugConsole::DebugLog(message.c_str(), DebugConsole::WARNING_LOG);
		}
	}
}