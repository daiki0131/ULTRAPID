#pragma once
#include "Renderer.h"
#include <memory>

//===========================================//
// ゲーム内のすべての描画オブジェクトを扱う基底クラス
//===========================================//
class GameObject
{
protected:
	DirectX::XMFLOAT3 position = {};             // 座標
	DirectX::XMFLOAT3 rotation = {};             // 回転
	DirectX::XMFLOAT3 scaling = { 1, 1, 1 };     // 大きさ
	DirectX::XMFLOAT4 color = { 1,1,1,1 };       // 色
	DirectX::XMFLOAT4 firstColor = { 1,1,1,1 };  // 最初の色
	bool isActive = true;                        // 描画可否
	DirectX::XMMATRIX matrixWorld;				 // ワールド変換行列
	
public:
	GameObject(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _rot, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT4 _color);
	virtual ~GameObject() = default;
	virtual void Update(float _deltaTime) {};
	virtual void Draw() {};
	// Set関数
	virtual void SetPosition(DirectX::XMFLOAT3 _pos) { position = _pos; };
	void SetRotation(DirectX::XMFLOAT3 _rot) { rotation = _rot; }
	void SetIsActive(bool _active) { isActive = _active; }
	void SetScale(DirectX::XMFLOAT3 _scale) { scaling = _scale; };
	void SetMatrixWorld(DirectX::XMMATRIX _matrix) { matrixWorld = _matrix; }
	void SetRGBColor(DirectX::XMFLOAT3 _rgb);
	void SetFirstColor() { color = firstColor; }
	void SetColorAlpha(float _alpha) { color.w = _alpha; }
	// Get関数
	bool& GetIsActive() { return  isActive; }
	DirectX::XMFLOAT3 GetPosition() const { return position; }
	DirectX::XMFLOAT3 GetScale() { return scaling; }
	DirectX::XMMATRIX GetMatrixWorld() { return matrixWorld; }
	DirectX::XMFLOAT3 GetRGBColor();
	DirectX::XMFLOAT4 GetColor() { return color; }
	// ワールド変換行列を計算
	static DirectX::XMMATRIX CalculateMatrixWorld(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT3 _rot);
	
protected:
	// UV行列取得
	virtual DirectX::XMMATRIX GetUvMatrix();
	
};