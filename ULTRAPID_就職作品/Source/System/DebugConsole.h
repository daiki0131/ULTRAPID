#pragma once
#include <iostream>
#include <vector>

//===========================================//
// �f�o�b�O�E�C���h�E��p�N���X
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
	// ����������
	static void CreateDebugConsole();
	// ���O�o��
	static void DebugLog(const std::string& _message, const LOG_TYPE _type);
	// ���ʃt�@�C���o��
	static void SaveLogMesseage();
};

