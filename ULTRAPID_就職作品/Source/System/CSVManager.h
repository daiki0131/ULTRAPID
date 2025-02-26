#pragma once
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>
#include <variant>
#include <DirectXMath.h>
#include "GameScene.h"

//===========================================//
// CSV読み書きを管理するクラス
//===========================================//
class CSVManager
{
public:
	// 描き込む変数の型
	enum TypeName
	{
		INT,
		FLOAT,
		BOOL,
		FLOAT3,
	};

public:
	// ゲーム内のパラメータを保存
	static void SaveCSVPrameters(
		const std::map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>>& myMap, 
		const std::string& filename);
	// ゲーム内のパラメータを読み込む
	static bool LoadCSVPrameters(
		std::unordered_map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>>& myMap,
		const std::string& filename);
	// ゲーム内のスコアを保存
	static void SaveCSVScore(const std::map < std::string, int>& myMap, const std::string& filename);
	// ゲーム内のステージデータを読み込む
	static bool LoadCSVStageData(std::map < std::string, std::variant<int, float>>& myMap, const std::string& filename, TypeName);
	// ステージマップの読み込み
	static void LoadCSVMapData(std::vector<GameScene::MapObject>& data, const std::string& filename);
};