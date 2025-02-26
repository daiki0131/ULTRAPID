#pragma once
#include "Renderer.h"
#include "Vector.h"
#include "Collision.h"

//===========================================//
// �p�[�e�B�N���N���X
//===========================================//
class Particle
{
public:
	static constexpr int MaxThreadDataNum = 256;    // CPU1�X���b�h������̌v�Z��
	static constexpr float MaxLifeTime = 3.0f;      // �ő吶������
	static constexpr float MinLifeTime = 2.0f;      // �ŏ���������
	static constexpr float MaxSpeed = 30.0f;        // �ő呬�x
	static constexpr float MinSpeed = 20.0f;        // �ŏ����x
	static constexpr float MaxSize = 0.5f;          // �ő�T�C�Y
	static constexpr int MaxParticleNum = 10000;     // �ő�̃p�[�e�B�N����
	static constexpr float MaxGravity = 9.8f;       // �ő�̏d�͒l
	static constexpr float MaxAlpha = 1.0f;         // �ő�̃��l
	static constexpr float MaxViewDistance = 100.0f; // �p�[�e�B�N���������ł��鋗��

	// �p�[�e�B�N���f�[�^
	struct ParticleData
	{
		DirectX::XMFLOAT3 position;       // ���W
		DirectX::XMFLOAT4 color;          // �F
		DirectX::XMFLOAT3 velocity;       // ����
		float speed;                      // ���x
		float lifeTime;                   // ��������
		float ageTime;                    // �o�ߎ���
		unsigned int isActive;            // ������
		unsigned int isVisible;
		
	};
	// �p�[�e�B�N�����ʃo�b�t�@
	struct ParameterBuffer
	{
		float deltaTime;                  // �t���[������
		float gravity;                    // �d��
		float maxAlpha;                   // ���l
		unsigned int drawNum;             // �`�搔
		DirectX::XMFLOAT3 cameraPosition;
		float maxViewDistance;
		DirectX::XMFLOAT4 frustumPlanes[6];
	};
protected:
	static ComPtr<ID3D11Buffer> particleBuffer;           // �p�[�e�B�N���f�[�^�̃o�b�t�@
	static ComPtr<ID3D11Buffer> parameterBuffer;          // ���ʕ����̃o�b�t�@
	static ComPtr<ID3D11Buffer> stagingBuffer;            // GPU ���� CPU �փf�[�^��ǂݍ��ނ��߂̒��ԃo�b�t�@
	static ComPtr<ID3D11UnorderedAccessView> uavBuffer;   // �s�N�Z���V�F�[�_�[��R���s���[�g�V�F�[�_�[���烉���_���ɓǂݏ����ł���o�b�t�@

	Renderer::Model model;
	std::vector<Renderer::ObjectBuffer> instanceDatas;                 // Renderer���M�p�I�u�W�F�N�g�f�[�^
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;            // �g�p���钸�_�V�F�[�_�[
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;            // �g�p����s�N�Z���V�F�[�_�[
	std::vector<Particle::ParticleData> data;             // �p�[�e�B�N���f�[�^
	std::vector<Particle::ParticleData> sendData;
	float maxLifeTime = MaxLifeTime;                      // �ő吶������
	float minLifeTime = MinLifeTime;                      // �ŏ���������
	float maxSpeed = MaxSpeed;                            // �ő呬�x
	float minSpeed = MinSpeed;                            // �ŏ����x
	float size = MaxSize;                                 // �ő�T�C�Y
	float maxAlpha = 0.5f;                                // �ő僿�l
	float gravity = MaxGravity;                           // �ő�d��
	int maxParticleNum = MaxParticleNum;                  // �ő�p�[�e�B�N����
	int realParticleNum = MaxParticleNum;                 // ���݂̃p�[�e�B�N����
	bool isPlaying = false;                               // �p�[�e�B�N���G�t�F�N�g�Đ���
	DirectX::XMFLOAT3 color = { 1,1,1 };                  // �p�[�e�B�N���̐F
	static DirectX::XMFLOAT3 cameraPosition;
	static DirectX::XMFLOAT4 frustumPlanes[6];
	
public:
	static void Init();
	virtual void Update(float _deltaTime);
	void UpdateParticlesRange(int start, int end, float deltaTime);
	void ComputeParticleUpdate(float _deltaTime);
	void MultiThreadedUpdate(float _deltaTime);
	void Draw();
	static void SetCameraPosition(DirectX::XMFLOAT3 _pos) { cameraPosition = _pos; };
	static void SetFrustumPlanes(const DirectX::XMFLOAT4* planes) { memcpy(frustumPlanes, planes, sizeof(DirectX::XMFLOAT4) * 6); };
	
	// Set�֐�
	void SetMaxLifeTime(float _lifeTime) { maxLifeTime = _lifeTime; }
	void SetMinLifeTime(float _lifeTime) { minLifeTime = _lifeTime; }
	void SetMaxSpeed(float _speed) { maxSpeed = _speed; };
	void SetMinSpeed(float _speed) { minSpeed = _speed; };
	void SetSize(float _size) { size = _size; }
	void SetGravity(float _gravity) { gravity = _gravity; }
	void SetMaxParticleNum(int _num) { maxParticleNum = _num; }
	void SetColor(DirectX::XMFLOAT3 _color) { color = _color; }
	void SetMaxAlpha(float _alpha) { maxAlpha = _alpha; }
	void SetIsPlaying(bool _isPlay) { isPlaying = _isPlay; }
	void SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps) { useVS = _vs, usePS = _ps; }
	void SetUpParticle();
	// Get�֐�
	float GetMaxLifeTime() { return maxLifeTime; }
	float GetMinLifeTime() { return minLifeTime; }
	float GetMaxSpeed(){ return maxSpeed; }
	float GetMinSpeed() { return minSpeed; }
	float GetSize() { return size; }
	float GetGravity()  { return gravity; }
	int GetMaxParticleNum() { return maxParticleNum; }
	float GetMaxAlpha() { return maxAlpha; }
	bool GetIsPlaying() { return isPlaying; }

	
};

//===========================================//
// �A�C�e����p�G�t�F�N�g�N���X
//===========================================//
class ItemEffect : public Particle
{
private:
	int frameCount = 0;
public:
	void Update(float _deltaTime) override;
	//�@�p�[�e�B�N���G�t�F�N�g�Đ�
	void ParticlePlay(DirectX::XMFLOAT3 _center);
	// �G�t�F�N�g��~
	void ParticleStop();

};

//===========================================//
// �ΉԃG�t�F�N�g��p�N���X
//===========================================//
class FireFlowerEffect : public Particle
{
public:
	static constexpr float ParticleNumUpSpeed = 0.2f;     // �p�[�e�B�N���������x
	static constexpr float ParticleNumDownSpeed = 2.0f;   // �p�[�e�B�N���������x
	static constexpr float MinSpawnHeightValue = 3.0f;    // �ŏ��̐����͈�
	static constexpr float MaxSpawnHeightValue = 6.0f;    // �ő�̐����͈�
	static constexpr float MinDirValue = 1.0f;            // �ŏ��̕��o�͈�
	static constexpr float MaxDirValue = 10.0f;            // �ő�̕��o�͈�

private:
	DirectX::XMFLOAT3 particleSpawnPos{};       // �p�[�e�B�N�����S�������W
	DirectX::XMFLOAT3 targetNewPos{};           // ���o�Ώۂ̌��݂̍��W
	DirectX::XMFLOAT3 targetOldPos{};           // ���o�Ώۂ̑O�t���[���̍��W
	bool isCanSpawn = false;                    // ������
	float targetSpeedRatio = 0.0f;              // ���o�Ώۂ̈ړ����x����
	int previousParticleNum = 0;                // �����p�[�e�B�N������⊮����ϐ�
	float upBaseSpeed = 0.2f;                   // �p�[�e�B�N���ʑ������x
	float downBaseSpeed = 2.0f;                 // �p�[�e�B�N���ʌ������x
	
public:
	void Update(float _deltaTime) override;
	// Set�֐�
	void SetParticleSpawnPosition(DirectX::XMFLOAT3 _pos) { particleSpawnPos = _pos; }
	void SetIsCanSpawn(bool _isCan) { isCanSpawn = _isCan; }
	void SetTargetSpeedRatio(float _ratio) { targetSpeedRatio = _ratio; }
	void SetTargetNewPosition(DirectX::XMFLOAT3 _pos) { targetNewPos = _pos; }
	void SetTargetOldPosition(DirectX::XMFLOAT3 _pos) { targetOldPos = _pos; }
	// Get�֐�
	int GetRealParticleNum() { return realParticleNum; }
private:
	// Set�֐�
	void SetSpawnParticle(ParticleData& _spawnData);
};