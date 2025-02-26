#pragma once
#include <map>
#include <random>

#define D2R(deg) ((deg)*3.14159265f/180.0f) // �x���烉�W�A���ϊ��̃}�N����`

//===========================================//
// �Q�[�����f�[�^���Ǘ�����N���X
//===========================================//
class GameManager
{
public:
	// �I���X�e�[�W
	enum SELECT_STAGE
	{
		STAGE_1,
		STAGE_2,
		STAGE_3,
		STAGE_MAX,
	};
	
	static std::random_device rd;                         // ���������p
	static std::mt19937 mt;                               // ���������p
	static constexpr float GravityScale = -19.6f;		  // �d��
	static constexpr float RadToDeg = 180.0f / 3.1415926f;// ���W�A������x�ɕϊ�   
	static constexpr float FullRotationDegress = 360.0f;  // 1��] 
	static constexpr int MaxScore = 99999;				  // �ő�X�R�A
	static constexpr int Max3DobjectCount = 10000;		  // 1�̃^�C�v�̃I�u�W�F�N�g�ő吔
private:
	static SELECT_STAGE selectStage;					  // ���݂̑I���X�e�[�W
	std::map<SELECT_STAGE, int> scoreData;                // �e�X�e�[�W�̃X�R�A�f�[�^
	static std::map<SELECT_STAGE, float> stageTimeData;   // �e�X�e�[�W�̐������ԃf�[�^
	static int stageScore;                                // ���݊l���̃X�R�A

public:
	void Init();
	void UnInit();
	// �X�e�[�W�ɓ���O�̏���������
	static void InitStageScore() { stageScore = 0; };
	// Set�֐�
	void SetSelectStage(SELECT_STAGE _stage) { selectStage = _stage; }
	// Get�֐�
	static const SELECT_STAGE GetSelectStage() { return selectStage; }
	int GetScore(SELECT_STAGE _stage) { return scoreData[_stage]; }
	int GetStageScore() { return stageScore; }
	static const float GetStageTimeData() { return stageTimeData[selectStage]; }
	// CSV�ۑ��i�X�R�A�j
	void SaveScoreData();
	// CSV�ǂݍ��݁i�X�R�A�j
	void LoadScoreData();
	// CSV�ǂݍ��݁i�������ԁj
	void LoadStageTimeData();
	// �X�R�A���Z
	void AddStageScore(int _score);
	// �ő�X�R�A�����Z���čX�V�����ꍇ�̌��ʂ�Ԃ�
	bool IsHighScoreUpdated();
};