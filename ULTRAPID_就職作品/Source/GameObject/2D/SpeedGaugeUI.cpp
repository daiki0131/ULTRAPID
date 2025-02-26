#include "SpeedGaugeUI.h"

using namespace DirectX;

// �R���X�g���N�^
SpeedGaugeUI::SpeedGaugeUI(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	: Object2D(_pos, _rot, _scale, _color)
{
}

// �`�揈��
void SpeedGaugeUI::Draw()
{
	if (!isActive) return; // ��A�N�e�B�u��Ԃ̏ꍇreturn

	// �I�u�W�F�N�g�f�[�^�iColor�͖�������j
	Renderer::ObjectBuffer ob;
	ob.matrixWorld = XMMatrixTranspose(matrixWorld); // ���[���h�ϊ��s��Z�b�g

	// UV�ϊ��s��ݒ�
	model.matrixUV = XMMatrixTranspose(GetUvMatrix());

	// ���x�����ݒ�i���̑��̓��e�͏��������ɐݒ�ρj
	gcb.speedRatio = ratio;

	// �`��
	Renderer::DrawGaugeObject(model, ob, gcb, useVS, usePS, true);
}

// �e��������ݒ�
void SpeedGaugeUI::SetSpeedRatioColorPhase(float phase1, float phase2, float phase3, float phase4)
{
	changeColorRatioPhase1 = phase1;
	changeColorRatioPhase2 = phase2;
	changeColorRatioPhase3 = phase3;
	changeColorRatioPhase4 = phase4;
}

// �e�����̐F��ݒ�
void SpeedGaugeUI::SetSpeedRatioColor(
	DirectX::XMFLOAT3 colorPhase1, 
	DirectX::XMFLOAT3 colorPhase2, 
	DirectX::XMFLOAT3 colorPhase3, 
	DirectX::XMFLOAT3 colorPhase4)
{
	speedRatioColor1 = colorPhase1;
	speedRatioColor2 = colorPhase2;
	speedRatioColor3 = colorPhase3;
	speedRatioColor4 = colorPhase4;
}

// �e�������o�b�t�@�f�[�^�Ɋi�[
// ��SetSpeedRatioColorPhase��SetSpeedRatioColor��ݒ肵����ɌĂяo��
void SpeedGaugeUI::InitGaugeColor()
{
	gcb.color[0] = XMFLOAT4(speedRatioColor1.x, speedRatioColor1.y, speedRatioColor1.z, 1.0f);
	gcb.color[1] = XMFLOAT4(speedRatioColor2.x, speedRatioColor2.y, speedRatioColor2.z, 1.0f);
	gcb.color[2] = XMFLOAT4(speedRatioColor3.x, speedRatioColor3.y, speedRatioColor3.z, 1.0f);
	gcb.color[3] = XMFLOAT4(speedRatioColor4.x, speedRatioColor4.y, speedRatioColor4.z, 1.0f);

	gcb.thresholds = XMFLOAT4(changeColorRatioPhase1, changeColorRatioPhase2, changeColorRatioPhase3, changeColorRatioPhase4);

	gcb.lowAlphaValue = Object2D::UiLowAlpha;
}