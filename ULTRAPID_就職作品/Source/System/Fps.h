#pragma once
#undef max
#undef min
#include <iostream>
#include <algorithm>

//===========================================//
// FPSを管理するクラス
//===========================================//
class FPS
{
private:
	float fpsLimit;                            // 1秒間のフレーム数
	float frameTime;                           // 1フレームの時間
	float deltaTime;                           // 前のフレームからの経過時間
	float currentFPS;                          // 現在のFPS
	const float fixedDeltaTime = 0.01666667f;  // 固定のフレーム時間
	LARGE_INTEGER frequency;                   // カウンタの周波数 (1秒あたりのカウント数) を保存
	LARGE_INTEGER lastTime;					   // 最後に記録した時刻 (カウンタの値) を保存
	
public:
	
	// 初期化
	void Init(float _fps)
	{
		fpsLimit = _fps;
		frameTime = 1.0f / fpsLimit;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&lastTime);
		deltaTime = 0;
		currentFPS = 0;
	}

	// 更新
	void Update()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		deltaTime = (currentTime.QuadPart - lastTime.QuadPart) / (float)frequency.QuadPart;

		if (deltaTime > 0.02f)
		{
			int hoge = 0;
		}

		if (deltaTime < frameTime)
		{
			while ((currentTime.QuadPart - lastTime.QuadPart) / (float)frequency.QuadPart < frameTime)
			{
				QueryPerformanceCounter(&currentTime);
			}
		}

		// 次のフレームのために時間を更新
		QueryPerformanceCounter(&lastTime);

		currentFPS = 1.0f / GetDeltaTime();
	}

	// Get関数
	float GetFPS() { return currentFPS; }
	float GetDeltaTime() { return std::max(frameTime, std::min(deltaTime, 0.1f)); }
	float GetFixedDeltaTime() { return fixedDeltaTime; }

	// シングルトン
	static FPS* Get()
	{
		static FPS instance;
		return &instance;
	}
};



