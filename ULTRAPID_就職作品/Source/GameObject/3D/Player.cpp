#include "Player.h"
#include "Vector.h"
#include "DebugUI.h"
#include "GameManager.h"

using namespace DirectX;

// �R���X�g���N�^
Player::Player(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:GameObject(_pos, _rot, _scale, _color)
{   
	// ���[�V�����u���[�̉ߋ����W�̎擾��
	oldPositionsMotionBlur.resize(4);
	// �����l�ݒ�i�v���C���[�̏����l�ɐݒ�j
	OldPositionsMotionBlurUpdate(position, true);
	oldPosition = position;
	startPosition = position;
	defaultScale = scaling.x;

	// �}�e���A���ݒ�
	mat.emission = XMFLOAT4(0.0f, 1.0f, 0.5f, 1.5f);
	mat.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.5f);
	mat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.5f);
	mat.other = XMFLOAT4(0.02f, 0.0f, 0.0f, 0.0f);

	instanceDatas.reserve(MaxMotionBlurImprintNum);
}

// �X�V�����i��ɑ��삵�Ȃ��p�j
void Player::Update(float _deltatime)
{
	// ���x��������ɍő��
	speedRatio = 1.0f;

	// ���W�̋L�^
	oldPosition = position;
	OldPositionsMotionBlurUpdate(position, false);

	// ��]����
	RotationPlayer();

	// �����蔻��̒��S�_���X�V
	boundingSphere.center = position;
	itemBounds.center = position;
}

// �`�揈��
void Player::Draw()
{
	// ���[���h�ϊ��s��v�Z
	matrixWorld = CalculateMatrixWorld(position, scaling, rotation);

	// Renderer�ɑ���f�[�^���i�[
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose(matrixWorld);
	ob.color = color;
	
	// �ʏ�`��
	Renderer::DrawObject(model, ob, mat, useVS, usePS, false, false, true);

}

void Player::DrawShadowMap()
{
	// ���[���h�ϊ��s��v�Z
	matrixWorld = CalculateMatrixWorld(position, scaling, rotation);

	// Renderer�ɑ���f�[�^���i�[
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose(matrixWorld);
	ob.color = color;

	// �ʏ�`��
	Renderer::DrawObject(model, ob, std::nullopt, Shader::VS_SHADOW_MAP, Shader::PS_NULLPTR, false, true, false);
}

// ���[�V�����u���[�̃G�t�F�N�g����
void Player::MortionBlurDraw()
{
	// ���[���h�ϊ��s���ޔ�
	XMMATRIX tempMatrix = matrixWorld;

	// �ߋ��̍��W�����������ԂɂȂ�悤�Ƀ\�[�g����
	std::vector<XMFLOAT3> sortedPositions(oldPositionsMotionBlur.size());

	for (size_t i = 0; i < oldPositionsMotionBlur.size(); i++)
	{
		// �ŐV�̃f�[�^���擪�ɗ���悤�ɃR�s�[
		int index = (oldPositionsCountMotionBlur - 1 - i + static_cast<int>(oldPositionsMotionBlur.size())) % oldPositionsMotionBlur.size();
		sortedPositions[i] = oldPositionsMotionBlur[index];
	}

	instanceDatas.clear();

	// �ߋ��̍��W���m�̊ԂɎc���G�t�F�N�g��u���Ă���
	for (int i = 0; i < motionBlurImprintNum; i++)
	{
		// ���݂̎c���̐i�s�x�i0.0�`1.0�j���v�Z
		float t = static_cast<float>(i / static_cast<float>(motionBlurImprintNum - 1));

		XMFLOAT3 drawPos; // �`�悷��c���̈ʒu

		int segmentCount = static_cast<int>(sortedPositions.size());     // �ێ����Ă���ߋ��̈ʒu�̐�
		float segmentSize = 1.0f / (segmentCount + 1); // �e��Ԃ̃T�C�Y
		// t �̐i�s�x�Ɋ�Â��ēK�؂ȉߋ��̈ʒu��Ԃ��v�Z
		int segmentIndex = std::min(static_cast<int>(t / segmentSize), segmentCount - 1);
		float localT = (t - segmentIndex * segmentSize) / segmentSize;

		// �I��������ԂŐ��`��Ԃ��s���A�c���̈ʒu������
		if (segmentIndex == 0)
		{
			// �ŏ��̋�Ԃ͌��݈ʒu�ƍŏ��̉ߋ��ʒu�̕��
			drawPos = Vec3::LerpXMFLOAT3(position, sortedPositions[0], localT);
		}
		else
		{
			// ���̑��̋�Ԃ͉ߋ���2�̈ʒu����
			drawPos = Vec3::LerpXMFLOAT3(sortedPositions[segmentIndex - 1], sortedPositions[segmentIndex], localT);
		}
		// ���[���h�ϊ��s��v�Z
		matrixWorld = CalculateMatrixWorld(drawPos, scaling, rotation);

		// Renderer�ɑ���f�[�^���i�[
		XMFLOAT4 blurColor = color; // �c���̐F�̓����x���v�Z
		blurColor.w = std::clamp(((motionBlurAlphaValue * speedRatio) - t) * 0.1f, 0.0f, 1.0f);
		instanceDatas.push_back({ XMMatrixTranspose(matrixWorld), blurColor });
		
	}
	// �����c����`�悷�邽�߂̃f�[�^������΁A�C���X�^���V���O�ŕ`�悷��
	if (motionBlurImprintNum > 0)
	{
		// �C���X�^���V���O�Ŏc���G�t�F�N�g��`��
		Renderer::DrawObjectInstancing(model, instanceDatas,mat, useMotionBlurVS, useMotionBlurPS, false, false, true);
	}

	// �ޔ������s������ɖ߂�
	matrixWorld = tempMatrix;
}

// ���W���X�V�iTitleScene�ł̂ݎg�p�j
void Player::AddPosition(DirectX::XMFLOAT3 _addPos)
{
	position.x += _addPos.x;
	position.y += _addPos.y;
	position.z += _addPos.z;

	newPosition = position;
}

// �o�E���f�B���O�X�t�B�A�ݒ�
void Player::SetBoundingSphere(DirectX::XMFLOAT3 _center, float _radius)
{
	boundingSphere.center = _center;
	boundingSphere.radius = _radius;
}

// �A�C�e���Ƃ̔���p�̃o�E���f�B���O�X�t�B�A
void Player::SetItemBoundingSphere(DirectX::XMFLOAT3 _center, float _radius)
{
	itemBounds.center = _center;
	itemBounds.radius = _radius;
}

// ���[�V�����u���[�Ŏg�p����ߋ��̍��W��z��Ɋi�[����֐�
void Player::OldPositionsMotionBlurUpdate(const DirectX::XMFLOAT3 _pos, const bool isAllSet)
{
	// ����i�[����ꍇ
	if (!isAllSet)
	{
		oldPositionsMotionBlur[oldPositionsCountMotionBlur] = _pos;

		oldPositionsCountMotionBlur++;

		// �z��̃T�C�Y�ɖ���������J�E���g��0�ɂ���
		if (oldPositionsCountMotionBlur >= oldPositionsMotionBlur.size())
		{
			oldPositionsCountMotionBlur = 0;
		}
	}
	// �z��̗v�f���ׂĂɊi�[����
	else
	{
		for (auto& oldPos : oldPositionsMotionBlur)
		{
			oldPos = _pos;
		}
		oldPositionsCountMotionBlur = 0;
	}
}

void Player::RotationPlayer()
{
	// ���[���h�ϊ��s�񂩂�K�v�ȏ����擾����
	XMFLOAT3 outScale; // �X�P�[���i�[�p
	XMFLOAT3 outPos;   // ���W�i�[�p
	XMMATRIX matWrold = GetMatrixWorld(); // ���[���h�ϊ��s��擾

	// �s��𕪉����č��W�E��]�E�X�P�[�����擾
	DirectX::XMVECTOR scale, rotationQuat, translation;
	DirectX::XMMatrixDecompose(&scale, &rotationQuat, &translation, matWrold);

	// �擾�����X�P�[���A�ʒu�����ꂼ��i�[
	DirectX::XMStoreFloat3(&outScale, scale);
	DirectX::XMStoreFloat3(&outPos, translation);

	// �ړ������ɉ����ĉ�]�ʂ����߂�
	// �V�����ʒu�ƌ��݂̈ʒu�̃x�N�g�������擾
	Vec3 moveV = Vec3::SetVector(newPosition) - Vec3::SetVector(outPos);
	moveV.y = 0.0f; // Y���̉�]�͖�������i�n�ʂɉ�������]�̂݁j

	// �ړ������̒������v�Z
	float rollDistance = moveV.Length();

	// ���x���o�����Ď~�܂��Č����Ȃ��悤�ɂ��邽��
	// ������x�̋������Ɏ��߂�
	if (rollDistance >= MaxRollDistance * (scaling.x / defaultScale))
	{
		rollDistance = MaxRollDistance;
	}

	// ��]�ʂ̌v�Z
	float rollRotation = rollDistance / (outScale.x * 0.5f);

	// ���K��
	moveV.Normalize();

	// ��]������
	rotation.x += rollRotation;
	// �l�����Ȃ��悤�Ɏ����I�ɒ�������
	rotation.x = std::fmod(rotation.x, GameManager::FullRotationDegress);
}