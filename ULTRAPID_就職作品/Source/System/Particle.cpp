#include "Particle.h"
#include "SceneManager.h"
#include "Shader.h"
#include "Geometry.h"
#include "GameManager.h"
#include <random>
#include <span>
#include "DebugConsole.h"
#include <thread>
#include <future>

using namespace DirectX;

ComPtr<ID3D11Buffer> Particle::particleBuffer;
ComPtr<ID3D11Buffer> Particle::parameterBuffer;
ComPtr<ID3D11Buffer> Particle::stagingBuffer;
ComPtr<ID3D11UnorderedAccessView> Particle::uavBuffer;
DirectX::XMFLOAT3 Particle::cameraPosition;
DirectX::XMFLOAT4 Particle::frustumPlanes[6];



//=====================================================
//  Particle
//=====================================================

// 初期化処理
void Particle::Init()
{

	HRESULT hr;

	// パーティクルバッファ作成
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Particle::ParticleData) * MaxParticleNum;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = sizeof(Particle::ParticleData);
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, particleBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("particleBuffer 作成失敗：Particle::Init内で発生", DebugConsole::ERROR_LOG);
	}

	// パーティクルパラメータバッファ作成
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.ByteWidth = sizeof(Particle::ParameterBuffer);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	hr = Renderer::GetDevice()->CreateBuffer(&cbDesc, nullptr, parameterBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("parameterBuffer 作成失敗：Particle::Init内で発生", DebugConsole::ERROR_LOG);
	}

	// 読み取り専用バッファ作成
	D3D11_BUFFER_DESC stagingDesc = {};
	stagingDesc.ByteWidth = sizeof(Particle::ParticleData) * MaxParticleNum;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = Renderer::GetDevice()->CreateBuffer(&stagingDesc, nullptr, stagingBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("stagingBuffer 作成失敗：Particle::Init内で発生", DebugConsole::ERROR_LOG);
	}

	// 書き込みを行うためのバッファ作成
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = MaxParticleNum;
	if (particleBuffer != nullptr)
	{
		hr = Renderer::GetDevice()->CreateUnorderedAccessView(particleBuffer.Get(), &uavDesc, uavBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			DebugConsole::DebugLog("particleBuffer 作成失敗：Particle::Init内で発生", DebugConsole::ERROR_LOG);
		}
	}
}

// 更新処理
void Particle::Update(float _deltaTime)
{
	
}

// パーティクル初期化処理
void Particle::SetUpParticle()
{
	data.resize(MaxParticleNum);
	instanceDatas.reserve(MaxParticleNum);
	sendData.reserve(MaxParticleNum);

	// 頂点情報を設定
	Geometry::VertexData vd = Geometry::GetSquareVertexData();
	model.indexBuffer = vd.indexBuffer;
	model.vertexBuffer = vd.vertexBuffer;
	model.numIndex = vd.indexCount;
	model.matrixUV = XMMatrixIdentity();
	model.texture = nullptr;
}

void Particle::UpdateParticlesRange(int start, int end, float deltaTime)
{
	for (int i = start; i < end; i++)
	{
		if (!data[i].isActive) continue;

		// 座標更新
		data[i].position.x += data[i].velocity.x * data[i].speed * deltaTime;
		data[i].position.y += data[i].velocity.y * data[i].speed * deltaTime;
		data[i].position.z += data[i].velocity.z * data[i].speed * deltaTime;

		// 重力適用
		data[i].velocity.y -= gravity * deltaTime;

		// 寿命の更新
		data[i].ageTime += deltaTime;

		// アルファ値更新 (寿命が過ぎた場合の対応含む)
		if (data[i].ageTime >= data[i].lifeTime)
		{
			data[i].isActive = false;
			data[i].isVisible = false;
		}
		else
		{
			float lifeRatio = std::clamp(1.0f - (data[i].ageTime / data[i].lifeTime), 0.0f, 1.0f);
			data[i].color.w = lifeRatio * maxAlpha;
		}
		Vec3 distDir = Vec3::SetVector(data[i].position) - Vec3::SetVector(cameraPosition);
		float distSqrt = distDir.Dot(distDir);

		// フラスタム内チェック
		bool isInsideFrustum = true;
		for (int j = 0; j < 6; j++)
		{
			Vec3 frustumPlanesXYZ = Vec3::SetVector({ frustumPlanes[j].x, frustumPlanes[j].y, frustumPlanes[j].z });

			float dotProduct =
				(data[i].position.x * frustumPlanesXYZ.x +
					data[i].position.y * frustumPlanesXYZ.y +
					data[i].position.z * frustumPlanesXYZ.z)
				+ frustumPlanes[j].w;

			if (dotProduct < 0.0f)
			{
				isInsideFrustum = false;
				break;
			}
		}

		data[i].isVisible = (distSqrt <= MaxViewDistance * MaxViewDistance && isInsideFrustum) ? true : false;
	}
}

void Particle::ComputeParticleUpdate(float _deltaTime)
{
	// パーティクルバッファが無効またはパーティクル数がゼロの場合はreturn
	if (!stagingBuffer || realParticleNum == 0) return;

	if (realParticleNum > data.size())
	{
		DebugConsole::DebugLog("realParticleNumがdataの要素数を上回ってます：Particle::Update内で発生", DebugConsole::ERROR_LOG);
		return;
	}

	ComPtr<ID3D11DeviceContext> context = Renderer::GetContext();

	// パラメータバッファの更新
	ParameterBuffer pb;
	pb.deltaTime = _deltaTime;            // 時間差分
	pb.gravity = gravity;                 // 重力
	pb.maxAlpha = maxAlpha;               // 最大アルファ値
	pb.drawNum = realParticleNum;         // 描画するパーティクル数
	pb.cameraPosition = cameraPosition;
	pb.maxViewDistance = MaxViewDistance;
	memcpy(pb.frustumPlanes, frustumPlanes, sizeof(XMFLOAT4) * 6);

	// バッファ更新
	Renderer::UpdateBuffer(parameterBuffer.Get(), &pb, sizeof(ParameterBuffer));

	size_t dataSize = sizeof(ParticleData) * realParticleNum;

	// 更新範囲を指定
	D3D11_BOX updateBox = {};
	updateBox.left = 0;
	updateBox.right = dataSize;
	updateBox.top = 0;
	updateBox.bottom = 1;
	updateBox.front = 0;
	updateBox.back = 1;

	// GPU にパーティクルデータを転送（realParticleNum の個数だけ）
	context->UpdateSubresource(particleBuffer.Get(), 0, &updateBox, data.data(), 0, 0);

	// コンピュートシェーダーを設定
	context->CSSetConstantBuffers(0, 1, parameterBuffer.GetAddressOf());
	context->CSSetShader(Shader::GetCS(Shader::CS_PARTICLE), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, uavBuffer.GetAddressOf(), nullptr);

	// パーティクルの処理をGPUで行うためにDispatchを呼び出す
	const int threadsPerGroup = MaxThreadDataNum;
	const int groupCount = (realParticleNum + threadsPerGroup - 1) / threadsPerGroup;
	context->Dispatch(groupCount, 1, 1);

	// パーティクルデータをGPUからCPUへコピー（結果を取得）
	context->CopyResource(stagingBuffer.Get(), particleBuffer.Get());

	// ステージングバッファからデータをCPU側にマップして読み取る
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(stagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	// マッピング成功時にパーティクルデータをCPU側のデータ構造にコピー
	if (SUCCEEDED(hr))
	{
		auto* rawData = static_cast<const ParticleData*>(mappedResource.pData);
		std::memcpy(data.data(), rawData, sizeof(ParticleData) * realParticleNum);
		context->Unmap(stagingBuffer.Get(), 0);
	}

	// UAV を解除
	ComPtr<ID3D11UnorderedAccessView> nullUAV = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, nullUAV.GetAddressOf(), nullptr);
}

void Particle::MultiThreadedUpdate(float _deltaTime)
{
	int threadCount = std::thread::hardware_concurrency();

	std::vector<std::future<void>> futures;

	int particlesPerThread = realParticleNum / threadCount;

	for (int i = 0; i < threadCount; i++) {
		int start = i * particlesPerThread;
		int end = (i == threadCount - 1) ? realParticleNum : start + particlesPerThread;

		futures.emplace_back(std::async(std::launch::async, &Particle::UpdateParticlesRange, this, start, end, _deltaTime));
	}

	for (auto& future : futures)
	{
		future.get();
	}
}

// 描画処理
void Particle::Draw()
{
	// 再生中でない場合return
	if (!isPlaying) return;

	if (realParticleNum > data.size())
	{
		DebugConsole::DebugLog("realParticleNumがdataの要素数を上回ってます：Particle::Draw内で発生", DebugConsole::WARNING_LOG);
	}

	// スケーリング行列を作成
	XMMATRIX matrixScale = XMMatrixScaling(size, size, size);

	// ビュー行列の取得
	XMMATRIX invView = Renderer::GetViewMatrix();

	// ビュー行列の位置部分をゼロに設定（カメラ位置を無視）
	invView.r[3].m128_f32[0] = 0.0f;
	invView.r[3].m128_f32[1] = 0.0f;
	invView.r[3].m128_f32[2] = 0.0f;

	// ビュー行列の逆行列を計算（カメラの視点を反転）
	invView = XMMatrixInverse(nullptr, invView);

	// 回転行列（ビュー行列の逆行列）
	XMMATRIX matrixRotate = invView;

	instanceDatas.clear();

	// 各パーティクルのワールド行列を計算
	for (int i = 0; i < realParticleNum; i++)
	{
		if (!data[i].isVisible) continue;

		// パーティクルの位置に基づいた移動行列
		XMMATRIX matrixMove = XMMatrixTranslation(data[i].position.x, data[i].position.y, data[i].position.z);

		// ワールド変換行列計算
		XMMATRIX matrixWorld = matrixScale * matrixRotate * matrixMove;

		instanceDatas.push_back({ XMMatrixTranspose(matrixWorld) ,data[i].color });
	}

	if (instanceDatas.size() == 0) return;

	// インスタンシング描画
	Renderer::DrawObjectInstancing(model, instanceDatas, std::nullopt, useVS, usePS, true, false, false);
}

//=====================================================
//  ItemEffect
//=====================================================

// 更新処理
void ItemEffect::Update(float _deltaTime)
{
	// 再生中でない場合return
	if (!isPlaying) return;

	//// コンピュートシェーダーでパーティクル更新
	//ComputeParticleUpdate(_deltaTime);

	// CPUマルチスレッドでパーティクル更新
	MultiThreadedUpdate(_deltaTime);

	int activeCount = 0;
	// アクティブ状態をカウント
	for (int i = 0; i < maxParticleNum; i++)
	{
		if (data[i].isActive) activeCount++;
	}

	// アクティブ数が0の場合パーティクルを停止
	if (activeCount == 0)
	{
		isPlaying = false;
	}
}

// パーティクル再生処理
void ItemEffect::ParticlePlay(DirectX::XMFLOAT3 _center)
{
	isPlaying = true;
	realParticleNum = maxParticleNum;
	// パーティクル寿命
	std::uniform_real_distribution<float> rLifeTime(minLifeTime, maxLifeTime);
	// パーティクル速度
	std::uniform_real_distribution<float> rSpeed(minSpeed, maxSpeed);
	// 発射方向
	std::uniform_real_distribution<float> rDir(-1.0f, 1.0f);

	// 各粒子の初期化を実行
	for (int i = 0; i < realParticleNum; i++)
	{
		data[i].position = _center;
		data[i].isActive = true;
		data[i].ageTime = 0.0f;
		data[i].lifeTime = rLifeTime(GameManager::rd);
		data[i].speed = rSpeed(GameManager::rd);
		float x = rDir(GameManager::rd);
		float y = rDir(GameManager::rd);
		float z = rDir(GameManager::rd);
		float length = sqrtf(x * x + y * y + z * z);
		data[i].velocity.x = x / length;
		data[i].velocity.y = y / length;
		data[i].velocity.z = z / length;
		data[i].color = XMFLOAT4(color.x, color.y, color.z, maxAlpha);
	}
}

// パーティクル停止処理
void ItemEffect::ParticleStop()
{
	for (auto& _data : data)
	{
		_data.isActive = false;
	}
}

//=====================================================
//  FireFlowerEffect
//=====================================================

// 更新処理
void FireFlowerEffect::Update(float _deltaTime)
{ 
	// パーティクル数が減少している場合はdownBaseSpeedを使用し、増加している場合はupBaseSpeedを使用
	float baseSpeed = (realParticleNum < previousParticleNum) ? downBaseSpeed : upBaseSpeed;

	// 最小値と最大値の設定（パーティクル数の範囲）
	int minValue = 0;
	int maxValue = maxParticleNum;

	// 現在のパーティクル数を保存
	previousParticleNum = realParticleNum;

	// 現在のターゲット速度に基づいて、パーティクル数の増加量を計算
	int value = minValue + (maxValue - minValue) * targetSpeedRatio;

	// スピードに基づいて、現在のパーティクル数を補間
	long double interpNum = baseSpeed * _deltaTime;
	realParticleNum = realParticleNum + (value - realParticleNum) * interpNum;

	// 速度割合が0.1以下の場合、パーティクル数を0に設定
	if (targetSpeedRatio <= 0.1f) realParticleNum = 0;
	
	// アクティブ状態でないパーティクルを生成
	for (int i = 0; i < realParticleNum; i++)
	{
		if (!data[i].isActive && isCanSpawn)
		{
			SetSpawnParticle(data[i]);
		}
	}

	// 描画しないパーティクルを非アクティブ状態にする
	for (int i = realParticleNum; i < maxParticleNum; i++)
	{
		data[i].isActive = false;
	}

	//// コンピュートシェーダーでパーティクル更新
	ComputeParticleUpdate(_deltaTime);

	// CPUマルチスレッドでパーティクル更新
	//MultiThreadedUpdate(_deltaTime);
}

// パーティクルの生成
void FireFlowerEffect::SetSpawnParticle(ParticleData& _spawnData)
{
	// パーティクルの角度
	std::uniform_real_distribution<float> angleDist(-DirectX::XM_PI * 1 / 3, DirectX::XM_PI * 1 / 3);
	// パーティクルの高さ
	std::uniform_real_distribution<float> rHeight(MinSpawnHeightValue * targetSpeedRatio, MaxSpawnHeightValue * targetSpeedRatio);
	// パーティクルの位置
	std::uniform_real_distribution<float> rPos(0.0f, 1.0f);
	// パーティクルの寿命
	std::uniform_real_distribution<float> rLifeTime(minLifeTime * (1.1f - targetSpeedRatio), maxLifeTime * (1.1f - targetSpeedRatio));
	// パーティクルの速度
	std::uniform_real_distribution<float> rSpeed(minSpeed, maxSpeed);
	// パーティクルの方向
	std::uniform_real_distribution<float> rDir(MinDirValue * targetSpeedRatio, MaxDirValue * targetSpeedRatio);

	// ランダムな角度を生成
	float randomAngle = angleDist(GameManager::mt);

	// 角度に基づいて、cosとsinを計算
	float cosTheta = cos(randomAngle);
	float sinTheta = sin(randomAngle);

	// パーティクルの移動に関する補間値
	float t = rPos(GameManager::mt);

	// 新旧のターゲット位置を使って発射方向を計算
	Vec3 shotDir = Vec3::SetVector(targetNewPos) - Vec3::SetVector(targetOldPos);
	shotDir = shotDir.Normalize();
	shotDir *= -1;

	// 各種パラメータ設定
	_spawnData.position.x = targetOldPos.x + t * (targetNewPos.x - targetOldPos.x);
	_spawnData.position.y = particleSpawnPos.y;
	_spawnData.position.z = targetOldPos.z + t * (targetNewPos.z - targetOldPos.z);
	_spawnData.isActive = true;
	_spawnData.ageTime = 0.0f;
	_spawnData.lifeTime = rLifeTime(GameManager::rd);
	_spawnData.speed = rSpeed(GameManager::rd);
	XMFLOAT3 originalVelocity = Vec3::SetXMFLOAT3(shotDir);
	float randomDirectionScale = rDir(GameManager::mt);
	_spawnData.velocity.x = (originalVelocity.x * cosTheta - originalVelocity.z * sinTheta) * randomDirectionScale;
	_spawnData.velocity.y = originalVelocity.y + rHeight(GameManager::mt);
	_spawnData.velocity.z = (originalVelocity.x * sinTheta + originalVelocity.z * cosTheta) * randomDirectionScale;
	_spawnData.color = XMFLOAT4(color.x, color.y, color.z, maxAlpha * targetSpeedRatio);
}