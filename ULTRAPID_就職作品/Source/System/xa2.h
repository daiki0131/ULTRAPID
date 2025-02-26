//=============================================================================
//
// �T�E���h���� [xa2.h]
//
//=============================================================================
#ifndef _XAUDIO2_H_
#define _XAUDIO2_H_

static constexpr float ItemSEVol = 0.4f;
static constexpr float DecisionSEVol = 1.0f;
static constexpr float CancelSEVol = 1.0f;
static constexpr float SelectSEVol = 0.3f;
static constexpr float CountSEVol = 0.7f;
static constexpr float DashSEVol = 1.0f;
static constexpr float JumpSEVol = 0.6f;
static constexpr float ScoreSEVol = 1.0f;
static constexpr float StartSEVol = 0.4f;
static constexpr float TimeUpSEVol = 0.6f;
static constexpr float ChargeSEVol = 0.4f;
static constexpr float ChargeOKSEVol = 0.6f;
static constexpr float AddScoreSEVol = 0.3f;
static constexpr float TitleBGMVol = 0.6f;
static constexpr float SelectStageBGMVol = 0.55f;
static constexpr float StageBGMVol = 0.35f;
static constexpr float ResultBGMVol = 0.55f;
static constexpr float WallSEVol = 0.4f;
static constexpr float NoBreakSEVol = 0.5f;
static constexpr float MoveSEVol = 0.1f;

static constexpr float ChargeSESpeed = 0.8f;
static constexpr float WallSESpeed = 1.2f;
static constexpr float NoBreakSESpeed = 2.0f;
static constexpr float MoveSESpeed = 0.8f;
static constexpr float StartCountSESpeed = 1.5f;

static constexpr bool isXAPlay = true;
static constexpr float volRatio = 0.2f;

#include <xaudio2.h>

// �T�E���h�t�@�C��
typedef enum
{
	BALLOON_SE = 0,
	DECISION_SE,
	CANCEL_SE,
	SELECT_SE,
	COUNT_SE,
	DASH_SE,
	JUMP_SE,
	SCORE_SE,
	START_SE,
	TIME_UP_SE,
	CHARGE_SE,
	CHARGE_OK_SE,
	ADD_SCORE_SE,
	TITLE_BGM,
	SELECT_STAGE_BGM,
	STAGE_BGM,
	RESULT_BGM,
	WALL_SE,
	NO_BREAK_SE,
	MOVE_SE,
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

// �Q�[�����[�v�J�n�O�ɌĂяo���T�E���h�̏���������
HRESULT XA_Initialize(void);

// �Q�[�����[�v�I����ɌĂяo���T�E���h�̉������
void XA_Release(void);

// �����Ŏw�肵���T�E���h���Đ�����
void XA_Play(SOUND_LABEL label, float vol);

// �����Ŏw�肵���T�E���h���~����
void XA_Stop(SOUND_LABEL label);

// BGM�����ݍĐ�����Ă邩�̔���
bool XA_IsPlaying(SOUND_LABEL label);

// �����Ŏw�肵���T�E���h�̍Đ����ĊJ����
void XA_Resume(SOUND_LABEL label);

void XA_SetSpeed(SOUND_LABEL label, float speed);
void XA_SetVolume(SOUND_LABEL label, float vol);

#endif
