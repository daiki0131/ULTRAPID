#include "ScoreUI.h"
#include "Geometry.h"
#include "DebugConsole.h"

using namespace DirectX;

ScoreUI::ScoreUI()
{
	std::vector<XMFLOAT3> positions =
	{
		XMFLOAT3(0.55f, -0.9f, -0.21f),  // 10000�̈�
		XMFLOAT3(0.79f, -0.9f, -0.21f),  // 1000�̈�
		XMFLOAT3(1.03f, -0.9f, -0.21f),  // 100�̈�
		XMFLOAT3(1.27f, -0.9f, -0.21f),  // 10�̈�
		XMFLOAT3(1.51f, -0.9f, -0.21f),  // 1�̈�
	};

	// ���f��
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/number.png");

	scores.resize(5);

	// �e����UI�̐���
	for (int i = 0; i < scores.size(); i++)
	{
		scores[i] = std::make_unique<Object2D>(
			positions[i],
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(0.3f, 0.45f, 1.0f),
			XMFLOAT4(1, 1, 1, 1));
		// �V�F�[�_�[�Z�b�g
		scores[i]->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
		// ���f���Z�b�g
		scores[i]->SetModel(model);
	}

	// �eUI�̌��̃^�C�v�ݒ�
	scorePlace[0] = PLACE_10000;
	scorePlace[1] = PLACE_1000;
	scorePlace[2] = PLACE_100;
	scorePlace[3] = PLACE_10;
	scorePlace[4] = PLACE_1;

	// ���[���h�ϊ��s��Z�b�g
	SetWorldMatrix(scores);
}

void ScoreUI::Update(const int _score)
{
	// �X�R�A�Ɋ�Â��Ċe���̐��������o����UV�l��ݒ�
	for (int i = 0; i < scores.size(); i++)
	{
		// �ŏ��ɔ�������Ԃɂ���
		// �g�p��ԂɂȂ�Όォ�烿�l��1.0�ɂ���
		scores[i]->SetColorAlpha(NumberTexture2D::UiLowAlpha);
		auto it = scorePlace.find(i); // �����̃^�C�v���擾

		if (it == scorePlace.end())
		{
			DebugConsole::DebugLog("�X�R�A�̃^�C�v���ݒ肳��Ă܂���FScoreUI::Update���Ŕ���", DebugConsole::WARNING_LOG);
			scorePlace[i] = PLACE_1;
		}
	
		switch (it->second)
		{
		case PLACE_1:       // 1�̈�
			SetUV(_score % 10);
			if (_score >= 0)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		case PLACE_10:      // 10�̈�
			SetUV((_score / 10) % 10);
			if (_score >= 10)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		case PLACE_100:     // 100�̈�
			SetUV((_score / 100) % 10);
			if (_score >= 100)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		case PLACE_1000:    // 1000�̈�
			SetUV((_score / 1000) % 10);
			if (_score >= 1000)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		case PLACE_10000:   // 10000�̈�
			SetUV((_score / 10000) % 10);
			if (_score >= 10000)
			{
				scores[i]->SetColorAlpha(1.0f);
			}
			break;
		default:
			break;
		}
		// UV�Z�b�g
		scores[i]->SetUV(uv);
	}
}

// �`�揈��
void ScoreUI::Draw()
{
	for (auto& ui : scores)
	{
		ui->Draw();
	}
}

// �e���W�̍X�V
void ScoreUI::SetParentPosition(DirectX::XMFLOAT3 _pos)
{
	parentPosition = _pos;
	SetWorldMatrix(scores);
}

// �e�X�P�[���̍X�V
void ScoreUI::SetParentScale(DirectX::XMFLOAT3 _scale)
{
	parentScale = _scale;
	SetWorldMatrix(scores);
}

// �A�N�e�B�u��Ԃ��ꊇ�؂�ւ�
void ScoreUI::SetActive(bool _active)
{
	for (auto& ui : scores)
	{
		ui->SetIsActive(_active);
	}
}