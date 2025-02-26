#pragma once

#include <DirectXMath.h>
#include <math.h>

//===========================================//
// �R�����x�N�g���N���X
//===========================================//
class Vec3
{
public:
	float x, y, z;

	// Vec3 �� XMFLOAT3
	static DirectX::XMFLOAT3 SetXMFLOAT3(Vec3 _v)
	{
		return { _v.x, _v.y, _v.z };
	}
	// XMFLOAT3 �� Vec3
	static Vec3 SetVector(DirectX::XMFLOAT3 _float3)
	{
		return { _float3.x, _float3.y, _float3.z };
	}
	
	// ���Z�q�I�[�o�[���[�h
	// ���Z
	inline Vec3 operator+(const Vec3& _v3) const {
		return Vec3{ this->x + _v3.x, this->y + _v3.y,this->z + _v3.z };
	}
	// ���Z�i�����j
	inline Vec3 operator+(const float _r) const {
		return	Vec3{ this->x + _r ,this->y + _r, this->z + _r };
	}

	// ���Z
	inline Vec3 operator-(const Vec3& _v3) const {
		return Vec3{ this->x - _v3.x, this->y - _v3.y,this->z - _v3.z };
	}
	// ���Z�i�����j
	inline Vec3 operator-(const float _r) const {
		return	Vec3{ this->x - _r ,this->y - _r, this->z - _r };
	}

	// ��Z
	inline Vec3 operator*(const Vec3& _v3) const {
		return Vec3{ this->x * _v3.x, this->y * _v3.y,this->z * _v3.z };
	}
	// ��Z�i�����j
	inline Vec3 operator*(const float _r) const {
		return	Vec3{ this->x * _r ,this->y * _r, this->z * _r };
	}

	// ���Z
	inline Vec3 operator/(const Vec3& _v3) const {
		return Vec3{ this->x / _v3.x, this->y / _v3.y,this->z / _v3.z };
	}
	// ���Z�i�����j
	inline Vec3 operator/(const float _r) const {
		if (_r == 0) {
			return V_ZERO;	//�[�����Z�̏ꍇ�̓[���x�N�g����Ԃ�
		}
		return	Vec3{ this->x / _r ,this->y / _r, this->z / _r };
	}

	// ���ȑ�����Z�q
	// ���Z(v1 += v2)
	inline Vec3& operator+=(const Vec3& _v) {
		this->x += _v.x;
		this->y += _v.y;
		this->z += _v.z;
		return	*this;
	}
	// ���Z(v1 += ����)
	inline Vec3& operator+=(const float _r) {
		this->x += _r;
		this->y += _r;
		this->z += _r;
		return	*this;
	}

	// ��Z(v1 *= v2)
	inline Vec3& operator*=(const Vec3& _v) {
		this->x *= _v.x;
		this->y *= _v.y;
		this->z *= _v.z;
		return	*this;
	}
	// ��Z(v1 *= ����)
	inline Vec3& operator*=(const float _r) {
		this->x *= _r;
		this->y *= _r;
		this->z *= _r;
		return	*this;
	}

	// ��r���Z
	inline bool operator!=(const Vec3& _v){
		
		return	this->x != _v.x || this->y != _v.y || this->z != _v.z;
	}

	// ����
	inline float Dot(const Vec3& _v3) const
	{
		return (_v3.x * _v3.x) + (_v3.y * _v3.y) + (_v3.z * _v3.z);
	}

	// �O��
	inline Vec3 Cross(const Vec3& _v3) const
	{
		return{ ((this->y * _v3.z) - (this->z * _v3.y)),((this->z * _v3.x) - (this->x * _v3.z)), ((this->x * _v3.y) - (this->y * _v3.x)) };
	}

	// ����
	inline float Length(void) const
	{
		return sqrtf(this->Dot(*this));
	}

	// �P�ʃx�N�g��
	inline Vec3 Normalize(void) const
	{
		return *this / this->Length();
	}

	static const Vec3 V_ZERO;	// �[���x�N�g��

	static DirectX::XMFLOAT3 LerpXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, float t)
	{
		// XMFLOAT3 �̐��`���
		DirectX::XMFLOAT3 result;
		result.x = start.x + t * (end.x - start.x);
		result.y = start.y + t * (end.y - start.y);
		result.z = start.z + t * (end.z - start.z);
		return result;
	}

	static DirectX::XMFLOAT3 CatmullRomXMFLOAT3(
		const DirectX::XMFLOAT3& p0, 
		const DirectX::XMFLOAT3& p1, 
		const DirectX::XMFLOAT3& p2, 
		const DirectX::XMFLOAT3& p3, 
		float t)
	{
		float t2 = t * t;
		float t3 = t2 * t;

		DirectX::XMFLOAT3 result;
		result.x = 0.5f * (2.0f * p1.x + (-p0.x + p2.x) * t +
			(2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
			(-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);

		result.y = 0.5f * (2.0f * p1.y + (-p0.y + p2.y) * t +
			(2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
			(-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);

		result.z = 0.5f * (2.0f * p1.z + (-p0.z + p2.z) * t +
			(2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 +
			(-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3);

		return result;
	}

	static float CalculateDistance(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) 
	{
		using namespace DirectX;
		DirectX::XMVECTOR va = DirectX::XMLoadFloat3(&a);
		DirectX::XMVECTOR vb = DirectX::XMLoadFloat3(&b);
		DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(va, vb);
		return DirectX::XMVectorGetX(DirectX::XMVector3Length(diff));
	}
};
__declspec(selectany) const Vec3 Vec3::V_ZERO{ 0, 0 ,0 };