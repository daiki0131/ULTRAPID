#pragma once
#include "Renderer.h"
#include "Vector.h"
#include "Collision.h"

//===========================================//
// パーティクルクラス
//===========================================//
class Particle
{
public:
	static constexpr int MaxThreadDataNum = 256;    // CPU1スレッド当たりの計算量
	static constexpr float MaxLifeTime = 3.0f;      // 最大生存時間
	static constexpr float MinLifeTime = 2.0f;      // 最小生存時間
	static constexpr float MaxSpeed = 30.0f;        // 最大速度
	static constexpr float MinSpeed = 20.0f;        // 最小速度
	static constexpr float MaxSize = 0.5f;          // 最大サイズ
	static constexpr int MaxParticleNum = 10000;     // 最大のパーティクル数
	static constexpr float MaxGravity = 9.8f;       // 最大の重力値
	static constexpr float MaxAlpha = 1.0f;         // 最大のα値
	static constexpr float MaxViewDistance = 100.0f; // パーティクルを可視化できる距離

	// パーティクルデータ
	struct ParticleData
	{
		DirectX::XMFLOAT3 position;       // 座標
		DirectX::XMFLOAT4 color;          // 色
		DirectX::XMFLOAT3 velocity;       // 向き
		float speed;                      // 速度
		float lifeTime;                   // 生存時間
		float ageTime;                    // 経過時間
		unsigned int isActive;            // 生存可否
		unsigned int isVisible;
		
	};
	// パーティクル共通バッファ
	struct ParameterBuffer
	{
		float deltaTime;                  // フレーム時間
		float gravity;                    // 重力
		float maxAlpha;                   // α値
		unsigned int drawNum;             // 描画数
		DirectX::XMFLOAT3 cameraPosition;
		float maxViewDistance;
		DirectX::XMFLOAT4 frustumPlanes[6];
	};
protected:
	static ComPtr<ID3D11Buffer> particleBuffer;           // パーティクルデータのバッファ
	static ComPtr<ID3D11Buffer> parameterBuffer;          // 共通部分のバッファ
	static ComPtr<ID3D11Buffer> stagingBuffer;            // GPU から CPU へデータを読み込むための中間バッファ
	static ComPtr<ID3D11UnorderedAccessView> uavBuffer;   // ピクセルシェーダーやコンピュートシェーダーからランダムに読み書きできるバッファ

	Renderer::Model model;
	std::vector<Renderer::ObjectBuffer> instanceDatas;                 // Renderer送信用オブジェクトデータ
	Shader::VS_NAME useVS = Shader::VS_TEXTURE;            // 使用する頂点シェーダー
	Shader::PS_NAME usePS = Shader::PS_TEXTURE;            // 使用するピクセルシェーダー
	std::vector<Particle::ParticleData> data;             // パーティクルデータ
	std::vector<Particle::ParticleData> sendData;
	float maxLifeTime = MaxLifeTime;                      // 最大生存時間
	float minLifeTime = MinLifeTime;                      // 最小生存時間
	float maxSpeed = MaxSpeed;                            // 最大速度
	float minSpeed = MinSpeed;                            // 最小速度
	float size = MaxSize;                                 // 最大サイズ
	float maxAlpha = 0.5f;                                // 最大α値
	float gravity = MaxGravity;                           // 最大重力
	int maxParticleNum = MaxParticleNum;                  // 最大パーティクル数
	int realParticleNum = MaxParticleNum;                 // 現在のパーティクル数
	bool isPlaying = false;                               // パーティクルエフェクト再生可否
	DirectX::XMFLOAT3 color = { 1,1,1 };                  // パーティクルの色
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
	
	// Set関数
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
	// Get関数
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
// アイテム専用エフェクトクラス
//===========================================//
class ItemEffect : public Particle
{
private:
	int frameCount = 0;
public:
	void Update(float _deltaTime) override;
	//　パーティクルエフェクト再生
	void ParticlePlay(DirectX::XMFLOAT3 _center);
	// エフェクト停止
	void ParticleStop();

};

//===========================================//
// 火花エフェクト専用クラス
//===========================================//
class FireFlowerEffect : public Particle
{
public:
	static constexpr float ParticleNumUpSpeed = 0.2f;     // パーティクル増加速度
	static constexpr float ParticleNumDownSpeed = 2.0f;   // パーティクル減少速度
	static constexpr float MinSpawnHeightValue = 3.0f;    // 最小の生成範囲
	static constexpr float MaxSpawnHeightValue = 6.0f;    // 最大の生成範囲
	static constexpr float MinDirValue = 1.0f;            // 最小の放出範囲
	static constexpr float MaxDirValue = 10.0f;            // 最大の放出範囲

private:
	DirectX::XMFLOAT3 particleSpawnPos{};       // パーティクル中心生成座標
	DirectX::XMFLOAT3 targetNewPos{};           // 放出対象の現在の座標
	DirectX::XMFLOAT3 targetOldPos{};           // 放出対象の前フレームの座標
	bool isCanSpawn = false;                    // 生成可否
	float targetSpeedRatio = 0.0f;              // 放出対象の移動速度割合
	int previousParticleNum = 0;                // 生成パーティクル数を補完する変数
	float upBaseSpeed = 0.2f;                   // パーティクル量増加速度
	float downBaseSpeed = 2.0f;                 // パーティクル量減少速度
	
public:
	void Update(float _deltaTime) override;
	// Set関数
	void SetParticleSpawnPosition(DirectX::XMFLOAT3 _pos) { particleSpawnPos = _pos; }
	void SetIsCanSpawn(bool _isCan) { isCanSpawn = _isCan; }
	void SetTargetSpeedRatio(float _ratio) { targetSpeedRatio = _ratio; }
	void SetTargetNewPosition(DirectX::XMFLOAT3 _pos) { targetNewPos = _pos; }
	void SetTargetOldPosition(DirectX::XMFLOAT3 _pos) { targetOldPos = _pos; }
	// Get関数
	int GetRealParticleNum() { return realParticleNum; }
private:
	// Set関数
	void SetSpawnParticle(ParticleData& _spawnData);
};