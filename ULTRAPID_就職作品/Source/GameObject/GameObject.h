#pragma once
#include "Renderer.h"
#include <memory>

//===========================================//
// �Q�[�����̂��ׂĂ̕`��I�u�W�F�N�g���������N���X
//===========================================//
class GameObject
{
protected:
	DirectX::XMFLOAT3 position = {};             // ���W
	DirectX::XMFLOAT3 rotation = {};             // ��]
	DirectX::XMFLOAT3 scaling = { 1, 1, 1 };     // �傫��
	DirectX::XMFLOAT4 color = { 1,1,1,1 };       // �F
	DirectX::XMFLOAT4 firstColor = { 1,1,1,1 };  // �ŏ��̐F
	bool isActive = true;                        // �`���
	DirectX::XMMATRIX matrixWorld;				 // ���[���h�ϊ��s��
	
public:
	GameObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
	virtual ~GameObject() = default;
	virtual void Update(float _deltaTime) {};
	virtual void Draw() {};
	// Set�֐�
	virtual void SetPosition(DirectX::XMFLOAT3 _pos) { position = _pos; };
	void SetRotation(DirectX::XMFLOAT3 _rot) { rotation = _rot; }
	void SetIsActive(bool _active) { isActive = _active; }
	void SetScale(DirectX::XMFLOAT3 _scale) { scaling = _scale; };
	void SetMatrixWorld(DirectX::XMMATRIX _matrix) { matrixWorld = _matrix; }
	void SetRGBColor(DirectX::XMFLOAT3 _rgb);
	void SetFirstColor() { color = firstColor; }
	void SetColorAlpha(float _alpha) { color.w = _alpha; }
	// Get�֐�
	bool& GetIsActive() { return  isActive; }
	DirectX::XMFLOAT3 GetPosition() const { return position; }
	DirectX::XMFLOAT3 GetScale() { return scaling; }
	DirectX::XMMATRIX GetMatrixWorld() { return matrixWorld; }
	DirectX::XMFLOAT3 GetRGBColor();
	DirectX::XMFLOAT4 GetColor() { return color; }
	// ���[���h�ϊ��s����v�Z
	static DirectX::XMMATRIX CalculateMatrixWorld(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT3 _rot);
	
protected:
	// UV�s��擾
	virtual DirectX::XMMATRIX GetUvMatrix();
	
};