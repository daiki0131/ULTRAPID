#pragma once
#undef max
#undef min
#include <iostream>
#include <algorithm>

//===========================================//
// FPS���Ǘ�����N���X
//===========================================//
class FPS
{
private:
	float fpsLimit;                            // 1�b�Ԃ̃t���[����
	float frameTime;                           // 1�t���[���̎���
	float deltaTime;                           // �O�̃t���[������̌o�ߎ���
	float currentFPS;                          // ���݂�FPS
	const float fixedDeltaTime = 0.01666667f;  // �Œ�̃t���[������
	LARGE_INTEGER frequency;                   // �J�E���^�̎��g�� (1�b������̃J�E���g��) ��ۑ�
	LARGE_INTEGER lastTime;					   // �Ō�ɋL�^�������� (�J�E���^�̒l) ��ۑ�
	
public:
	
	// ������
	void Init(float _fps)
	{
		fpsLimit = _fps;
		frameTime = 1.0f / fpsLimit;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&lastTime);
		deltaTime = 0;
		currentFPS = 0;
	}

	// �X�V
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

		// ���̃t���[���̂��߂Ɏ��Ԃ��X�V
		QueryPerformanceCounter(&lastTime);

		currentFPS = 1.0f / GetDeltaTime();
	}

	// Get�֐�
	float GetFPS() { return currentFPS; }
	float GetDeltaTime() { return std::max(frameTime, std::min(deltaTime, 0.1f)); }
	float GetFixedDeltaTime() { return fixedDeltaTime; }

	// �V���O���g��
	static FPS* Get()
	{
		static FPS instance;
		return &instance;
	}
};



