#include "GameManager.h"
#include "CSVManager.h"

// ランダムのシード値初期化
std::random_device GameManager::rd;
std::mt19937 GameManager::mt(GameManager::rd());
// Static変数
GameManager::SELECT_STAGE GameManager::selectStage = SELECT_STAGE::STAGE_1;
int GameManager::stageScore = 0;
std::map<GameManager::SELECT_STAGE, float> GameManager::stageTimeData;

// 初期化処理
void GameManager::Init()
{
	// スコアデータの初期化処理
	for (int i = 0; i < STAGE_MAX; i++)
	{
		scoreData[static_cast<SELECT_STAGE>(i)] = 0;
	}
	
	LoadScoreData();     // CSVからスコアデータを読み込み
	LoadStageTimeData(); // CSVからステージのタイム情報を読み込み
}

// 解放処理
void GameManager::UnInit()
{
	SaveScoreData(); // スコアをCSVに保存
}

// CSVにスコアを保存する関数
void GameManager::SaveScoreData()
{
	std::map<std::string, int> saveData; // 保存するデータ
	std::string text = "Stage";          // 文字列の先頭にStageを指定
	int stageNumber = 1;                 // Stageの後のステージナンバー

	// スコアデータをコピー
	for (const auto& entry : scoreData)
	{
		std::string result = text + std::to_string(stageNumber);
		saveData[result] = entry.second;
		stageNumber++;
	}
	// 保存
	CSVManager::SaveCSVScore(saveData, "Assets/StageDataList/SaveData.csv");
}

// CSVからスコアデータを読み取る関数
void GameManager::LoadScoreData()
{
	std::map<std::string, std::variant<int, float>> loadData; // 読み込みデータ

	// CSVからデータをロード（ファイルがない場合初期値を保存）
	if (!CSVManager::LoadCSVStageData(loadData, "Assets/StageDataList/SaveData.csv",CSVManager::INT))
	{
		SaveScoreData();
		return;
	}

	// 読み込みデータをscoreDataにコピー
	int indexCount = 0;
	for (const auto& entry : loadData)
	{
		scoreData[static_cast<SELECT_STAGE>(indexCount)] = std::get<int>(entry.second);
		indexCount++;
	}
}

// CSVからステージのタイムデータを読み込み
void GameManager::LoadStageTimeData()
{
	std::map<std::string, std::variant<int, float>> loadData; // 読み込みデータ
	// 読み込み
	CSVManager::LoadCSVStageData(loadData, "Assets/StageDataList/StageTimeData.csv", CSVManager::FLOAT);

	// 読み込んだデータをstageTimeDataにコピー
	int indexCount = 0;
	for (const auto& entry : loadData)
	{
		stageTimeData[static_cast<SELECT_STAGE>(indexCount)] = std::get<float>(entry.second);
		indexCount++;
	}
}

// スコアを加算する関数（GameSceneで使用）
void GameManager::AddStageScore(int _score)
{
	stageScore += _score; // スコア加算

	// 最大値を超えないようにする
	stageScore = std::clamp(stageScore, 0, MaxScore);
}

// 最大スコアを更新し、その結果を返す関数
bool GameManager::IsHighScoreUpdated()
{
	// 履歴のステージスコアより多い場合上書き
	if (scoreData[selectStage] < stageScore)
	{
		scoreData[selectStage] = stageScore; // スコア上書き

		return true; // 最大スコアが更新されたのでtrue
	}

	return false; // 更新されてないのでfalse
}