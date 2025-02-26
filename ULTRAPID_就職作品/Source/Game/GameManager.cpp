#include "GameManager.h"
#include "CSVManager.h"

// �����_���̃V�[�h�l������
std::random_device GameManager::rd;
std::mt19937 GameManager::mt(GameManager::rd());
// Static�ϐ�
GameManager::SELECT_STAGE GameManager::selectStage = SELECT_STAGE::STAGE_1;
int GameManager::stageScore = 0;
std::map<GameManager::SELECT_STAGE, float> GameManager::stageTimeData;

// ����������
void GameManager::Init()
{
	// �X�R�A�f�[�^�̏���������
	for (int i = 0; i < STAGE_MAX; i++)
	{
		scoreData[static_cast<SELECT_STAGE>(i)] = 0;
	}
	
	LoadScoreData();     // CSV����X�R�A�f�[�^��ǂݍ���
	LoadStageTimeData(); // CSV����X�e�[�W�̃^�C������ǂݍ���
}

// �������
void GameManager::UnInit()
{
	SaveScoreData(); // �X�R�A��CSV�ɕۑ�
}

// CSV�ɃX�R�A��ۑ�����֐�
void GameManager::SaveScoreData()
{
	std::map<std::string, int> saveData; // �ۑ�����f�[�^
	std::string text = "Stage";          // ������̐擪��Stage���w��
	int stageNumber = 1;                 // Stage�̌�̃X�e�[�W�i���o�[

	// �X�R�A�f�[�^���R�s�[
	for (const auto& entry : scoreData)
	{
		std::string result = text + std::to_string(stageNumber);
		saveData[result] = entry.second;
		stageNumber++;
	}
	// �ۑ�
	CSVManager::SaveCSVScore(saveData, "Assets/StageDataList/SaveData.csv");
}

// CSV����X�R�A�f�[�^��ǂݎ��֐�
void GameManager::LoadScoreData()
{
	std::map<std::string, std::variant<int, float>> loadData; // �ǂݍ��݃f�[�^

	// CSV����f�[�^�����[�h�i�t�@�C�����Ȃ��ꍇ�����l��ۑ��j
	if (!CSVManager::LoadCSVStageData(loadData, "Assets/StageDataList/SaveData.csv",CSVManager::INT))
	{
		SaveScoreData();
		return;
	}

	// �ǂݍ��݃f�[�^��scoreData�ɃR�s�[
	int indexCount = 0;
	for (const auto& entry : loadData)
	{
		scoreData[static_cast<SELECT_STAGE>(indexCount)] = std::get<int>(entry.second);
		indexCount++;
	}
}

// CSV����X�e�[�W�̃^�C���f�[�^��ǂݍ���
void GameManager::LoadStageTimeData()
{
	std::map<std::string, std::variant<int, float>> loadData; // �ǂݍ��݃f�[�^
	// �ǂݍ���
	CSVManager::LoadCSVStageData(loadData, "Assets/StageDataList/StageTimeData.csv", CSVManager::FLOAT);

	// �ǂݍ��񂾃f�[�^��stageTimeData�ɃR�s�[
	int indexCount = 0;
	for (const auto& entry : loadData)
	{
		stageTimeData[static_cast<SELECT_STAGE>(indexCount)] = std::get<float>(entry.second);
		indexCount++;
	}
}

// �X�R�A�����Z����֐��iGameScene�Ŏg�p�j
void GameManager::AddStageScore(int _score)
{
	stageScore += _score; // �X�R�A���Z

	// �ő�l�𒴂��Ȃ��悤�ɂ���
	stageScore = std::clamp(stageScore, 0, MaxScore);
}

// �ő�X�R�A���X�V���A���̌��ʂ�Ԃ��֐�
bool GameManager::IsHighScoreUpdated()
{
	// �����̃X�e�[�W�X�R�A��葽���ꍇ�㏑��
	if (scoreData[selectStage] < stageScore)
	{
		scoreData[selectStage] = stageScore; // �X�R�A�㏑��

		return true; // �ő�X�R�A���X�V���ꂽ�̂�true
	}

	return false; // �X�V����ĂȂ��̂�false
}