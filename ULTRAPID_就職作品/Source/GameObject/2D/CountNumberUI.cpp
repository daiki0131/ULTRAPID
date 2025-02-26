#include "CountNumberUI.h"
#include "Geometry.h"

using namespace DirectX;

// �R���X�g���N�^
CountNumberUI::CountNumberUI()
{
	// UI�I�u�W�F�N�g����
	numUI = std::make_unique<Object2D>(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.4f, 0.7f, 1.0f),
		XMFLOAT4(1, 1, 1, 1));

	// �V�F�[�_�[�Z�b�g
	numUI->SetShader(Shader::VS_TEXTURE, Shader::PS_TEXTURE);
	// ���f���Z�b�g
	model = Geometry::CreateSquare({ 0,0 }, { 1,1 }, { 0.1f, 1.0f });
	model.texture = Renderer::LoadTexture(L"Assets/Texture/number.png");
	numUI->SetModel(model);
}

// �`�揈��
void CountNumberUI::Draw()
{
	if (!numUI->GetIsActive()) return; // ��A�N�e�B�u��Ԃ̎�return

	numUI->Draw(); // �`����s
}

// �������Z�b�g
void CountNumberUI::SetNum(int _num)
{
	int num = std::abs(_num) % 10; // 1�̈ʂ��v�Z

	SetUV(num); // UV�v�Z

	numUI->SetUV(uv); // UV�Z�b�g
}