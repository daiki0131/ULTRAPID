#include "SpeedUI.h"
#include "Geometry.h"
#include "DebugConsole.h"

using namespace DirectX;

SpeedUI::SpeedUI()
{
	std::vector<XMFLOAT3> positions =
	{
		XMFLOAT3(0.275f, -1.08f, 0.0f), // ����UI1
		XMFLOAT3(0.6f, -1.08f, 0.0f),   // ����UI2
		XMFLOAT3(0.925f, -1.08f, 0.0f), // ����UI3
		XMFLOAT3(1.25f, -1.08f, 0.0f),  // ����UI4
		//XMFLOAT3(1.55f, -1.17f, 0.0f),  // ����UI5
		//XMFLOAT3(1.73f, -1.17f, 0.0f),  // ����UI6
		//XMFLOAT3(1.44f, -1.28f, 0.0f),  // �h�b�g
		
	};

	std::vector<XMFLOAT3> scales =
	{
		XMFLOAT3(0.45f, 0.675f, 1.0f),    // ����UI1
		XMFLOAT3(0.45f, 0.675f, 1.0f),    // ����UI2
		XMFLOAT3(0.45f, 0.675f, 1.0f),    // ����UI3
		XMFLOAT3(0.45f, 0.675f, 1.0f),    // ����UI4
		//XMFLOAT3(0.2625f, 0.39375f, 1.0f),// ����UI5
		//XMFLOAT3(0.2625f, 0.39375f, 1.0f),// ����UI6
		//XMFLOAT3(0.17f, 0.21f, 1.0f),     // �h�b�g
		
	};

	speedNums.resize(4);

	// �I�u�W�F�N�g�����E�V�F�[�_�[�Z�b�g
	for (int i = 0; i < speedNums.size(); i++)
	{
		speedNums[i] = std::make_unique<Object2D>(
			positions[i],
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			scales[i],
			XMFLOAT4(1, 1, 1, 1));

		speedNums[i]->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	}

	// ���f���Z�b�g
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/number.png");
	// ����UI
	for (int i = 0; i < speedNums.size(); i++)
	{
		speedNums[i]->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
		speedNums[i]->SetModel(model);
	}
	//// �h�b�g
	//model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1.0f, 1.0f });
	//model.texture = Renderer::LoadTexture(L"Assets/Texture/dot.png");
	//speedNums[6]->SetModel(model);
	

	// �eUI�̐����^�C�v���Z�b�g
	speedPlace[0] = PLACE_1000;
	speedPlace[1] = PLACE_100;       // 100�̈�
	speedPlace[2] = PLACE_10;        // 10�̈�
	speedPlace[3] = PLACE_1;         // 1�̈�
	//speedPlace[4] = MILLI_PLACE_10;  // ������1��
	//speedPlace[5] = MILLI_PLACE_1;   // ������2��
	//speedPlace[6] = OTHER;           // �h�b�g
	

	// ���[���h�ϊ��s��Z�b�g
	SetWorldMatrix(speedNums);
}

// �X�V����
void SpeedUI::Update(const float _speed)
{
	float speedConversionFactor = MaxSpeedScaledValue / maxSpeed;
	float speed = _speed * speedConversionFactor;

	if (speed > MaxSpeedScaledValue) speed = MaxSpeedScaledValue;

	const unsigned int num = (int)speed; // ���������؂���
	const unsigned int milliNum = (int)((speed - (int)speed) * 100); // ���������؂���

	// �؂蕪���������Ɋ�Â���UV�l�����肷��
	for (int i = 0; i < speedNums.size(); i++)
	{
		// �ŏ��ɔ�������Ԃɂ���
		// �g�p��ԂɂȂ�Όォ�烿�l��1.0�ɂ���
		speedNums[i]->SetColorAlpha(NumberTexture2D::UiLowAlpha);
		auto it = speedPlace.find(i); // �����̃^�C�v���擾

		if (it == speedPlace.end())
		{
			DebugConsole::DebugLog("���xUI�̃^�C�v���ݒ肳��Ă܂���FSpeedUI::Update���Ŕ���", DebugConsole::WARNING_LOG);
			speedPlace[i] = PLACE_1;
		}

		switch (it->second)
		{
		case SpeedUI::PLACE_1:   // 1�̈�
			SetUV(num % 10);
			speedNums[i]->SetColorAlpha(1.0f);
			break;
		case SpeedUI::PLACE_10:  //10�̈�
			SetUV((num / 10) % 10);
			if (num >= 10)
			{
				speedNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case SpeedUI::PLACE_100: // 100�̈�
			SetUV((num / 100) % 10);
			if (num >= 100)
			{
				speedNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case SpeedUI::PLACE_1000: // 1000�̈�
			SetUV((num / 1000) % 10);
			if (num >= 1000)
			{
				speedNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case SpeedUI::MILLI_PLACE_1:  // ������1��
			SetUV(milliNum % 10);
			speedNums[i]->SetColorAlpha(1.0f);
			break;
		case SpeedUI::MILLI_PLACE_10: // ������2��
			SetUV(milliNum / 10);
			speedNums[i]->SetColorAlpha(1.0f);
			break;
		case SpeedUI::OTHER:          // �h�b�g�Ekm/h
			SetUV(10);
			speedNums[i]->SetColorAlpha(1.0f);
			break;
		default:
			break;
		}
		// UV�Z�b�g
		speedNums[i]->SetUV(uv);
	}
}

// �`�揈��
void SpeedUI::Draw()
{
	for (auto& ui : speedNums)
	{
		ui->Draw();
	}
}

// �e���W�̍X�V
void SpeedUI::SetParentPosition(DirectX::XMFLOAT3 _pos)
{
	parentPosition = _pos;
	SetWorldMatrix(speedNums);
}

// �e�X�P�[���̍X�V
void SpeedUI::SetParentScale(DirectX::XMFLOAT3 _scale)
{
	parentScale = _scale;
	SetWorldMatrix(speedNums);
}

// ���x�����ɉ������F�̍X�V
void SpeedUI::UpdateSpeedRatioColor(float _ratio)
{
	XMFLOAT3 color;

	// Phase1
	if (_ratio < changeColorRatioPhase2)
	{
		color.x = speedRatioColor1.x;
		color.y = speedRatioColor1.y;
		color.z = speedRatioColor1.z;
	}
	// Phase2
	else if (_ratio < changeColorRatioPhase3)
	{
		color.x = speedRatioColor2.x;
		color.y = speedRatioColor2.y;
		color.z = speedRatioColor2.z;
	}
	// Phase3
	else if (_ratio < changeColorRatioPhase4)
	{
		color.x = speedRatioColor3.x;
		color.y = speedRatioColor3.y;
		color.z = speedRatioColor3.z;
	}
	// Phase4
	else
	{
		color.x = speedRatioColor4.x;
		color.y = speedRatioColor4.y;
		color.z = speedRatioColor4.z;
	}

	// �J���[�Z�b�g
	for (auto& ui : speedNums)
	{
		ui->SetRGBColor(color);
	}
}

// �e��������ݒ�
void SpeedUI::SetSpeedRatioColorPhase(float phase1, float phase2, float phase3, float phase4)
{
	changeColorRatioPhase1 = phase1;
	changeColorRatioPhase2 = phase2;
	changeColorRatioPhase3 = phase3;
	changeColorRatioPhase4 = phase4;
}

// �e�����̐F��ݒ�
void SpeedUI::SetSpeedRatioColor(
	DirectX::XMFLOAT3 colorPhase1,
	DirectX::XMFLOAT3 colorPhase2,
	DirectX::XMFLOAT3 colorPhase3,
	DirectX::XMFLOAT3 colorPhase4)
{
	speedRatioColor1 = colorPhase1;
	speedRatioColor2 = colorPhase2;
	speedRatioColor3 = colorPhase3;
	speedRatioColor4 = colorPhase4;
}

// �A�N�e�B�u��Ԃ��ꊇ�؂�ւ�
void SpeedUI::SetActive(bool _active)
{
	for (auto& ui : speedNums)
	{
		ui->SetIsActive(_active);
	}
}