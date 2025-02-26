#pragma once
#include <map>
#include <random>

#define D2R(deg) ((deg)*3.14159265f/180.0f) // 度からラジアン変換のマクロ定義

//===========================================//
// ゲーム内データを管理するクラス
//===========================================//
class GameManager
{
public:
	// 選択ステージ
	enum SELECT_STAGE
	{
		STAGE_1,
		STAGE_2,
		STAGE_3,
		STAGE_MAX,
	};
	
	static std::random_device rd;                         // 乱数生成用
	static std::mt19937 mt;                               // 乱数生成用
	static constexpr float GravityScale = -19.6f;		  // 重力
	static constexpr float RadToDeg = 180.0f / 3.1415926f;// ラジアンから度に変換   
	static constexpr float FullRotationDegress = 360.0f;  // 1回転 
	static constexpr int MaxScore = 99999;				  // 最大スコア
	static constexpr int Max3DobjectCount = 10000;		  // 1つのタイプのオブジェクト最大数
private:
	static SELECT_STAGE selectStage;					  // 現在の選択ステージ
	std::map<SELECT_STAGE, int> scoreData;                // 各ステージのスコアデータ
	static std::map<SELECT_STAGE, float> stageTimeData;   // 各ステージの制限時間データ
	static int stageScore;                                // 現在獲得のスコア

public:
	void Init();
	void UnInit();
	// ステージに入る前の初期化処理
	static void InitStageScore() { stageScore = 0; };
	// Set関数
	void SetSelectStage(SELECT_STAGE _stage) { selectStage = _stage; }
	// Get関数
	static const SELECT_STAGE GetSelectStage() { return selectStage; }
	int GetScore(SELECT_STAGE _stage) { return scoreData[_stage]; }
	int GetStageScore() { return stageScore; }
	static const float GetStageTimeData() { return stageTimeData[selectStage]; }
	// CSV保存（スコア）
	void SaveScoreData();
	// CSV読み込み（スコア）
	void LoadScoreData();
	// CSV読み込み（制限時間）
	void LoadStageTimeData();
	// スコア加算
	void AddStageScore(int _score);
	// 最大スコアを加算して更新した場合の結果を返す
	bool IsHighScoreUpdated();
};