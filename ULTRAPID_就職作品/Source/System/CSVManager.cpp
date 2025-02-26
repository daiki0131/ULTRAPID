#include "CSVManager.h"
#include "DebugConsole.h"

// �Q�[�����̃p�����[�^��CSV�ɕۑ�����
void CSVManager::SaveCSVPrameters(const std::map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>>& myMap, const std::string& filename)
{
    // �t�@�C�����J��
    std::ofstream file(filename);
    if (!file.is_open())
    {
        // �t�@�C�����J���Ȃ������ꍇ�̃G���[���b�Z�[�W
        std::string message = "�t�@�C�� " + filename + " ���J�����Ƃ��ł��܂���ł����B�FCSVManager::SaveCSVPrameters���Ŕ���";
        DebugConsole::DebugLog(message.c_str(), DebugConsole::ERROR_LOG);
        return;
    }

    // CSV�̃w�b�_�[�s����������
    file << "Key,Value,Type\n";

    // myMap�̊e�G���g�������[�v
    for (const auto& entry : myMap)
    {
        const std::string& key = entry.first;        // �G���g���̃L�[�i���O�j
        const auto& value = entry.second.first;      // �G���g���̒l�i�^�ɂ���ĈقȂ�j
        const TypeName& type = entry.second.second;  // �G���g���̌^

        // �L�[����������
        file << key << ",";

        // �^���Ƃɒl��K�؂ɏ�������
        switch (type)
        {
        case TypeName::INT:
            file << std::get<int>(value) << ",";  // int�̏ꍇ
            break;
        case TypeName::FLOAT:
            file << std::get<float>(value) << ",";  // float�̏ꍇ
            break;
        case TypeName::BOOL:
            file << (std::get<bool>(value) ? "true" : "false") << ",";  // bool�̏ꍇ
            break;
        case TypeName::FLOAT3:
        {
            const DirectX::XMFLOAT3& float3 = std::get<DirectX::XMFLOAT3>(value);
            file << float3.x << "|" << float3.y << "|" << float3.z << ",";  // XMFLOAT3�̏ꍇ�A'|'�ŋ�؂�
        }
        break;
        }

        // �^�̎��ʂ���������
        switch (type)
        {
        case TypeName::INT:
            file << "int" << "\n";   // �^��int�̏ꍇ
            break;
        case TypeName::FLOAT:
            file << "float" << "\n"; // �^��float�̏ꍇ
            break;
        case TypeName::BOOL:
            file << "bool" << "\n";  // �^��bool�̏ꍇ
            break;
        case TypeName::FLOAT3:
            file << "float3" << "\n";// �^��XMFLOAT3�̏ꍇ
            break;
        default:
            break;
        }
    }

    // �t�@�C�������
    file.close();
}

// �Q�[�����p�����[�^��CSV����ǂݎ��
bool CSVManager::LoadCSVPrameters(std::unordered_map<std::string, std::pair<std::variant<int, float, bool, DirectX::XMFLOAT3>, CSVManager::TypeName>>& myMap, const std::string& filename)
{
    // �t�@�C�����J��
    std::ifstream file(filename);

    // �t�@�C�����J���Ȃ��ꍇ
    if (!file.is_open())
    {
        return false;
    }

    std::string line;

    // �ŏ��̍s�i�w�b�_�[�j���X�L�b�v
    std::getline(file, line);

    // CSV�̊e�s��ǂݍ���
    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        std::string key;
        std::string valueStr;
        std::string typeStr;

        // �J���}��؂�ŃL�[�A�l�A�^�𕪊�
        if (std::getline(ss, key, ',') && std::getline(ss, valueStr, ',') && std::getline(ss, typeStr)) {
            // �^�̎��ʔԍ����m�F���ATypeName�Ƀ}�b�s���O
            CSVManager::TypeName type;

            // �^�ɉ����Ēl���i�[
            if (typeStr == "int")
            {
                type = CSVManager::TypeName::INT;
                myMap[key] = { std::stoi(valueStr), type };    // �l��int�Ƃ��Ċi�[
            }
            else if (typeStr == "float") {
                type = CSVManager::TypeName::FLOAT;
                myMap[key] = { std::stof(valueStr), type };    // �l��float�Ƃ��Ċi�[
            }
            else if (typeStr == "bool")
            {
                type = CSVManager::TypeName::BOOL;
                myMap[key] = { (valueStr == "true"), type };   // �l��bool�Ƃ��Ċi�[
            }
            else if (typeStr == "float3")  // XMFLOAT3�̏ꍇ
            {
                type = CSVManager::TypeName::FLOAT3;

                std::stringstream ss(valueStr);
                std::string part;
               
                // 1�ڂ̒l�ix�j
                std::getline(ss, part, '|');
                float x = std::stof(part);  // string �� float

                // 2�ڂ̒l�iy�j
                std::getline(ss, part, '|');
                float y = std::stof(part);  // string �� float

                // 3�ڂ̒l�iz�j
                std::getline(ss, part, '|');
                float z = std::stof(part);  // string �� float
                myMap[key] = { DirectX::XMFLOAT3(x, y, z), type };  // �l��XMFLOAT3�Ƃ��Ċi�[
            }
            else {
                // �^���s���ȏꍇ
                MessageBoxA(NULL, "���݂��Ȃ��^���Fint�^�œo�^", "�G���[", MB_OK | MB_ICONERROR);
                type = CSVManager::TypeName::INT;
                myMap[key] = { 0, type };  // �l��int�Ƃ��Ċi�[
            }
        }
    }

    // �t�@�C�������
    file.close();
    return true;
}

// �Q�[�����X�R�A��CSV�ɕۑ�
void CSVManager::SaveCSVScore(const std::map<std::string, int>& myMap, const std::string& filename)
{
    // �t�@�C�����J��
    std::ofstream file(filename);
    if (!file.is_open())
    {
        // �t�@�C�����J���Ȃ��ꍇ�̓G���[���b�Z�[�W��\�����ă��^�[��
        std::string message = "�t�@�C�� " + filename + " ���J�����Ƃ��ł��܂���ł����B�FCSVManager::SaveCSVScore���Ŕ���";
        DebugConsole::DebugLog(message.c_str(), DebugConsole::ERROR_LOG);
        return;
    }

    // CSV�̃w�b�_�[�s����������
    file << "StageName, Score\n";

    // myMap�̊e�G���g�������[�v���ăt�@�C���ɏ�������
    for (const auto& entry : myMap)
    {
        const std::string& key = entry.first; // �X�e�[�W��
        const auto& value = entry.second;     // �X�R�A

        // �L�[����������
        file << key << ",";
        file << value << "\n";

    }

    // �t�@�C�������
    file.close();
}

// �Q�[�����̃X�e�[�W�f�[�^��ǂݍ���
bool CSVManager::LoadCSVStageData(std::map<std::string, std::variant<int, float>>& myMap, const std::string& filename, const TypeName type)
{
    // �t�@�C�����J��
    std::ifstream file(filename);

    // �t�@�C�����J���Ȃ��ꍇ
    if (!file.is_open())
    {
        return false;
    }

    std::string line;

    // �ŏ��̍s�i�w�b�_�[�j���X�L�b�v
    std::getline(file, line);

    // CSV�̊e�s��ǂݍ���
    while (std::getline(file, line))
    {
        std::stringstream ss(line); // �s���X�g���[���Ƃ��ĉ��
        std::string key;            // �L�[���i�[����ϐ�
        std::string valueStr;       // �l���i�[����ϐ�
        int nValue;                 // int�^�̒l
        float fValue;               // float�^�̒l

        // �J���}��؂�ŃL�[�ƃX�R�A�̒l���擾
        if (std::getline(ss, key, ',') && std::getline(ss, valueStr))
        {
            switch (type)
            {
            case CSVManager::TypeName::INT:
                nValue = std::stoi(valueStr);  // �l��int�^�ɕϊ�
                myMap[key] = nValue;           // �}�b�v�Ɋi�[
                break;
            case CSVManager::TypeName::FLOAT:
                fValue = std::stof(valueStr);  // �l��float�^�ɕϊ�
                myMap[key] = fValue;           // �}�b�v�Ɋi�[
                break;
            default:
                break;
            }
        }
    }
    file.close();
    return true;
}


// �Q�[�����̃X�e�[�W�̃}�b�v�f�[�^��ǂݍ���
void CSVManager::LoadCSVMapData(std::vector<GameScene::MapObject>& data, const std::string& filename)
{
    // �t�@�C�����J��
    std::ifstream file(filename);

    // �s���i�[���邽�߂̕ϐ�
    std::string line;

    // �t�@�C���̊e�s��ǂݍ���
    while (std::getline(file, line))
    {
        // �s���J���}��؂�ŉ�͂��邽�߂̃X�g���[��
        std::istringstream ss(line);

        // �ǂݍ��ݐ�p�ϐ�
        GameScene::MapObject obj;
        std::string value;

        // �ʒu
        std::getline(ss, value, ',');
        obj.position.x = std::stof(value);  // x���W

        std::getline(ss, value, ',');
        obj.position.y = std::stof(value);  // y���W

        std::getline(ss, value, ',');
        obj.position.z = std::stof(value);  // z���W

        // ��]
        std::getline(ss, value, ',');
        obj.rotation.x = std::stof(value);  // x����]

        std::getline(ss, value, ',');
        obj.rotation.y = std::stof(value);  // y����]

        std::getline(ss, value, ',');
        obj.rotation.z = std::stof(value);  // z����]

        // �X�P�[��
        std::getline(ss, value, ',');
        obj.scale.x = std::stof(value);     // x�X�P�[��

        std::getline(ss, value, ',');
        obj.scale.y = std::stof(value);     // y�X�P�[��

        std::getline(ss, value, ',');
        obj.scale.z = std::stof(value);     // z�X�P�[��

        // �I�u�W�F�N�g�^�C�v
        std::getline(ss, value, ',');
        obj.objType = static_cast<GameScene::MAP_OBJECT_TYPE>(std::stoi(value));

        // �}�b�v�f�[�^��ǉ�
        data.push_back(obj);
    }

    // �t�@�C�������
    file.close();
}