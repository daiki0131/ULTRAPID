#include "DebugConsole.h"
#include <Windows.h>
#include <string>
#include <fstream>

std::vector<std::string> DebugConsole::messeageLog;

// �f�o�b�O�E�C���h�E�����グ����
void DebugConsole::CreateDebugConsole()
{
#ifdef _DEBUG
    // �R���\�[�������蓖�Ă�
    AllocConsole();

    // �W���o�́A�W���G���[�o�́A�W�����͂̃��_�C���N�g
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);   // �W���o�͂��R���\�[���Ƀ��_�C���N�g
    freopen_s(&fp, "CONOUT$", "w", stderr);   // �W���G���[�o�͂��R���\�[���Ƀ��_�C���N�g
    freopen_s(&fp, "CONIN$", "r", stdin);     // �W�����͂��R���\�[���Ƀ��_�C���N�g

    // �R���\�[���̃o�b�t�@�T�C�Y��ݒ�
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { 80, 300 }; // �o�b�t�@�̃T�C�Y (��80, ����300)
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // �R���\�[���E�B���h�E�̃T�C�Y��ݒ�
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow)
    {
        // �E�B���h�E�ʒu�ƃT�C�Y��ݒ� (X=100, Y=100, ��=960, ����=540)
        MoveWindow(consoleWindow, 100, 100, 960, 540, TRUE); // (X, Y, Width, Height)

        // �V�X�e�����j���[�������{�^�����폜
        HMENU hMenu = GetSystemMenu(consoleWindow, FALSE);
        if (hMenu)
        {
            RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND); // �R���\�[���E�B���h�E�����{�^��������
        }
    }

    // �f�o�b�O�p���b�Z�[�W�̏o��
    std::cout << "Create Debug Console" << std::endl;

#endif
}

// �f�o�b�O���O���o�͂���֐�
void DebugConsole::DebugLog(const std::string& _message, const LOG_TYPE _type)
{
#ifdef _DEBUG
    // ���O�o��
    switch (_type)
    {
    case DebugConsole::INFO_LOG:    // �ʏ탍�O
        std::cout << "Log       :" << _message << std::endl;
        messeageLog.push_back("Log       :" + _message);
        break;
    case DebugConsole::WARNING_LOG: // �x�����O
        std::cout << "WarningLog:" << _message << std::endl;
        messeageLog.push_back("WarningLog:" + _message);
        break;
    case DebugConsole::ERROR_LOG:  // �G���[���O
        MessageBoxA(NULL, _message.c_str(), "�G���[", MB_OK | MB_ICONERROR);
        std::cout << "ErrorLog  :" << _message << std::endl;
        messeageLog.push_back("ErrorLog  :" + _message);
        messeageLog.push_back("�G���[�������������߃A�v���P�[�V�����������I�����܂���");
        PostQuitMessage(1); // �ُ�I���i�v���O�����I���j
        break;
    default:
        break;
    }
#else
    switch (_type)
    {
    case DebugConsole::ERROR_LOG:
        MessageBoxA(NULL, "�G���[�������������߃A�v���P�[�V�����I�����܂��B", "�G���[", MB_OK | MB_ICONERROR);
        PostQuitMessage(1);
        break;
    default:
        break;
    }
   
#endif
}

// �e�L�X�g�t�@�C���Ƀf�o�b�O�������㏑���ŕۑ�����
void DebugConsole::SaveLogMesseage()
{
#ifdef _DEBUG

    std::ofstream outFile("DebugLog.txt", std::ios::trunc);

    // �t�@�C��������ɊJ�����ꍇ
    if (outFile.is_open())
    {
        // ���b�Z�[�W���O (messeageLog) �̊e���b�Z�[�W���t�@�C���ɏ�������
        for (const auto& msg : messeageLog)
        {
            outFile << msg << std::endl;
        }

        // �t�@�C�������
        outFile.close();
    }
#endif
}