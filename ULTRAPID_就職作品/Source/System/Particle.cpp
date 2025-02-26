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

// ����������
void Particle::Init()
{

	HRESULT hr;

	// �p�[�e�B�N���o�b�t�@�쐬
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Particle::ParticleData) * MaxParticleNum;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = sizeof(Particle::ParticleData);
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	hr = Renderer::GetDevice()->CreateBuffer(&bufferDesc, nullptr, particleBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("particleBuffer �쐬���s�FParticle::Init���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// �p�[�e�B�N���p�����[�^�o�b�t�@�쐬
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
		DebugConsole::DebugLog("parameterBuffer �쐬���s�FParticle::Init���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// �ǂݎ���p�o�b�t�@�쐬
	D3D11_BUFFER_DESC stagingDesc = {};
	stagingDesc.ByteWidth = sizeof(Particle::ParticleData) * MaxParticleNum;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	hr = Renderer::GetDevice()->CreateBuffer(&stagingDesc, nullptr, stagingBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		DebugConsole::DebugLog("stagingBuffer �쐬���s�FParticle::Init���Ŕ���", DebugConsole::ERROR_LOG);
	}

	// �������݂��s�����߂̃o�b�t�@�쐬
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = MaxParticleNum;
	if (particleBuffer != nullptr)
	{
		hr = Renderer::GetDevice()->CreateUnorderedAccessView(particleBuffer.Get(), &uavDesc, uavBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			DebugConsole::DebugLog("particleBuffer �쐬���s�FParticle::Init���Ŕ���", DebugConsole::ERROR_LOG);
		}
	}
}

// �X�V����
void Particle::Update(float _deltaTime)
{
	
}

// �p�[�e�B�N������������
void Particle::SetUpParticle()
{
	data.resize(MaxParticleNum);
	instanceDatas.reserve(MaxParticleNum);
	sendData.reserve(MaxParticleNum);

	// ���_����ݒ�
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

		// ���W�X�V
		data[i].position.x += data[i].velocity.x * data[i].speed * deltaTime;
		data[i].position.y += data[i].velocity.y * data[i].speed * deltaTime;
		data[i].position.z += data[i].velocity.z * data[i].speed * deltaTime;

		// �d�͓K�p
		data[i].velocity.y -= gravity * deltaTime;

		// �����̍X�V
		data[i].ageTime += deltaTime;

		// �A���t�@�l�X�V (�������߂����ꍇ�̑Ή��܂�)
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

		// �t���X�^�����`�F�b�N
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
	// �p�[�e�B�N���o�b�t�@�������܂��̓p�[�e�B�N�������[���̏ꍇ��return
	if (!stagingBuffer || realParticleNum == 0) return;

	if (realParticleNum > data.size())
	{
		DebugConsole::DebugLog("realParticleNum��data�̗v�f���������Ă܂��FParticle::Update���Ŕ���", DebugConsole::ERROR_LOG);
		return;
	}

	ComPtr<ID3D11DeviceContext> context = Renderer::GetContext();

	// �p�����[�^�o�b�t�@�̍X�V
	ParameterBuffer pb;
	pb.deltaTime = _deltaTime;            // ���ԍ���
	pb.gravity = gravity;                 // �d��
	pb.maxAlpha = maxAlpha;               // �ő�A���t�@�l
	pb.drawNum = realParticleNum;         // �`�悷��p�[�e�B�N����
	pb.cameraPosition = cameraPosition;
	pb.maxViewDistance = MaxViewDistance;
	memcpy(pb.frustumPlanes, frustumPlanes, sizeof(XMFLOAT4) * 6);

	// �o�b�t�@�X�V
	Renderer::UpdateBuffer(parameterBuffer.Get(), &pb, sizeof(ParameterBuffer));

	size_t dataSize = sizeof(ParticleData) * realParticleNum;

	// �X�V�͈͂��w��
	D3D11_BOX updateBox = {};
	updateBox.left = 0;
	updateBox.right = dataSize;
	updateBox.top = 0;
	updateBox.bottom = 1;
	updateBox.front = 0;
	updateBox.back = 1;

	// GPU �Ƀp�[�e�B�N���f�[�^��]���irealParticleNum �̌������j
	context->UpdateSubresource(particleBuffer.Get(), 0, &updateBox, data.data(), 0, 0);

	// �R���s���[�g�V�F�[�_�[��ݒ�
	context->CSSetConstantBuffers(0, 1, parameterBuffer.GetAddressOf());
	context->CSSetShader(Shader::GetCS(Shader::CS_PARTICLE), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, uavBuffer.GetAddressOf(), nullptr);

	// �p�[�e�B�N���̏�����GPU�ōs�����߂�Dispatch���Ăяo��
	const int threadsPerGroup = MaxThreadDataNum;
	const int groupCount = (realParticleNum + threadsPerGroup - 1) / threadsPerGroup;
	context->Dispatch(groupCount, 1, 1);

	// �p�[�e�B�N���f�[�^��GPU����CPU�փR�s�[�i���ʂ��擾�j
	context->CopyResource(stagingBuffer.Get(), particleBuffer.Get());

	// �X�e�[�W���O�o�b�t�@����f�[�^��CPU���Ƀ}�b�v���ēǂݎ��
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(stagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

	// �}�b�s���O�������Ƀp�[�e�B�N���f�[�^��CPU���̃f�[�^�\���ɃR�s�[
	if (SUCCEEDED(hr))
	{
		auto* rawData = static_cast<const ParticleData*>(mappedResource.pData);
		std::memcpy(data.data(), rawData, sizeof(ParticleData) * realParticleNum);
		context->Unmap(stagingBuffer.Get(), 0);
	}

	// UAV ������
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

// �`�揈��
void Particle::Draw()
{
	// �Đ����łȂ��ꍇreturn
	if (!isPlaying) return;

	if (realParticleNum > data.size())
	{
		DebugConsole::DebugLog("realParticleNum��data�̗v�f���������Ă܂��FParticle::Draw���Ŕ���", DebugConsole::WARNING_LOG);
	}

	// �X�P�[�����O�s����쐬
	XMMATRIX matrixScale = XMMatrixScaling(size, size, size);

	// �r���[�s��̎擾
	XMMATRIX invView = Renderer::GetViewMatrix();

	// �r���[�s��̈ʒu�������[���ɐݒ�i�J�����ʒu�𖳎��j
	invView.r[3].m128_f32[0] = 0.0f;
	invView.r[3].m128_f32[1] = 0.0f;
	invView.r[3].m128_f32[2] = 0.0f;

	// �r���[�s��̋t�s����v�Z�i�J�����̎��_�𔽓]�j
	invView = XMMatrixInverse(nullptr, invView);

	// ��]�s��i�r���[�s��̋t�s��j
	XMMATRIX matrixRotate = invView;

	instanceDatas.clear();

	// �e�p�[�e�B�N���̃��[���h�s����v�Z
	for (int i = 0; i < realParticleNum; i++)
	{
		if (!data[i].isVisible) continue;

		// �p�[�e�B�N���̈ʒu�Ɋ�Â����ړ��s��
		XMMATRIX matrixMove = XMMatrixTranslation(data[i].position.x, data[i].position.y, data[i].position.z);

		// ���[���h�ϊ��s��v�Z
		XMMATRIX matrixWorld = matrixScale * matrixRotate * matrixMove;

		instanceDatas.push_back({ XMMatrixTranspose(matrixWorld) ,data[i].color });
	}

	if (instanceDatas.size() == 0) return;

	// �C���X�^���V���O�`��
	Renderer::DrawObjectInstancing(model, instanceDatas, std::nullopt, useVS, usePS, true, false, false);
}

//=====================================================
//  ItemEffect
//=====================================================

// �X�V����
void ItemEffect::Update(float _deltaTime)
{
	// �Đ����łȂ��ꍇreturn
	if (!isPlaying) return;

	//// �R���s���[�g�V�F�[�_�[�Ńp�[�e�B�N���X�V
	//ComputeParticleUpdate(_deltaTime);

	// CPU�}���`�X���b�h�Ńp�[�e�B�N���X�V
	MultiThreadedUpdate(_deltaTime);

	int activeCount = 0;
	// �A�N�e�B�u��Ԃ��J�E���g
	for (int i = 0; i < maxParticleNum; i++)
	{
		if (data[i].isActive) activeCount++;
	}

	// �A�N�e�B�u����0�̏ꍇ�p�[�e�B�N�����~
	if (activeCount == 0)
	{
		isPlaying = false;
	}
}

// �p�[�e�B�N���Đ�����
void ItemEffect::ParticlePlay(DirectX::XMFLOAT3 _center)
{
	isPlaying = true;
	realParticleNum = maxParticleNum;
	// �p�[�e�B�N������
	std::uniform_real_distribution<float> rLifeTime(minLifeTime, maxLifeTime);
	// �p�[�e�B�N�����x
	std::uniform_real_distribution<float> rSpeed(minSpeed, maxSpeed);
	// ���˕���
	std::uniform_real_distribution<float> rDir(-1.0f, 1.0f);

	// �e���q�̏����������s
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

// �p�[�e�B�N����~����
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

// �X�V����
void FireFlowerEffect::Update(float _deltaTime)
{ 
	// �p�[�e�B�N�������������Ă���ꍇ��downBaseSpeed���g�p���A�������Ă���ꍇ��upBaseSpeed���g�p
	float baseSpeed = (realParticleNum < previousParticleNum) ? downBaseSpeed : upBaseSpeed;

	// �ŏ��l�ƍő�l�̐ݒ�i�p�[�e�B�N�����͈̔́j
	int minValue = 0;
	int maxValue = maxParticleNum;

	// ���݂̃p�[�e�B�N������ۑ�
	previousParticleNum = realParticleNum;

	// ���݂̃^�[�Q�b�g���x�Ɋ�Â��āA�p�[�e�B�N�����̑����ʂ��v�Z
	int value = minValue + (maxValue - minValue) * targetSpeedRatio;

	// �X�s�[�h�Ɋ�Â��āA���݂̃p�[�e�B�N��������
	long double interpNum = baseSpeed * _deltaTime;
	realParticleNum = realParticleNum + (value - realParticleNum) * interpNum;

	// ���x������0.1�ȉ��̏ꍇ�A�p�[�e�B�N������0�ɐݒ�
	if (targetSpeedRatio <= 0.1f) realParticleNum = 0;
	
	// �A�N�e�B�u��ԂłȂ��p�[�e�B�N���𐶐�
	for (int i = 0; i < realParticleNum; i++)
	{
		if (!data[i].isActive && isCanSpawn)
		{
			SetSpawnParticle(data[i]);
		}
	}

	// �`�悵�Ȃ��p�[�e�B�N�����A�N�e�B�u��Ԃɂ���
	for (int i = realParticleNum; i < maxParticleNum; i++)
	{
		data[i].isActive = false;
	}

	//// �R���s���[�g�V�F�[�_�[�Ńp�[�e�B�N���X�V
	ComputeParticleUpdate(_deltaTime);

	// CPU�}���`�X���b�h�Ńp�[�e�B�N���X�V
	//MultiThreadedUpdate(_deltaTime);
}

// �p�[�e�B�N���̐���
void FireFlowerEffect::SetSpawnParticle(ParticleData& _spawnData)
{
	// �p�[�e�B�N���̊p�x
	std::uniform_real_distribution<float> angleDist(-DirectX::XM_PI * 1 / 3, DirectX::XM_PI * 1 / 3);
	// �p�[�e�B�N���̍���
	std::uniform_real_distribution<float> rHeight(MinSpawnHeightValue * targetSpeedRatio, MaxSpawnHeightValue * targetSpeedRatio);
	// �p�[�e�B�N���̈ʒu
	std::uniform_real_distribution<float> rPos(0.0f, 1.0f);
	// �p�[�e�B�N���̎���
	std::uniform_real_distribution<float> rLifeTime(minLifeTime * (1.1f - targetSpeedRatio), maxLifeTime * (1.1f - targetSpeedRatio));
	// �p�[�e�B�N���̑��x
	std::uniform_real_distribution<float> rSpeed(minSpeed, maxSpeed);
	// �p�[�e�B�N���̕���
	std::uniform_real_distribution<float> rDir(MinDirValue * targetSpeedRatio, MaxDirValue * targetSpeedRatio);

	// �����_���Ȋp�x�𐶐�
	float randomAngle = angleDist(GameManager::mt);

	// �p�x�Ɋ�Â��āAcos��sin���v�Z
	float cosTheta = cos(randomAngle);
	float sinTheta = sin(randomAngle);

	// �p�[�e�B�N���̈ړ��Ɋւ����Ԓl
	float t = rPos(GameManager::mt);

	// �V���̃^�[�Q�b�g�ʒu���g���Ĕ��˕������v�Z
	Vec3 shotDir = Vec3::SetVector(targetNewPos) - Vec3::SetVector(targetOldPos);
	shotDir = shotDir.Normalize();
	shotDir *= -1;

	// �e��p�����[�^�ݒ�
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