#include "SelectStageScene.h"
#include "Input.h"
#include "XInputGamePad.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "DebugUI.h"
#include "Fps.h"
#include "Geometry.h"
#include "CSVManager.h"
#include "xa2.h"

using namespace DirectX;

// ����������
void SelectStageScene::Init()
{
	//=================================================
	//  �I�u�W�F�N�g����
	//=================================================

	// �ǂݍ��񂾃��f�����Ǘ�����z��
	modelManager = std::make_unique<ModelHash>();

	// SelectStageUI
	selectStageTexture = std::make_unique<Object2D>(
		XMFLOAT3(-1.4f, 1.1f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.65f, 0.4f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	// BestScoreUI
	bestScoreTexture = std::make_unique<Object2D>(
		XMFLOAT3(1.03f, -0.6f, -0.21f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.9f, 0.3f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	// �X�R�A���lUI
	scoreUI = std::make_unique<ScoreUI>();
	
	// �X�e�[�W��UI
	stageNames.resize(GameManager::STAGE_MAX);
	for (auto& stage : stageNames)
	{
		stage = std::make_unique<Object2D>(
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.5f, 0.35f, 1.0f),
			XMFLOAT4(1, 1, 1, 1));
	}

	// �X�e�[�W�̉摜
	stageMapPhoto.resize(GameManager::STAGE_MAX);
	for (auto& mapPhoto : stageMapPhoto)
	{
		mapPhoto = std::make_unique<Object2D>(
			XMFLOAT3(1.04f, 0.4f, -0.19f),
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.8f, 1.01f, 1.0f),
			XMFLOAT4(1, 1, 1, 1));
	}

	// �t���[��1
	frame01 = std::make_unique<Object2D>(
		XMFLOAT3(1.04f, -0.9f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.8f, 0.6f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	// �t���[��2
	frame02 = std::make_unique<Object2D>(
		XMFLOAT3(1.04f, 0.4f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(2.7f, 1.52f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	//=================================================
	//  ���f���E�e�N�X�`���ǂݍ���
	//=================================================

	// �摜�t�@�C��
	const wchar_t* textureFileName[] =
	{
		L"Assets/Texture/SelectStage.png",
		L"Assets/Texture/Stage1.png",
		L"Assets/Texture/Stage2.png",
		L"Assets/Texture/Stage3.png",
		L"Assets/Texture/StageMapPhoto01.png",
		L"Assets/Texture/StageMapPhoto02.png",
		L"Assets/Texture/StageMapPhoto03.png",
		L"Assets/Texture/frame.png",
		L"Assets/Texture/BestScore.png",
	};
	// �e�N�X�`���ǂݍ���
	for (int i = 0; i < ARRAYSIZE(textureFileName); i++)
	{
		model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1,1 });
		model.texture = Renderer::LoadTexture(textureFileName[i]);
		modelManager->emplace(textureFileName[i], model);
	}

	//=================================================
	//  ���f���Z�b�g
	//=================================================
	
	// SelectStageUI
	model = modelManager->at(L"Assets/Texture/SelectStage.png");
	selectStageTexture->SetModel(model);

	// BestScoreUI
	model = modelManager->at(L"Assets/Texture/BestScore.png");
	bestScoreTexture->SetModel(model);

	// �X�e�[�W��UI
	model = modelManager->at(L"Assets/Texture/Stage1.png");
	stageNames[0]->SetModel(model);
	model = modelManager->at(L"Assets/Texture/Stage2.png");
	stageNames[1]->SetModel(model);
	model = modelManager->at(L"Assets/Texture/Stage3.png");
	stageNames[2]->SetModel(model);

	// �X�e�[�W�̉摜
	model = modelManager->at(L"Assets/Texture/StageMapPhoto01.png");
	stageMapPhoto[0]->SetModel(model);
	model = modelManager->at(L"Assets/Texture/StageMapPhoto02.png");
	stageMapPhoto[1]->SetModel(model);
	model = modelManager->at(L"Assets/Texture/StageMapPhoto03.png");
	stageMapPhoto[2]->SetModel(model);

	// �t���[��
	model = modelManager->at(L"Assets/Texture/frame.png");
	frame01->SetModel(model);
	frame02->SetModel(model);

	//=================================================
	//  �����ݒ�
	//=================================================

	// SelectStageUI
	selectStageTexture->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// BestScoreUI
	bestScoreTexture->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// �X�e�[�W��UI
	for (auto& stage : stageNames)
	{
		stage->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	}

	// �X�e�[�W�̉摜
	for (auto& mapPhoto : stageMapPhoto)
	{
		mapPhoto->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	}

	// �t���[��
	frame01->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	frame02->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	
	// BGM�Đ�
	XA_Play(SELECT_STAGE_BGM, SelectStageBGMVol * volRatio);
}

// �������
void SelectStageScene::UnInit()
{
	// BGM��~
	XA_Stop(SELECT_STAGE_BGM);

#ifdef _DEBUG
	SaveParameters();
#endif
}

// �X�V����
void SelectStageScene::Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm)
{
	// Enter�L�[�܂��̓Q�[���p�b�hA���t�F�[�h���łȂ��ꍇ
	if ((Input::Get().GetKeyTrigger(DIK_RETURN) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_A)) && !sceneManager.GetIsSceneFade())
	{
		XA_Play(DECISION_SE, DecisionSEVol * volRatio);
		gm.SetSelectStage(static_cast<GameManager::SELECT_STAGE>(selectStageCount));

		// GameScene�ǂݍ���
		sceneManager.SetScene(std::make_unique<GameScene>());
	}

	// BackSpace�L�[�܂��̓Q�[���p�b�hB���t�F�[�h���łȂ��ꍇ
	if ((Input::Get().GetKeyTrigger(DIK_BACKSPACE) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_B)) && !sceneManager.GetIsSceneFade())
	{
		XA_Play(CANCEL_SE, CancelSEVol * volRatio);

		// TitleScene�ǂݍ���
		sceneManager.SetScene(std::make_unique<TitleScene>());
	}

	// �Q�[���p�b�h�̃A�i���O�X�e�B�b�N�̏����擾
	XInputGamePad::AnalogStick analogStick = { 0,0 };

	// �Q�[���p�b�h�̍��X�e�B�b�N��Y���̃g���K�[��Ԃ��擾
	if (XInputGamePad::Get().GetStickTriggerLY())
	{
		analogStick = XInputGamePad::Get().GetAnalogStick();
	}

	// �X�e�[�W2�ȏオ�I����Ԃ̂Ƃ�
	if (selectStageCount > 0)
	{
		// W�L�[�܂��̓Q�[���p�b�h���X�e�B�b�NY���ォ�t�F�[�h��ԂłȂ��Ƃ�
		if ((Input::Get().GetKeyTrigger(DIK_W) || analogStick.LY > 0.0f) && !sceneManager.GetIsSceneFade())
		{
			XA_Play(SELECT_SE, SelectSEVol * volRatio);
			selectStageCount--;
		}
	}
	// �X�e�[�W�ő吔���Ⴂ�ꍇ
	if (selectStageCount < stageNames.size() - 1)
	{
		// S�L�[�܂��̓Q�[���p�b�h���X�e�B�b�NY�������t�F�[�h��ԂłȂ��Ƃ�
		if ((Input::Get().GetKeyTrigger(DIK_S) || analogStick.LY < 0.0f) && !sceneManager.GetIsSceneFade())
		{
			XA_Play(SELECT_SE, SelectSEVol * volRatio);
			selectStageCount++;
		}
	}

	// �I���X�e�[�W�ɉ����ĊeUI�̏�Ԃ�؂�ւ���
	for (int i = 0; i < stageNames.size(); i++)
	{
		// ���݂̑I���X�e�[�W
		if (i == selectStageCount)
		{
			stageNames[i]->SetPosition(nameMiddlePosition); // ����
			stageNames[i]->SetScale(nameMiddleScale);
			stageNames[i]->SetColorAlpha(1.0f);
			stageNames[i]->SetIsActive(true);

			// ���̃X�e�[�W�̃X�R�A������\��������
			selectStageScore = gm.GetScore(static_cast<GameManager::SELECT_STAGE>(selectStageCount));
		}
		// �I���X�e�[�W��1��̃X�e�[�W�i��F�I���X�e�[�W2�̏ꍇ�X�e�[�W1�j
		else if (i == selectStageCount - 1)
		{
			stageNames[i]->SetPosition(nameUpPosition); // ��
			stageNames[i]->SetScale(nameUpDownScale);
			stageNames[i]->SetColorAlpha(Object2D::UiLowAlpha);
			stageNames[i]->SetIsActive(true);
		}
		// �I���X�e�[�W��1���̃X�e�[�W�i��F�I���X�e�[�W2�̏ꍇ�X�e�[�W3�j
		else if (i == selectStageCount + 1)
		{
			stageNames[i]->SetPosition(nameDownPosition); // ��
			stageNames[i]->SetScale(nameUpDownScale);
			stageNames[i]->SetColorAlpha(Object2D::UiLowAlpha);
			stageNames[i]->SetIsActive(true);
		}
		else
		{
			// ��L�ȊO�̏ꍇ�`�悵�Ȃ�
			stageNames[i]->SetIsActive(false);
		}
	}

	// �X�e�[�W�̉摜�̍X�V
	for (int i = 0; i < stageMapPhoto.size(); i++)
	{
		if (i == selectStageCount)
		{
			// ���݂̑I���X�e�[�W�̉摜�̂ݕ`�悷��
			stageMapPhoto[i]->SetIsActive(true);
		}
		else
		{
			stageMapPhoto[i]->SetIsActive(false);
		}
	}

	// �X�R�A���lUI
	scoreUI->Update(selectStageScore);
}

// �`�揈��
void SelectStageScene::Draw(Camera2D& _camera2D)
{
	// 2D�J�����Z�b�g
	Renderer::SetCameraViewProjection(_camera2D.GetViewMatrix(), _camera2D.GetProjectionMatrix());

	// �J�X�^���̐[�x�X�e���V���X�e�[�g
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState().Get(), 1);

	// SelectStageUI
	selectStageTexture->Draw();

	// �t���[��
	frame01->Draw();
	frame02->Draw();

	// BestScoreUI
	bestScoreTexture->Draw();
	
	// �X�R�A���lUI
	scoreUI->Draw();

	// �X�e�[�W��UI
	for (auto& stage : stageNames)
	{
		stage->MoveDraw();
	}

	// �X�e�[�W�摜
	for (auto& mapPhoto : stageMapPhoto)
	{
		mapPhoto->Draw();
	}
}

// IMGUI�̐ݒ�
void SelectStageScene::SetIMGUI()
{
	//=================================================
	//  FPS
	//=================================================
	ImGui::Begin("FPS");
	ImGui::Text("FrameRate (%.1f FPS)", FPS::Get()->GetFPS());
	ImGui::End();

	XMFLOAT3 fNum3; // Float3�^�ϐ�

	//=================================================
	//  UI
	//=================================================
	ImGui::Begin("UIParameter");

	// �X�R�A���lUI
	fNum3 = scoreUI->GetParentPosition();
	if (ImGui::SliderFloat2("ScoreUIPosition", &fNum3.x, -3.0f, 3.0f))
	{
		scoreUI->SetParentPosition(fNum3);
	}

	fNum3 = scoreUI->GetParentScale();
	if (ImGui::SliderFloat2("ScoreUIScale", &fNum3.x, 0.1f, 3.0f))
	{
		scoreUI->SetParentScale(fNum3);
	}

	// SelectStageUI
	fNum3 = selectStageTexture->GetPosition();
	if (ImGui::SliderFloat2("SelectStageTexturePosition", &fNum3.x, -Object2D::MaxTexPos, Object2D::MaxTexPos))
	{
		selectStageTexture->SetPosition(fNum3);
		selectStageTexture->ReCalculateWorldMatrix();
	}

	fNum3 = selectStageTexture->GetScale();
	if (ImGui::SliderFloat2("SelectStageTextureScale", &fNum3.x, 0.1f, Object2D::MaxTexScale))
	{
		selectStageTexture->SetScale(fNum3);
		selectStageTexture->ReCalculateWorldMatrix();
	}

	// BestScoreUI
	fNum3 = bestScoreTexture->GetPosition();
	if (ImGui::SliderFloat2("BestScoreTexturePosition", &fNum3.x, -Object2D::MaxTexPos, Object2D::MaxTexPos))
	{
		bestScoreTexture->SetPosition(fNum3);
		bestScoreTexture->ReCalculateWorldMatrix();
	}

	fNum3 = bestScoreTexture->GetScale();
	if (ImGui::SliderFloat2("BestScoreTextureScale", &fNum3.x, 0.1f, Object2D::MaxTexScale))
	{
		bestScoreTexture->SetScale(fNum3);
		bestScoreTexture->ReCalculateWorldMatrix();
	}

	// �t���[��1
	fNum3 = frame01->GetPosition();
	if (ImGui::SliderFloat2("Frame1Position", &fNum3.x, -Object2D::MaxTexPos, Object2D::MaxTexPos))
	{
		frame01->SetPosition(fNum3);
		frame01->ReCalculateWorldMatrix();
	}

	fNum3 = frame01->GetScale();
	if (ImGui::SliderFloat2("Frame1Scale", &fNum3.x, 0.1f, Object2D::MaxTexScale))
	{
		frame01->SetScale(fNum3);
		frame01->ReCalculateWorldMatrix();
	}

	// �t���[��2
	fNum3 = frame02->GetPosition();
	if (ImGui::SliderFloat2("Frame2Position", &fNum3.x, -Object2D::MaxTexPos, Object2D::MaxTexPos))
	{
		frame02->SetPosition(fNum3);
		frame02->ReCalculateWorldMatrix();
	}

	fNum3 = frame02->GetScale();
	if (ImGui::SliderFloat2("Frame2Scale", &fNum3.x, 0.1f, Object2D::MaxTexScale))
	{
		frame02->SetScale(fNum3);
		frame02->ReCalculateWorldMatrix();
	}

	// �X�e�[�W�摜
	fNum3 = stageMapPhoto[0]->GetPosition();
	if (ImGui::SliderFloat2("StageMapPhotoPosition", &fNum3.x, -Object2D::MaxTexPos, Object2D::MaxTexPos))
	{
		for (auto& tex : stageMapPhoto)
		{
			tex->SetPosition(fNum3);
			tex->ReCalculateWorldMatrix();
		}
	}

	fNum3 = stageMapPhoto[0]->GetScale();
	if (ImGui::SliderFloat2("StageMapPhotoScale", &fNum3.x, 0.1f, Object2D::MaxTexScale))
	{
		for (auto& tex : stageMapPhoto)
		{
			tex->SetScale(fNum3);
			tex->ReCalculateWorldMatrix();
		}
	}

	// �X�e�[�W��UI
	ImGui::SliderFloat2("StageNameUpPosition", &nameUpPosition.x, -Object2D::MaxTexPos, Object2D::MaxTexPos);
	ImGui::SliderFloat2("StageNameMiddlePosition", &nameMiddlePosition.x, -Object2D::MaxTexPos, Object2D::MaxTexPos);
	ImGui::SliderFloat2("StageNameDownPosition", &nameDownPosition.x, -Object2D::MaxTexPos, Object2D::MaxTexPos);
	ImGui::SliderFloat2("StageNameMiddleScale", &nameMiddleScale.x, 0.1f, Object2D::MaxTexScale);
	ImGui::SliderFloat2("StageNameUpDownScale", &nameUpDownScale.x, 0.1f, Object2D::MaxTexScale);

	ImGui::End();
}

// IMGUI�Őݒ肵���p�����[�^��CSV�ɕۑ�����
void SelectStageScene::SaveParameters()
{
	// �p�����[�^���Ɛ��l�Ƃ��̌^map�ł܂Ƃ߂�
	std::map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	//=================================================
	//  UI
	//=================================================
	params["ScoreUIPosition"] = { scoreUI->GetParentPosition(),CSVManager::TypeName::FLOAT3 };
	params["ScoreUIScale"] = { scoreUI->GetParentScale(),CSVManager::TypeName::FLOAT3 };
	params["UISelectStageTexturePosition"] = { selectStageTexture->GetPosition(),CSVManager::TypeName::FLOAT3 };
	params["UISelectStageTextureScale"] = { selectStageTexture->GetScale(),CSVManager::TypeName::FLOAT3 };
	params["UIBestScoreTexturePosition"] = { bestScoreTexture->GetPosition(),CSVManager::TypeName::FLOAT3 };
	params["UIBestScoreTextureScale"] = { bestScoreTexture->GetScale(),CSVManager::TypeName::FLOAT3 };
	params["UIFrame1Position"] = { frame01->GetPosition(),CSVManager::TypeName::FLOAT3 };
	params["UIFrame1Scale"] = { frame01->GetScale(),CSVManager::TypeName::FLOAT3 };
	params["UIFrame2Position"] = { frame02->GetPosition(),CSVManager::TypeName::FLOAT3 };
	params["UIFrame2Scale"] = { frame02->GetScale(),CSVManager::TypeName::FLOAT3 };
	params["UIStageMapPhotoPosition"] = { stageMapPhoto[0]->GetPosition(),CSVManager::TypeName::FLOAT3 };
	params["UIStageMapPhotoScale"] = { stageMapPhoto[0]->GetScale(),CSVManager::TypeName::FLOAT3 };
	params["UIStageNameUpPosition"] = { nameUpPosition,CSVManager::TypeName::FLOAT3 };
	params["UIStageNameMiddlePosition"] = { nameMiddlePosition,CSVManager::TypeName::FLOAT3 };
	params["UIStageNameDownPosition"] = { nameDownPosition,CSVManager::TypeName::FLOAT3 };
	params["UIStageNameMiddleScale"] = { nameMiddleScale,CSVManager::TypeName::FLOAT3 };
	params["UIStageNameUpDownScale"] = { nameUpDownScale,CSVManager::TypeName::FLOAT3 };

	// CSV�ۑ�
	CSVManager::SaveCSVPrameters(params, "Assets/SceneParameterList/StageSelectSceneParameters.csv");
}

// CSV����ǂݍ��񂾃p�����[�^�𔽉f������
void SelectStageScene::LoadParameters()
{
	// �p�����[�^���Ɛ��l�Ƃ��̌^map�ł܂Ƃ߂�
	std::unordered_map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	// �t�@�C���������݂��Ȃ��ꍇ�̓f�t�H���g�̒l�ŕۑ�����
	if (!CSVManager::LoadCSVPrameters(params, "Assets/SceneParameterList/StageSelectSceneParameters.csv"))
	{
		SaveParameters();
		return;
	}

	// map�̃p�����[�^�����Ԃɓǂݍ���
	for (const auto& entry : params)
	{
		//=================================================
		//  UI
		//=================================================
		if (entry.first == "ScoreUIPosition")
		{
			scoreUI->SetParentPosition(std::get<XMFLOAT3>(entry.second.first));

		}
		else if (entry.first == "ScoreUIScale")
		{
			scoreUI->SetParentScale(std::get<XMFLOAT3>(entry.second.first));
		}
		else if (entry.first == "UISelectStageTexturePosition")
		{
			selectStageTexture->SetPosition(std::get<XMFLOAT3>(entry.second.first));
			selectStageTexture->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UISelectStageTextureScale")
		{
			selectStageTexture->SetScale(std::get<XMFLOAT3>(entry.second.first));
			selectStageTexture->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UIBestScoreTexturePosition")
		{
			bestScoreTexture->SetPosition(std::get<XMFLOAT3>(entry.second.first));
			bestScoreTexture->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UIBestScoreTextureScale")
		{
			bestScoreTexture->SetScale(std::get<XMFLOAT3>(entry.second.first));
			bestScoreTexture->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UIFrame1Position")
		{
			frame01->SetPosition(std::get<XMFLOAT3>(entry.second.first));
			frame01->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UIFrame1Scale")
		{
			frame01->SetScale(std::get<XMFLOAT3>(entry.second.first));
			frame01->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UIFrame2Position")
		{
			frame02->SetPosition(std::get<XMFLOAT3>(entry.second.first));
			frame02->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UIFrame2Scale")
		{
			frame02->SetScale(std::get<XMFLOAT3>(entry.second.first));
			frame02->ReCalculateWorldMatrix();
		}
		else if (entry.first == "UIStageMapPhotoPosition")
		{
			for (auto& tex : stageMapPhoto)
			{
				tex->SetPosition(std::get<XMFLOAT3>(entry.second.first));
				tex->ReCalculateWorldMatrix();
			}
		}
		else if (entry.first == "UIStageMapPhotoScale")
		{
			for (auto& tex : stageMapPhoto)
			{
				tex->SetScale(std::get<XMFLOAT3>(entry.second.first));
				tex->ReCalculateWorldMatrix();
			}
		}
		else if (entry.first == "UIStageNameUpPosition")
		{
			nameUpPosition = std::get<XMFLOAT3>(entry.second.first);
		}
		else if (entry.first == "UIStageNameMiddlePosition")
		{
			nameMiddlePosition = std::get<XMFLOAT3>(entry.second.first);
		}
		else if (entry.first == "UIStageNameDownPosition")
		{
			nameDownPosition = std::get<XMFLOAT3>(entry.second.first);
		}
		else if (entry.first == "UIStageNameMiddleScale")
		{
			nameMiddleScale = std::get<XMFLOAT3>(entry.second.first);
		}
		else if (entry.first == "UIStageNameUpDownScale")
		{
			nameUpDownScale = std::get<XMFLOAT3>(entry.second.first);
		}
		// �p�����[�^�����o�^����Ă��Ȃ��ꍇ
		else
		{
			std::string message = entry.first + " :�l�ǂݍ��݃G���[";
			MessageBoxA(NULL, message.c_str(), "ErrorMesseage", MB_OK | MB_ICONERROR);
		}
	}
}