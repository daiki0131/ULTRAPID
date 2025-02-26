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

// ����������
void TitleScene::Init()
{
	//=================================================
	//  �I�u�W�F�N�g����
	//=================================================

	// �ǂݍ��񂾃��f�����Ǘ�����z��
	modelManager = std::make_unique<ModelHash>();

	// �J����3D
	camera3D = std::make_unique<TitleSceneCamera>();

#ifdef _DEBUG
	// �f�o�b�O��p�J����3D
	debugCamera3D = std::make_unique<DebugCamera>();
#endif

	// ���C�g
	sceneLight = std::make_unique<Light>(XMFLOAT3(1000.0f, 800.0f, 0.0f));

	// �v���C���[
	player = std::make_unique<Player>(
		XMFLOAT3(-5.0f, -2.0f, 10.0f),
		XMFLOAT3(0.0f, 90.0f, 0.0f),
		XMFLOAT3(0.1f, 0.1f, 0.1f),
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// �^�C�g����UI
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

	// �n�ʃI�u�W�F�N�g
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

	// �A�C�e���I�u�W�F�N�g
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

	// �A�C�e���G�t�F�N�g
	itemFx = std::make_unique<ItemEffect>();

	//=================================================
	//  ���f���E�e�N�X�`���ǂݍ���
	//=================================================

	// �摜�t�@�C��
	const wchar_t* textureFileName[] =
	{
		L"Assets/Texture/title.png",
		L"Assets/Texture/PressEnterKey.png",
		L"Assets/Texture/floor.png",
	};

	// �e�N�X�`���ǂݍ���
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

	// �v���C���[
	model = modelManager->at(L"Assets/Model/Player.obj");
	if (model.texture)
	{
		model.texture.Reset();
	}
	player->SetModel(model);

	// �^�C�g����UI
	model = modelManager->at(L"Assets/Texture/title.png");
	titleTexture->SetModel(model);

	// PressEnterKeyUI
	model = modelManager->at(L"Assets/Texture/PressEnterKey.png");
	pressEnterKeyTexture->SetModel(model);

	// �n�ʃI�u�W�F�N�g
	ground->SetTexture(modelManager->find(L"Assets/Texture/floor.png")->second.texture);

	//=================================================
	//  �����ݒ�
	//=================================================

	// �v���C���[
	player->SetShader(Shader::VS_OBJECT, Shader::PS_SPECULER);
	player->SetMotionBlurShader(Shader::VS_INSTANCING, Shader::PS_SPECULER);
	player->SetBoundingSphere(player->GetPosition(), 1.0f);
	player->SetItemBoundingSphere(player->GetPosition(), 1.0f);
	player->SetSpeedRatio(1.0f);

	// �^�C�g����UI
	titleTexture->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// PressEnterKeyUI
	pressEnterKeyTexture->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// �n��
	ground->SetShader(Shader::VS_INSTANCING_TEXTURE, Shader::PS_SPECULER_TEXTURE);
	Renderer::Material mat;
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 10.0f);
	mat.other = XMFLOAT4(0.2f, 0.0f, 0.0f, 0.0f);
	ground->SetMaterial(mat);

	// �A�C�e��
	item->SetShader(Shader::VS_INSTANCING, Shader::PS_SPECULER);
	item->SetAnimationShader(Shader::VS_ITEM_ANIMATION, Shader::PS_COLOR, Shader::GS_ITEM_ANIMATION);
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 0.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	mat.other = XMFLOAT4(0.02f, 0.0f, 0.0f, 0.0f);
	item->SetMaterial(mat);

	// �A�C�e���G�t�F�N�g
	itemFx->SetUpParticle();
	itemFx->SetShader(Shader::VS_INSTANCING, Shader::PS_COLOR);

	// BGM�Đ�
	XA_Play(TITLE_BGM, TitleBGMVol * volRatio);

}

// �������
void TitleScene::UnInit()
{
	// BGM��~
	XA_Stop(TITLE_BGM);

#ifdef _DEBUG
	SaveParameters();
#endif
}

// �X�V����
void TitleScene::Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm)
{
	// �A�C�e��
	item->Update(_deltaTime);

	Particle::SetCameraPosition(camera3D->GetEye());
	Particle::SetFrustumPlanes(frustum.planes);
	// �A�C�e���G�t�F�N�g
	itemFx->Update(_deltaTime);

	// �v���C���[
	// ���W���Z
	player->AddPosition(XMFLOAT3(playerSpeed, 0.0f, 0.0f));
	// ����̈ʒu�܂ŗ����猳�̍��W�ɖ߂�
	if (player->GetPosition().x >= limitPlayerPositionX)
	{
		player->SetPosition(player->GetStartPosition());
		player->OldPositionsMotionBlurUpdate(player->GetPosition(), true);
	}
	// �X�V
	player->Update(_deltaTime);

	// Enter�L�[�܂��̓Q�[���p�b�hA���t�F�[�h���łȂ��ꍇ
	if ((Input::Get().GetKeyTrigger(DIK_RETURN) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_A)) && !sceneManager.GetIsSceneFade())
	{
		// SE�Đ�
		XA_Play(DECISION_SE, DecisionSEVol * volRatio);

		// �V�[�����X�e�[�W�Z���N�g�V�[���ɐ؂�ւ���
		sceneManager.SetScene(std::make_unique<SelectStageScene>());
	}

	// �����蔻�菈��
	CheckObjectCollision();

	// �f�o�b�O���̂�
#ifdef _DEBUG

	// C�L�[�������ꂽ��
	// ���C���J�����ƃf�o�b�O�J������؂�ւ���
	if (Input::Get().GetKeyTrigger(DIK_C))
	{
		isDebugCameraChange = !isDebugCameraChange;
	}

	// U�L�[�������ꂽ��
	// UI�\���E��\���؂�ւ���
	if (Input::Get().GetKeyTrigger(DIK_U))
	{
		isDebugDrawUI = !isDebugDrawUI;
	}

	// �f�o�b�O�J�����ɐ؂�ւ����̂�
	// �X�V���������s
	if (isDebugCameraChange)
	{
		debugCamera3D->Update(_deltaTime);
	}
#endif

	frustum.ConstructFrustum(camera3D->GetViewProjectionMatrix());

	// �J����3D�X�V
	camera3D->Update(_deltaTime, *this);

	sceneLight->CalculateShadowMatrix(player->GetPosition(), camera3D->GetViewMatrix());
}

// �`�揈��
void TitleScene::Draw(Camera2D& _camera2D)
{
	// 3D�J�����Z�b�g
	Renderer::SetCameraViewProjection(camera3D->GetViewMatrix(), camera3D->GetProjectionMatrix());

	// �f�o�b�O���̂�
#ifdef _DEBUG

	// �f�o�b�O�J�������̂�
	// �f�o�b�O�J�����̃r���[�E�v���W�F�N�V�����s����g�p
	if (isDebugCameraChange)
	{
		Renderer::SetCameraViewProjection(debugCamera3D->GetViewMatrix(), debugCamera3D->GetProjectionMatrix());
	}
#endif
	
	// �f�t�H���g�̐[�x�X�e���V���X�e�[�g
	Renderer::GetContext()->OMSetDepthStencilState(nullptr, 1);

	// �n�ʃC���X�^���V���O�`��
	ground->InstancingWithShadowsDraw();

	// �v���C���[
	player->Draw();

	// �A�C�e���C���X�^���V���O�`��
	item->InstancingDraw();
	
	// �J�X�^���̐[�x�X�e���V���X�e�[�g
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState().Get(), 1);

	// �v���C���[�̃G�t�F�N�g�`��
	player->MortionBlurDraw();

	// �A�C�e���̔��U�A�j���[�V�����`��
	item->ExplosionAnimation();

	// �A�C�e���̃p�[�e�B�N���G�t�F�N�g�`��
	itemFx->Draw();

#ifdef _DEBUG
	// UI��\���̏ꍇ������return
	if (!isDebugDrawUI) return;
#endif

	// 2D�J�����Z�b�g
	Renderer::SetCameraViewProjection(_camera2D.GetViewMatrix(), _camera2D.GetProjectionMatrix());

	// �f�t�H���g�̐[�x�X�e���V���X�e�[�g
	Renderer::GetContext()->OMSetDepthStencilState(nullptr, 1);

	// �^�C�g����UI
	titleTexture->Draw();

	// PressEnterKeyUI
	pressEnterKeyTexture->Draw();
}

void TitleScene::DrawShadowMap()
{
	// ���C�g�̃r���[�E�v���W�F�N�V�����s����v�Z
	Renderer::SetLightViewProjection(sceneLight->GetViewProjection());

	// �J�X�^���̐[�x�X�e���V���X�e�[�g
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetShadowDepthStencilState().Get(), 1);

	player->DrawShadowMap();
	item->DrawInstancingShadowMap();
}

// �V�[�����̓����蔻������s
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


// IMGUI�̐ݒ�
void TitleScene::SetIMGUI()
{

	//=================================================
	//  FPS
	//=================================================
	ImGui::Begin("FPS");
	ImGui::Text("FrameRate (%.1f FPS)", FPS::Get()->GetFPS());
	ImGui::End();

	int nNum;    // int�^�̒l���i�[����
	float fNum;  // float�^�̒l���i�[����

	//=================================================
	//  �t�F�[�h
	//=================================================
	ImGui::Begin("FadeParameter");

	// �t�F�[�h���x�ݒ�
	fNum = SceneManager::GetFade().GetSpeed();
	ImGui::SliderFloat("FadeSpeed", &fNum, 1.0f, 3.0f);
	SceneManager::GetFade().SetSpeed(fNum);

	ImGui::End();

	//=================================================
	//  �J����
	//=================================================
	ImGui::Begin("TitleSceneCameraParameter");

	// Focus����
	XMFLOAT3 fNum3 = camera3D->GetAdjustFocus();
	ImGui::SliderFloat2("MaxAdjustFocus", &fNum3.x, -Camera3D::MaxAdjustFocus, Camera3D::MaxAdjustFocus);
	camera3D->SetAdjustFocus(fNum3);

	// Eye����
	fNum3 = camera3D->GetEye();
	ImGui::SliderFloat("EyeZ", &fNum3.z, Camera3D::MinEyeZ, -1.0f);
	camera3D->SetEye(fNum3);

	ImGui::End();

	//=================================================
	//  �v���C���[
	//=================================================
	ImGui::Begin("PlayerParameter");

	// ���E���WX
	fNum = this->GetLimitPlayerPositionX();
	ImGui::SliderFloat("LimitPlayerPositionX", &fNum, this->MinLimitPos, this->MaxLimitPos);
	this->SetLimitPlayerPositionX(fNum);

	// �ړ����x
	fNum = this->GetPlayerSpeed();
	ImGui::SliderFloat("PlayerSpeed", &fNum, 0.0f, this->MaxLimitSpeed);
	this->SetPlayerSpeed(fNum);

	// ���[�V�����u���[�̎c����
	nNum = player->GetMotionBlurImprintNum();
	ImGui::SliderInt("MotionBlurImprintNum", &nNum, 0, Player::MaxMotionBlurImprintNum);
	player->SetMotionBlurImprintNum(nNum);

	// ���[�V�����u���[�̍ő僿�l
	fNum = player->GetMotionBlurAlphaValue();
	ImGui::SliderFloat("MotionBlurAlphaValue", &fNum, 0.0f, Player::MaxMotionBlurAlphaValue);
	player->SetMotionBlurAlphaValue(fNum);

	// �X�P�[��
	fNum = player->GetScaleValue();
	ImGui::SliderFloat("ScaleValue", &fNum, 0.01f, Player::MaxScale);
	player->SetScaleValue(fNum);

	ImGui::End();

	//=================================================
	//  �A�C�e���G�t�F�N�g
	//=================================================
	ImGui::Begin("ItemFxParameter");

	// �d��
	fNum = itemFx->GetGravity();
	ImGui::SliderFloat("GravityScale", &fNum, 0.0f, Particle::MaxGravity);
	itemFx->SetGravity(fNum);

	// ���q�̃T�C�Y
	fNum = itemFx->GetSize();
	ImGui::SliderFloat("ParticleSize", &fNum, 0.01f, Particle::MaxSize);
	itemFx->SetSize(fNum);

	// �ő呬�x
	fNum = itemFx->GetMaxSpeed();
	if (ImGui::SliderFloat("MaxSpeed", &fNum, 0.1f, Particle::MaxSpeed))
	{
		if (itemFx->GetMinSpeed() > fNum) fNum = itemFx->GetMinSpeed() + 0.01f;
	}
	itemFx->SetMaxSpeed(fNum);

	// �ŏ����x
	fNum = itemFx->GetMinSpeed();
	if (ImGui::SliderFloat("MinSpeed", &fNum, 0.01f, Particle::MinSpeed))
	{
		if (itemFx->GetMaxSpeed() < fNum) fNum = itemFx->GetMaxSpeed() - 0.01f;
	}
	itemFx->SetMinSpeed(fNum);

	// �ő吶������
	fNum = itemFx->GetMaxLifeTime();
	if (ImGui::SliderFloat("MaxLifeTime", &fNum, 0.1f, Particle::MaxLifeTime))
	{
		if (itemFx->GetMinLifeTime() > fNum) fNum = itemFx->GetMinLifeTime() + 0.01f;
	}
	itemFx->SetMaxLifeTime(fNum);

	// �ŏ���������
	fNum = itemFx->GetMinLifeTime();
	if (ImGui::SliderFloat("MinLifeTime", &fNum, 0.01f, Particle::MinLifeTime))
	{
		if (itemFx->GetMaxLifeTime() < fNum) fNum = itemFx->GetMaxLifeTime() - 0.01f;
	}
	itemFx->SetMinLifeTime(fNum);

	// �p�[�e�B�N����
	nNum = itemFx->GetMaxParticleNum();
	if (ImGui::SliderInt("MaxParticleNum", &nNum, 0, Particle::MaxParticleNum))
	{
		itemFx->ParticleStop();
	}
	itemFx->SetMaxParticleNum(nNum);

	// �ő僿�l
	fNum = itemFx->GetMaxAlpha();
	ImGui::SliderFloat("MaxAlpha", &fNum, 0.01f, Particle::MaxAlpha);
	itemFx->SetMaxAlpha(fNum);

	ImGui::End();

	//=================================================
	//  UI
	//=================================================
	ImGui::Begin("UIParameter");

	// �^�C�g����UI���W
	fNum3 = titleTexture->GetPosition();
	if (ImGui::SliderFloat2("TitleTexturePosition", &fNum3.x, -MaxTexPos, MaxTexPos))
	{
		titleTexture->SetPosition(fNum3);
		titleTexture->ReCalculateWorldMatrix();
	}

	// �^�C�g����UI�X�P�[��
	fNum3 = titleTexture->GetScale();
	if (ImGui::SliderFloat2("TitleTextureScale", &fNum3.x, 0.1f, MaxTexScale))
	{
		titleTexture->SetScale(fNum3);
		titleTexture->ReCalculateWorldMatrix();
	}

	// PressEnterKeyUI���W
	fNum3 = pressEnterKeyTexture->GetPosition();
	if (ImGui::SliderFloat2("PressEnterKeyTexturePosition", &fNum3.x, -MaxTexPos, MaxTexPos))
	{
		pressEnterKeyTexture->SetPosition(fNum3);
		pressEnterKeyTexture->ReCalculateWorldMatrix();
	}

	// PressEnterKeyUI�X�P�[��
	fNum3 = pressEnterKeyTexture->GetScale();
	if (ImGui::SliderFloat2("PressEnterKeyTextureScale", &fNum3.x, 0.1f, MaxTexScale))
	{
		pressEnterKeyTexture->SetScale(fNum3);
		pressEnterKeyTexture->ReCalculateWorldMatrix();
	}

	ImGui::End();
}

// IMGUI�Őݒ肵���p�����[�^��CSV�ɕۑ�����
void TitleScene::SaveParameters()
{
	// �p�����[�^���Ɛ��l�Ƃ��̌^map�ł܂Ƃ߂�
	std::map<std::string, std::pair<std::variant<int, float, bool,DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	//=================================================
	//  �t�F�[�h
	//=================================================
	params["SceneFadeSpeed"] = { SceneManager::GetFade().GetSpeed(),CSVManager::TypeName::FLOAT};

	//=================================================
	//  �J����
	//=================================================
	params["CameraMaxAdjustFocus"] = { camera3D->GetAdjustFocus(),CSVManager::TypeName::FLOAT3 };
	params["CameraEye"] = { camera3D->GetEye(),CSVManager::TypeName::FLOAT3};

	//=================================================
	//  �v���C���[
	//=================================================
	params["PlayerLimitPosX"] = { this->GetLimitPlayerPositionX(),CSVManager::TypeName::FLOAT };
	params["PlayerSpeed"] = { this->GetPlayerSpeed(),CSVManager::TypeName::FLOAT};
	params["PlayerMotionBlurImprintNum"] = { player->GetMotionBlurImprintNum(),CSVManager::TypeName::INT};
	params["PlayerMaxMotionBlurAlphaValue"] = { player->GetMotionBlurAlphaValue(),CSVManager::TypeName::FLOAT};
	params["PlayerScaleValue"] = { player->GetScaleValue(),CSVManager::TypeName::FLOAT};

	//=================================================
	//  �A�C�e���G�t�F�N�g
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
	
	// CSV�ۑ�
	CSVManager::SaveCSVPrameters(params,"Assets/SceneParameterList/TitleSceneParameters.csv");
}

// CSV����ǂݍ��񂾃p�����[�^�𔽉f������
void TitleScene::LoadParameters()
{
	// �p�����[�^���Ɛ��l�Ƃ��̌^map�ł܂Ƃ߂�
	std::unordered_map<std::string, std::pair<std::variant<int, float, bool,DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	// �t�@�C���������݂��Ȃ��ꍇ�̓f�t�H���g�̒l�ŕۑ�����
	if (!CSVManager::LoadCSVPrameters(params, "Assets/SceneParameterList/TitleSceneParameters.csv"))
	{
		SaveParameters();
		return;
	}

	// map�̃p�����[�^�����Ԃɓǂݍ���
	for (const auto& entry : params)
	{
		//=================================================
		//  �J����
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
		//  �t�F�[�h
		//=================================================
		else if (entry.first == "SceneFadeSpeed")
		{
			SceneManager::GetFade().SetSpeed(std::get<float>(entry.second.first));
		}
		//=================================================
		//  �v���C���[
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
		//  �A�C�e���G�t�F�N�g
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
		// �p�����[�^�����o�^����Ă��Ȃ��ꍇ
		else
		{
			std::string message = entry.first + " :�p�����[�^�����o�^����Ă��܂���B:TitleScene::LoadParameters���Ŕ���";
			 DebugConsole::DebugLog(message.c_str(), DebugConsole::WARNING_LOG);
		}
	}
}