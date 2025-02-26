#include "UVAnimation2D.h"
#include "DebugConsole.h"

// �R���X�g���N�^
UVAnimation2D::UVAnimation2D(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:Object2D(_pos, _rot, _scale, _color)
{
}

// �X�V����
void UVAnimation2D::Update(float _deltaTime)
{
	if (!isAnimation) return; // �A�j���[�V�������Ȃ����return

	// �͈͂̃`�F�b�N
	if ((int)animCounter > animTable.size())
	{
		animCounter = 0;
	}
	// �A�j���[�V����ID��animCounter�Ɋ�Â��Đݒ�
	animID = animTable[(int)animCounter];

	// ����������2�ȏ�
	if (divSide > 1)
	{
		// ������UV�l���v�Z
		uv.x = animID % divSide * (1.0f / (float)divSide);
	}
	else
	{
		uv.x = 1.0f;
	}

	// �c��������2�ȏ�
	if (divVertical > 1)
	{
		// �c����UV�l���v�Z
		uv.y = animID / divSide * (1.0f / (float)divVertical);
	}
	else
	{
		uv.y = 1.0f;
	}

	animCounter += animSpeed * _deltaTime;   // �A�j���[�V�����J�E���^�[��i�߂�

	// �J�E���^�[���e�[�u���T�C�Y�𒴂�����ŏ��ɖ߂�
	if ((int)animCounter >= animTable.size())
	{
		animCounter = 0.0f;
		// ���[�v���Ȃ��Ȃ�A�j���[�V�����I��
		if (!isLoopAnimation)
		{
			isAnimation = false;
		}
	}
}

// �A�j���[�V�����̃R�}����ݒ�
void UVAnimation2D::SetAnimationTable(int tableNum)
{
	animTable.resize(tableNum);

	for (int i = 0; i < animTable.size(); i++)
	{
		animTable[i] = i;
	}
}

// �A�j���[�V�����Đ�
void UVAnimation2D::Play()
{
	isAnimation = true;
	animCounter = 0;
}

// �A�j���[�V������~
void UVAnimation2D::Stop()
{
	isAnimation = false;
}