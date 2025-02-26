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
// CSV�ǂݏ������Ǘ�����N���X
//===========================================//
class CSVManager
{
public:
	// �`�����ޕϐ��̌^
	enum TypeName
	{
		INT,
		FLOAT,
		BOOL,
		FLOAT3,
	};

public:
	// �Q�[�����̃p�����[�^��ۑ�
	static void SaveCSVPrameters(
		const std::map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>>& myMap, 
		const std::string& filename);
	// �Q�[�����̃p�����[�^��ǂݍ���
	static bool LoadCSVPrameters(
		std::unordered_map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>>& myMap,
		const std::string& filename);
	// �Q�[�����̃X�R�A��ۑ�
	static void SaveCSVScore(const std::map < std::string, int>& myMap, const std::string& filename);
	// �Q�[�����̃X�e�[�W�f�[�^��ǂݍ���
	static bool LoadCSVStageData(std::map < std::string, std::variant<int, float>>& myMap, const std::string& filename, TypeName);
	// �X�e�[�W�}�b�v�̓ǂݍ���
	static void LoadCSVMapData(std::vector<GameScene::MapObject>& data, const std::string& filename);
};