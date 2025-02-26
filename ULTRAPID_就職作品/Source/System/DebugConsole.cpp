#include "DebugConsole.h"
#include <Windows.h>
#include <string>
#include <fstream>

std::vector<std::string> DebugConsole::messeageLog;

// デバッグウインドウ立ち上げ処理
void DebugConsole::CreateDebugConsole()
{
#ifdef _DEBUG
    // コンソールを割り当てる
    AllocConsole();

    // 標準出力、標準エラー出力、標準入力のリダイレクト
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);   // 標準出力をコンソールにリダイレクト
    freopen_s(&fp, "CONOUT$", "w", stderr);   // 標準エラー出力をコンソールにリダイレクト
    freopen_s(&fp, "CONIN$", "r", stdin);     // 標準入力をコンソールにリダイレクト

    // コンソールのバッファサイズを設定
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { 80, 300 }; // バッファのサイズ (幅80, 高さ300)
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // コンソールウィンドウのサイズを設定
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow)
    {
        // ウィンドウ位置とサイズを設定 (X=100, Y=100, 幅=960, 高さ=540)
        MoveWindow(consoleWindow, 100, 100, 960, 540, TRUE); // (X, Y, Width, Height)

        // システムメニューから閉じるボタンを削除
        HMENU hMenu = GetSystemMenu(consoleWindow, FALSE);
        if (hMenu)
        {
            RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND); // コンソールウィンドウを閉じるボタン無効化
        }
    }

    // デバッグ用メッセージの出力
    std::cout << "Create Debug Console" << std::endl;

#endif
}

// デバッグログを出力する関数
void DebugConsole::DebugLog(const std::string& _message, const LOG_TYPE _type)
{
#ifdef _DEBUG
    // ログ出力
    switch (_type)
    {
    case DebugConsole::INFO_LOG:    // 通常ログ
        std::cout << "Log       :" << _message << std::endl;
        messeageLog.push_back("Log       :" + _message);
        break;
    case DebugConsole::WARNING_LOG: // 警告ログ
        std::cout << "WarningLog:" << _message << std::endl;
        messeageLog.push_back("WarningLog:" + _message);
        break;
    case DebugConsole::ERROR_LOG:  // エラーログ
        MessageBoxA(NULL, _message.c_str(), "エラー", MB_OK | MB_ICONERROR);
        std::cout << "ErrorLog  :" << _message << std::endl;
        messeageLog.push_back("ErrorLog  :" + _message);
        messeageLog.push_back("エラーが発生したためアプリケーションを強制終了しました");
        PostQuitMessage(1); // 異常終了（プログラム終了）
        break;
    default:
        break;
    }
#else
    switch (_type)
    {
    case DebugConsole::ERROR_LOG:
        MessageBoxA(NULL, "エラーが発生したためアプリケーション終了します。", "エラー", MB_OK | MB_ICONERROR);
        PostQuitMessage(1);
        break;
    default:
        break;
    }
   
#endif
}

// テキストファイルにデバッグ履歴を上書きで保存する
void DebugConsole::SaveLogMesseage()
{
#ifdef _DEBUG

    std::ofstream outFile("DebugLog.txt", std::ios::trunc);

    // ファイルが正常に開けた場合
    if (outFile.is_open())
    {
        // メッセージログ (messeageLog) の各メッセージをファイルに書き込む
        for (const auto& msg : messeageLog)
        {
            outFile << msg << std::endl;
        }

        // ファイルを閉じる
        outFile.close();
    }
#endif
}