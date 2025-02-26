#pragma once
#include <iostream>
#include <vector>

//===========================================//
// デバッグウインドウ専用クラス
//===========================================//
class DebugConsole
{
public:
	enum LOG_TYPE 
	{
		INFO_LOG = 0,
		WARNING_LOG = 1,
		ERROR_LOG = 2,
	};
private:
	static std::vector<std::string> messeageLog;

public:
	// 初期化処理
	static void CreateDebugConsole();
	// ログ出力
	static void DebugLog(const std::string& _message, const LOG_TYPE _type);
	// 結果ファイル出力
	static void SaveLogMesseage();
};

