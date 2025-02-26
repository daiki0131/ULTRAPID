#include "CSVManager.h"
#include "DebugConsole.h"

// ゲーム内のパラメータをCSVに保存する
void CSVManager::SaveCSVPrameters(const std::map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>>& myMap, const std::string& filename)
{
    // ファイルを開く
    std::ofstream file(filename);
    if (!file.is_open())
    {
        // ファイルが開けなかった場合のエラーメッセージ
        std::string message = "ファイル " + filename + " を開くことができませんでした。：CSVManager::SaveCSVPrameters内で発生";
        DebugConsole::DebugLog(message.c_str(), DebugConsole::ERROR_LOG);
        return;
    }

    // CSVのヘッダー行を書き込む
    file << "Key,Value,Type\n";

    // myMapの各エントリをループ
    for (const auto& entry : myMap)
    {
        const std::string& key = entry.first;        // エントリのキー（名前）
        const auto& value = entry.second.first;      // エントリの値（型によって異なる）
        const TypeName& type = entry.second.second;  // エントリの型

        // キーを書き込む
        file << key << ",";

        // 型ごとに値を適切に書き込む
        switch (type)
        {
        case TypeName::INT:
            file << std::get<int>(value) << ",";  // intの場合
            break;
        case TypeName::FLOAT:
            file << std::get<float>(value) << ",";  // floatの場合
            break;
        case TypeName::BOOL:
            file << (std::get<bool>(value) ? "true" : "false") << ",";  // boolの場合
            break;
        case TypeName::FLOAT3:
        {
            const DirectX::XMFLOAT3& float3 = std::get<DirectX::XMFLOAT3>(value);
            file << float3.x << "|" << float3.y << "|" << float3.z << ",";  // XMFLOAT3の場合、'|'で区切る
        }
        break;
        }

        // 型の識別を書き込む
        switch (type)
        {
        case TypeName::INT:
            file << "int" << "\n";   // 型がintの場合
            break;
        case TypeName::FLOAT:
            file << "float" << "\n"; // 型がfloatの場合
            break;
        case TypeName::BOOL:
            file << "bool" << "\n";  // 型がboolの場合
            break;
        case TypeName::FLOAT3:
            file << "float3" << "\n";// 型がXMFLOAT3の場合
            break;
        default:
            break;
        }
    }

    // ファイルを閉じる
    file.close();
}

// ゲーム内パラメータをCSVから読み取る
bool CSVManager::LoadCSVPrameters(std::unordered_map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>>& myMap, const std::string& filename)
{
    // ファイルを開く
    std::ifstream file(filename);

    // ファイルが開けない場合
    if (!file.is_open())
    {
        return false;
    }

    std::string line;

    // 最初の行（ヘッダー）をスキップ
    std::getline(file, line);

    // CSVの各行を読み込む
    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        std::string key;
        std::string valueStr;
        std::string typeStr;

        // カンマ区切りでキー、値、型を分割
        if (std::getline(ss, key, ',') && std::getline(ss, valueStr, ',') && std::getline(ss, typeStr)) {
            // 型の識別番号を確認し、TypeNameにマッピング
            CSVManager::TypeName type;

            // 型に応じて値を格納
            if (typeStr == "int")
            {
                type = CSVManager::TypeName::INT;
                myMap[key] = { std::stoi(valueStr), type };    // 値をintとして格納
            }
            else if (typeStr == "float") {
                type = CSVManager::TypeName::FLOAT;
                myMap[key] = { std::stof(valueStr), type };    // 値をfloatとして格納
            }
            else if (typeStr == "bool")
            {
                type = CSVManager::TypeName::BOOL;
                myMap[key] = { (valueStr == "true"), type };   // 値をboolとして格納
            }
            else if (typeStr == "float3")  // XMFLOAT3の場合
            {
                type = CSVManager::TypeName::FLOAT3;

                std::stringstream ss(valueStr);
                std::string part;
               
                // 1つ目の値（x）
                std::getline(ss, part, '|');
                float x = std::stof(part);  // string → float

                // 2つ目の値（y）
                std::getline(ss, part, '|');
                float y = std::stof(part);  // string → float

                // 3つ目の値（z）
                std::getline(ss, part, '|');
                float z = std::stof(part);  // string → float
                myMap[key] = { DirectX::XMFLOAT3(x, y, z), type };  // 値をXMFLOAT3として格納
            }
            else {
                // 型が不正な場合
                MessageBoxA(NULL, "存在しない型名：int型で登録", "エラー", MB_OK | MB_ICONERROR);
                type = CSVManager::TypeName::INT;
                myMap[key] = { 0, type };  // 値をintとして格納
            }
        }
    }

    // ファイルを閉じる
    file.close();
    return true;
}

// ゲーム内スコアをCSVに保存
void CSVManager::SaveCSVScore(const std::map<std::string, int>& myMap, const std::string& filename)
{
    // ファイルを開く
    std::ofstream file(filename);
    if (!file.is_open())
    {
        // ファイルが開けない場合はエラーメッセージを表示してリターン
        std::string message = "ファイル " + filename + " を開くことができませんでした。：CSVManager::SaveCSVScore内で発生";
        DebugConsole::DebugLog(message.c_str(), DebugConsole::ERROR_LOG);
        return;
    }

    // CSVのヘッダー行を書き込む
    file << "StageName, Score\n";

    // myMapの各エントリをループしてファイルに書き込む
    for (const auto& entry : myMap)
    {
        const std::string& key = entry.first; // ステージ名
        const auto& value = entry.second;     // スコア

        // キーを書き込む
        file << key << ",";
        file << value << "\n";

    }

    // ファイルを閉じる
    file.close();
}

// ゲーム内のステージデータを読み込む
bool CSVManager::LoadCSVStageData(std::map<std::string, std::variant<int, float>>& myMap, const std::string& filename, const TypeName type)
{
    // ファイルを開く
    std::ifstream file(filename);

    // ファイルが開けない場合
    if (!file.is_open())
    {
        return false;
    }

    std::string line;

    // 最初の行（ヘッダー）をスキップ
    std::getline(file, line);

    // CSVの各行を読み込む
    while (std::getline(file, line))
    {
        std::stringstream ss(line); // 行をストリームとして解析
        std::string key;            // キーを格納する変数
        std::string valueStr;       // 値を格納する変数
        int nValue;                 // int型の値
        float fValue;               // float型の値

        // カンマ区切りでキーとスコアの値を取得
        if (std::getline(ss, key, ',') && std::getline(ss, valueStr))
        {
            switch (type)
            {
            case CSVManager::TypeName::INT:
                nValue = std::stoi(valueStr);  // 値をint型に変換
                myMap[key] = nValue;           // マップに格納
                break;
            case CSVManager::TypeName::FLOAT:
                fValue = std::stof(valueStr);  // 値をfloat型に変換
                myMap[key] = fValue;           // マップに格納
                break;
            default:
                break;
            }
        }
    }
    file.close();
    return true;
}


// ゲーム内のステージのマップデータを読み込む
void CSVManager::LoadCSVMapData(std::vector<GameScene::MapObject>& data, const std::string& filename)
{
    // ファイルを開く
    std::ifstream file(filename);

    // 行を格納するための変数
    std::string line;

    // ファイルの各行を読み込む
    while (std::getline(file, line))
    {
        // 行をカンマ区切りで解析するためのストリーム
        std::istringstream ss(line);

        // 読み込み専用変数
        GameScene::MapObject obj;
        std::string value;

        // 位置
        std::getline(ss, value, ',');
        obj.position.x = std::stof(value);  // x座標

        std::getline(ss, value, ',');
        obj.position.y = std::stof(value);  // y座標

        std::getline(ss, value, ',');
        obj.position.z = std::stof(value);  // z座標

        // 回転
        std::getline(ss, value, ',');
        obj.rotation.x = std::stof(value);  // x軸回転

        std::getline(ss, value, ',');
        obj.rotation.y = std::stof(value);  // y軸回転

        std::getline(ss, value, ',');
        obj.rotation.z = std::stof(value);  // z軸回転

        // スケール
        std::getline(ss, value, ',');
        obj.scale.x = std::stof(value);     // xスケール

        std::getline(ss, value, ',');
        obj.scale.y = std::stof(value);     // yスケール

        std::getline(ss, value, ',');
        obj.scale.z = std::stof(value);     // zスケール

        // オブジェクトタイプ
        std::getline(ss, value, ',');
        obj.objType = static_cast<GameScene::MAP_OBJECT_TYPE>(std::stoi(value));

        // マップデータを追加
        data.push_back(obj);
    }

    // ファイルを閉じる
    file.close();
}