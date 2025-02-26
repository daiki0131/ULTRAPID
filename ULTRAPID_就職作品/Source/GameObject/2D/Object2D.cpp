#include "Object2D.h"

using namespace DirectX;

// �R���X�g���N�^
Object2D::Object2D(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color)
	:GameObject(_pos, _rot, _scale, _color)
{
}

// �`�揈��
void Object2D::Draw()
{
    if (!isActive) return; // ��A�N�e�B�u��return

    Renderer::ObjectBuffer ob;
    ob.matrixWorld = XMMatrixTranspose(matrixWorld); // ���[���h�ϊ��s��Z�b�g
    ob.color = color; // �J���[���Z�b�g
    model.matrixUV = XMMatrixTranspose(GetUvMatrix()); // UV�ϊ��s����Z�b�g

    Renderer::DrawObject(model, ob, std::nullopt, useVS, usePS, true, false, false); // �`��
}

// �I�u�W�F�N�g���ړ�����ۂ̕`�揈��
// ���[���h�ϊ��s����Čv�Z����
void Object2D::MoveDraw()
{
    ReCalculateWorldMatrix();
    this->Draw();
}

// UV�ϊ��s���Ԃ��֐�
DirectX::XMMATRIX Object2D::GetUvMatrix()
{
    return XMMatrixTranslation(uv.x, uv.y, 0.0f);
}

// �V�F�[�_�[�Z�b�g
void Object2D::SetShader(Shader::VS_NAME _vs, Shader::PS_NAME _ps)
{
    useVS = _vs;
    usePS = _ps;
}

// ���[���h�ϊ��s��Čv�Z����֐�
// �i��ɃI�u�W�F�N�g���Œ�ł͂Ȃ��Ƃ��j
void Object2D::ReCalculateWorldMatrix()
{
    matrixWorld = CalculateMatrixWorld(position, scaling, rotation);
}