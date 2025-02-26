#include "NumberTexture2D.h"

using namespace DirectX;

// �`�悷�鐔����UV���v�Z
void NumberTexture2D::SetUV(const int _num)
{
	// number.png��10����
	// 0�`9
	switch (_num)
	{
	case 0: // ����0
		uv.x = 0.0f;
		break;
	case 1: // ����1
		uv.x = 0.1f;
		break;
	case 2: // ����2
		uv.x = 0.2f;
		break;
	case 3: // ����3
		uv.x = 0.3f;
		break;
	case 4: // ����4
		uv.x = 0.4f;
		break;
	case 5: // ����5
		uv.x = 0.5f;
		break;
	case 6: // ����6
		uv.x = 0.6f;
		break;
	case 7: // ����7
		uv.x = 0.7f;
		break;
	case 8: // ����8
		uv.x = 0.8f;
		break;
	case 9: // ����9
		uv.x = 0.9f;
		break;
	case 10:// �����ȊO�i�h�b�g�Ȃǁj
		uv.x = 1.0f;
		break;
	default:
		break;
	}
}

// �����̐����̈ʒu�E�X�P�[�����ꊇ���삷��֐�
void NumberTexture2D::SetWorldMatrix(std::vector<std::unique_ptr<Object2D>>& _objects)
{
	// �e�I�u�W�F�N�g�̕ϊ��s��
	XMMATRIX parentTransform =
		XMMatrixScaling(parentScale.x, parentScale.y, 1.0f) *
		XMMatrixRotationZ(0) *
		XMMatrixTranslation(parentPosition.x, parentPosition.y, parentPosition.z);

	for (int i = 0; i < _objects.size(); i++)
	{
		XMFLOAT3 pos = _objects[i]->GetPosition();
		XMFLOAT3 scale = _objects[i]->GetScale();

		// �e�I�u�W�F�N�g�̃��[�J���ϊ��s����쐬
		XMMATRIX localTransform =
			XMMatrixScaling(scale.x, scale.y, 1.0f) *
			XMMatrixRotationZ(0) *
			XMMatrixTranslation(pos.x, pos.y, pos.z);

		// ���[�J�����W��e�̍��W�n�ɕϊ�����
		XMMATRIX finalTransform = localTransform * parentTransform;

		// �e�I�u�W�F�N�g�̃��[���h�s���ݒ�
		_objects[i]->SetMatrixWorld(finalTransform);
	}
}