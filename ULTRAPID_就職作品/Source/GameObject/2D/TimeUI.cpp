#include "TimeUI.h"
#include "Geometry.h"
#include "DebugConsole.h"

using namespace DirectX;

TimeUI::TimeUI()
{
	std::vector<XMFLOAT3> positions =
	{
		XMFLOAT3(0.55f, -0.9f, -0.21f),    // �~���b�i��ԉE�̒l�j
		XMFLOAT3(0.79f, -0.9f, -0.21f),    // �~���b�i�E����2�Ԗڂ̒l�j
		XMFLOAT3(1.10f, -0.9f, -0.21f),    // �b�i�E����3�Ԗڂ̒l�j
		XMFLOAT3(1.34f, -0.9f, -0.21f),    // �b�i�E����4�Ԗڂ̒l�j
		XMFLOAT3(1.63f, -0.955f, -0.21f),  // ���i�E����5�Ԗڂ̒l�j
		XMFLOAT3(1.785f, -0.955f, -0.21f), // ���i�E����6�Ԗڂ̒l�j
		XMFLOAT3(0.96f, -1.035f, -0.21f),  // �h�b�g1�i���ƕb�̊ԁj
		XMFLOAT3(1.5f, -1.038f, -0.21f),   // �h�b�g2�i�b�ƃ~���b�̊ԁj
	};

	std::vector<XMFLOAT3> scales =
	{
		XMFLOAT3(0.3f, 0.45f, 1.0f),      // �~���b�i��ԉE�̒l�j
		XMFLOAT3(0.3f, 0.45f, 1.0f),      // �~���b�i�E����2�Ԗڂ̒l�j
		XMFLOAT3(0.3f, 0.45f, 1.0f),      // �b�i�E����3�Ԗڂ̒l�j
		XMFLOAT3(0.3f, 0.45f, 1.0f),      // �b�i�E����4�Ԗڂ̒l�j
		XMFLOAT3(0.2f, 0.3f, 1.0f),       // ���i�E����5�Ԗڂ̒l�j
		XMFLOAT3(0.2f, 0.3f, 1.0f),       // ���i�E����6�Ԗڂ̒l�j
		XMFLOAT3(0.17f, 0.21f, 1.0f),     // �h�b�g1�i���ƕb�̊ԁj
		XMFLOAT3(0.14f, 0.173f, 1.0f),    // �h�b�g2�i�b�ƃ~���b�̊ԁj
	};

	timeNums.resize(8);

	// �I�u�W�F�N�g�����E�V�F�[�_�[�Z�b�g
	for (int i = 0; i < timeNums.size(); i++)
	{
		timeNums[i] = std::make_unique<Object2D>(
			positions[i],
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			scales[i],
			XMFLOAT4(1, 1, 1, 1));

		timeNums[i]->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	}

	// ���f���Z�b�g
	// ����UI
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/number.png");
	for (int i = 0; i < timeNums.size() -2; i++)
	{
		timeNums[i]->SetModel(model);
	}
	// �h�b�gUI
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 1.0f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/dot.png");
	timeNums[6]->SetModel(model);
	timeNums[7]->SetModel(model);

	// �eUI�̃^�C�v��ݒ�
	timePlace[0] = MINUTE_DIGIT2;
	timePlace[1] = MINUTE_DIGIT1;
	timePlace[2] = SECOND_DIGIT2;
	timePlace[3] = SECOND_DIGIT1;
	timePlace[4] = MILLISECONND_DIGIT2;
	timePlace[5] = MILLISECONND_DIGIT1;
	timePlace[6] = OTHER;
	timePlace[7] = OTHER;

	// ���[���h�ϊ��s��Z�b�g
	SetWorldMatrix(timeNums);
}

void TimeUI::Update(const float _time)
{
	if (_time < 0.0f) return;

	// �w��̕b���ȉ��ɂȂ�ƐF��ύX����
	if (_time < ChangeColorSecond)
	{
		for (auto& ui : timeNums)
		{
			ui->SetRGBColor(XMFLOAT3(1, 0, 0)); // �Ԃɕς���
		}
	}

	// �������𔲂����
	const unsigned int minute = (int)_time / 60;
	// �b�����𔲂����
	const unsigned int second = (int)_time % 60;
	// �~���b�����𔲂����
	const unsigned int milliSecond = (int)((_time - (int)_time) * 100);

	for (int i = 0; i < timeNums.size(); i++)
	{
		// �ŏ��ɔ�������Ԃɂ���
		// �g�p��ԂɂȂ�Όォ�烿�l��1.0�ɂ���
		timeNums[i]->SetColorAlpha(NumberTexture2D::UiLowAlpha);
		auto it = timePlace.find(i); // �����̃^�C�v���擾

		if (it == timePlace.end())
		{
			DebugConsole::DebugLog("�^�C��UI�̃^�C�v���ݒ肳��Ă܂���FTimeUI::Update���Ŕ���", DebugConsole::WARNING_LOG);
			timePlace[i] = SECOND_DIGIT1;
			continue;
		}

		switch (it->second)
		{
		case TimeUI::MINUTE_DIGIT1:       // ��1����
			SetUV(minute % 10);
			if (minute > 0)
			{
				timeNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case TimeUI::MINUTE_DIGIT2:       // ��2����
			SetUV(minute / 10);
			if (minute >= 10)
			{
				timeNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case TimeUI::SECOND_DIGIT1:       // �b1����
			SetUV(second % 10);
			timeNums[i]->SetColorAlpha(1.0f);
			break;
		case TimeUI::SECOND_DIGIT2:       // �b2����
			SetUV(second / 10);
			if (second >= 10 || minute > 0)
			{
				timeNums[i]->SetColorAlpha(1.0f);
			}
			break;
		case TimeUI::MILLISECONND_DIGIT1: // �~���b1����
			SetUV(milliSecond % 10);
			timeNums[i]->SetColorAlpha(1.0f);
			break;
		case TimeUI::MILLISECONND_DIGIT2: // �~���b1����
			SetUV(milliSecond / 10);
			timeNums[i]->SetColorAlpha(1.0f);
			break;
		case TimeUI::OTHER:               // �h�b�g
			SetUV(10);
			timeNums[i]->SetColorAlpha(1.0f);
			break;
		default:
			break;
		}
		// UV�Z�b�g
		timeNums[i]->SetUV(uv);
	}
}

// �`�揈��
void TimeUI::Draw()
{
	for (auto& ui : timeNums)
	{
		ui->Draw();
	}
}

// �e���W�̍X�V
void TimeUI::SetParentPosition(DirectX::XMFLOAT3 _pos)
{
	parentPosition = _pos;
	SetWorldMatrix(timeNums);
}

// �e�X�P�[���̍X�V
void TimeUI::SetParentScale(DirectX::XMFLOAT3 _scale)
{
	parentScale = _scale;
	SetWorldMatrix(timeNums);
}

// �A�N�e�B�u��Ԃ��ꊇ�؂�ւ�
void TimeUI::SetActive(bool _active)
{
	for (auto& ui : timeNums)
	{
		ui->SetIsActive(_active);
	}
}