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

// 初期化処理
void SelectStageScene::Init()
{
	//=================================================
	//  オブジェクト生成
	//=================================================

	// 読み込んだモデルを管理する配列
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

	// スコア数値UI
	scoreUI = std::make_unique<ScoreUI>();
	
	// ステージ名UI
	stageNames.resize(GameManager::STAGE_MAX);
	for (auto& stage : stageNames)
	{
		stage = std::make_unique<Object2D>(
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.5f, 0.35f, 1.0f),
			XMFLOAT4(1, 1, 1, 1));
	}

	// ステージの画像
	stageMapPhoto.resize(GameManager::STAGE_MAX);
	for (auto& mapPhoto : stageMapPhoto)
	{
		mapPhoto = std::make_unique<Object2D>(
			XMFLOAT3(1.04f, 0.4f, -0.19f),
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.8f, 1.01f, 1.0f),
			XMFLOAT4(1, 1, 1, 1));
	}

	// フレーム1
	frame01 = std::make_unique<Object2D>(
		XMFLOAT3(1.04f, -0.9f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.8f, 0.6f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	// フレーム2
	frame02 = std::make_unique<Object2D>(
		XMFLOAT3(1.04f, 0.4f, -0.2f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(2.7f, 1.52f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	//=================================================
	//  モデル・テクスチャ読み込み
	//=================================================

	// 画像ファイル
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
	// テクスチャ読み込み
	for (int i = 0; i < ARRAYSIZE(textureFileName); i++)
	{
		model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1,1 });
		model.texture = Renderer::LoadTexture(textureFileName[i]);
		modelManager->emplace(textureFileName[i], model);
	}

	//=================================================
	//  モデルセット
	//=================================================
	
	// SelectStageUI
	model = modelManager->at(L"Assets/Texture/SelectStage.png");
	selectStageTexture->SetModel(model);

	// BestScoreUI
	model = modelManager->at(L"Assets/Texture/BestScore.png");
	bestScoreTexture->SetModel(model);

	// ステージ名UI
	model = modelManager->at(L"Assets/Texture/Stage1.png");
	stageNames[0]->SetModel(model);
	model = modelManager->at(L"Assets/Texture/Stage2.png");
	stageNames[1]->SetModel(model);
	model = modelManager->at(L"Assets/Texture/Stage3.png");
	stageNames[2]->SetModel(model);

	// ステージの画像
	model = modelManager->at(L"Assets/Texture/StageMapPhoto01.png");
	stageMapPhoto[0]->SetModel(model);
	model = modelManager->at(L"Assets/Texture/StageMapPhoto02.png");
	stageMapPhoto[1]->SetModel(model);
	model = modelManager->at(L"Assets/Texture/StageMapPhoto03.png");
	stageMapPhoto[2]->SetModel(model);

	// フレーム
	model = modelManager->at(L"Assets/Texture/frame.png");
	frame01->SetModel(model);
	frame02->SetModel(model);

	//=================================================
	//  初期設定
	//=================================================

	// SelectStageUI
	selectStageTexture->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// BestScoreUI
	bestScoreTexture->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);

	// ステージ名UI
	for (auto& stage : stageNames)
	{
		stage->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	}

	// ステージの画像
	for (auto& mapPhoto : stageMapPhoto)
	{
		mapPhoto->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	}

	// フレーム
	frame01->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	frame02->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	
	// BGM再生
	XA_Play(SELECT_STAGE_BGM, SelectStageBGMVol * volRatio);
}

// 解放処理
void SelectStageScene::UnInit()
{
	// BGM停止
	XA_Stop(SELECT_STAGE_BGM);

#ifdef _DEBUG
	SaveParameters();
#endif
}

// 更新処理
void SelectStageScene::Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm)
{
	// EnterキーまたはゲームパッドAかつフェード中でない場合
	if ((Input::Get().GetKeyTrigger(DIK_RETURN) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_A)) && !sceneManager.GetIsSceneFade())
	{
		XA_Play(DECISION_SE, DecisionSEVol * volRatio);
		gm.SetSelectStage(static_cast<GameManager::SELECT_STAGE>(selectStageCount));

		// GameScene読み込み
		sceneManager.SetScene(std::make_unique<GameScene>());
	}

	// BackSpaceキーまたはゲームパッドBかつフェード中でない場合
	if ((Input::Get().GetKeyTrigger(DIK_BACKSPACE) || XInputGamePad::Get().GetButtonTrigger(XINPUT_GAMEPAD_B)) && !sceneManager.GetIsSceneFade())
	{
		XA_Play(CANCEL_SE, CancelSEVol * volRatio);

		// TitleScene読み込み
		sceneManager.SetScene(std::make_unique<TitleScene>());
	}

	// ゲームパッドのアナログスティックの情報を取得
	XInputGamePad::AnalogStick analogStick = { 0,0 };

	// ゲームパッドの左スティックのY軸のトリガー状態を取得
	if (XInputGamePad::Get().GetStickTriggerLY())
	{
		analogStick = XInputGamePad::Get().GetAnalogStick();
	}

	// ステージ2以上が選択状態のとき
	if (selectStageCount > 0)
	{
		// Wキーまたはゲームパッド左スティックY軸上かつフェード状態でないとき
		if ((Input::Get().GetKeyTrigger(DIK_W) || analogStick.LY > 0.0f) && !sceneManager.GetIsSceneFade())
		{
			XA_Play(SELECT_SE, SelectSEVol * volRatio);
			selectStageCount--;
		}
	}
	// ステージ最大数より低い場合
	if (selectStageCount < stageNames.size() - 1)
	{
		// Sキーまたはゲームパッド左スティックY軸下かつフェード状態でないとき
		if ((Input::Get().GetKeyTrigger(DIK_S) || analogStick.LY < 0.0f) && !sceneManager.GetIsSceneFade())
		{
			XA_Play(SELECT_SE, SelectSEVol * volRatio);
			selectStageCount++;
		}
	}

	// 選択ステージに応じて各UIの状態を切り替える
	for (int i = 0; i < stageNames.size(); i++)
	{
		// 現在の選択ステージ
		if (i == selectStageCount)
		{
			stageNames[i]->SetPosition(nameMiddlePosition); // 中央
			stageNames[i]->SetScale(nameMiddleScale);
			stageNames[i]->SetColorAlpha(1.0f);
			stageNames[i]->SetIsActive(true);

			// そのステージのスコア履歴を表示させる
			selectStageScore = gm.GetScore(static_cast<GameManager::SELECT_STAGE>(selectStageCount));
		}
		// 選択ステージの1つ上のステージ（例：選択ステージ2の場合ステージ1）
		else if (i == selectStageCount - 1)
		{
			stageNames[i]->SetPosition(nameUpPosition); // 上
			stageNames[i]->SetScale(nameUpDownScale);
			stageNames[i]->SetColorAlpha(Object2D::UiLowAlpha);
			stageNames[i]->SetIsActive(true);
		}
		// 選択ステージの1つ下のステージ（例：選択ステージ2の場合ステージ3）
		else if (i == selectStageCount + 1)
		{
			stageNames[i]->SetPosition(nameDownPosition); // 下
			stageNames[i]->SetScale(nameUpDownScale);
			stageNames[i]->SetColorAlpha(Object2D::UiLowAlpha);
			stageNames[i]->SetIsActive(true);
		}
		else
		{
			// 上記以外の場合描画しない
			stageNames[i]->SetIsActive(false);
		}
	}

	// ステージの画像の更新
	for (int i = 0; i < stageMapPhoto.size(); i++)
	{
		if (i == selectStageCount)
		{
			// 現在の選択ステージの画像のみ描画する
			stageMapPhoto[i]->SetIsActive(true);
		}
		else
		{
			stageMapPhoto[i]->SetIsActive(false);
		}
	}

	// スコア数値UI
	scoreUI->Update(selectStageScore);
}

// 描画処理
void SelectStageScene::Draw(Camera2D& _camera2D)
{
	// 2Dカメラセット
	Renderer::SetCameraViewProjection(_camera2D.GetViewMatrix(), _camera2D.GetProjectionMatrix());

	// カスタムの深度ステンシルステート
	Renderer::GetContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState().Get(), 1);

	// SelectStageUI
	selectStageTexture->Draw();

	// フレーム
	frame01->Draw();
	frame02->Draw();

	// BestScoreUI
	bestScoreTexture->Draw();
	
	// スコア数値UI
	scoreUI->Draw();

	// ステージ名UI
	for (auto& stage : stageNames)
	{
		stage->MoveDraw();
	}

	// ステージ画像
	for (auto& mapPhoto : stageMapPhoto)
	{
		mapPhoto->Draw();
	}
}

// IMGUIの設定
void SelectStageScene::SetIMGUI()
{
	//=================================================
	//  FPS
	//=================================================
	ImGui::Begin("FPS");
	ImGui::Text("FrameRate (%.1f FPS)", FPS::Get()->GetFPS());
	ImGui::End();

	XMFLOAT3 fNum3; // Float3型変数

	//=================================================
	//  UI
	//=================================================
	ImGui::Begin("UIParameter");

	// スコア数値UI
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

	// フレーム1
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

	// フレーム2
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

	// ステージ画像
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

	// ステージ名UI
	ImGui::SliderFloat2("StageNameUpPosition", &nameUpPosition.x, -Object2D::MaxTexPos, Object2D::MaxTexPos);
	ImGui::SliderFloat2("StageNameMiddlePosition", &nameMiddlePosition.x, -Object2D::MaxTexPos, Object2D::MaxTexPos);
	ImGui::SliderFloat2("StageNameDownPosition", &nameDownPosition.x, -Object2D::MaxTexPos, Object2D::MaxTexPos);
	ImGui::SliderFloat2("StageNameMiddleScale", &nameMiddleScale.x, 0.1f, Object2D::MaxTexScale);
	ImGui::SliderFloat2("StageNameUpDownScale", &nameUpDownScale.x, 0.1f, Object2D::MaxTexScale);

	ImGui::End();
}

// IMGUIで設定したパラメータをCSVに保存する
void SelectStageScene::SaveParameters()
{
	// パラメータ名と数値とその型mapでまとめる
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

	// CSV保存
	CSVManager::SaveCSVPrameters(params, "Assets/SceneParameterList/StageSelectSceneParameters.csv");
}

// CSVから読み込んだパラメータを反映させる
void SelectStageScene::LoadParameters()
{
	// パラメータ名と数値とその型mapでまとめる
	std::unordered_map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>> params;

	// ファイル名が存在しない場合はデフォルトの値で保存する
	if (!CSVManager::LoadCSVPrameters(params, "Assets/SceneParameterList/StageSelectSceneParameters.csv"))
	{
		SaveParameters();
		return;
	}

	// mapのパラメータを順番に読み込む
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
		// パラメータ名が登録されていない場合
		else
		{
			std::string message = entry.first + " :値読み込みエラー";
			MessageBoxA(NULL, message.c_str(), "ErrorMesseage", MB_OK | MB_ICONERROR);
		}
	}
}