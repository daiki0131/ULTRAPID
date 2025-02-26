#pragma once

#include <DirectXMath.h>
#include <math.h>

//===========================================//
// ÇRéüå≥ÉxÉNÉgÉãÉNÉâÉX
//===========================================//
class Vec3
{
public:
	float x, y, z;

	// Vec3 Å® XMFLOAT3
	static DirectX::XMFLOAT3 SetXMFLOAT3(Vec3 _v)
	{
		return { _v.x, _v.y, _v.z };
	}
	// XMFLOAT3 Å® Vec3
	static Vec3 SetVector(DirectX::XMFLOAT3 _float3)
	{
		return { _float3.x, _float3.y, _float3.z };
	}
	
	// ââéZéqÉIÅ[ÉoÅ[ÉçÅ[Éh
	// â¡éZ
	inline Vec3 operator+(const Vec3& _v3) const {
		return Vec3{ this->x + _v3.x, this->y + _v3.y,this->z + _v3.z };
	}
	// â¡éZÅié¿êîÅj
	inline Vec3 operator+(const float _r) const {
		return	Vec3{ this->x + _r ,this->y + _r, this->z + _r };
	}

	// å∏éZ
	inline Vec3 operator-(const Vec3& _v3) const {
		return Vec3{ this->x - _v3.x, this->y - _v3.y,this->z - _v3.z };
	}
	// å∏éZÅié¿êîÅj
	inline Vec3 operator-(const float _r) const {
		return	Vec3{ this->x - _r ,this->y - _r, this->z - _r };
	}

	// èÊéZ
	inline Vec3 operator*(const Vec3& _v3) const {
		return Vec3{ this->x * _v3.x, this->y * _v3.y,this->z * _v3.z };
	}
	// èÊéZÅié¿êîÅj
	inline Vec3 operator*(const float _r) const {
		return	Vec3{ this->x * _r ,this->y * _r, this->z * _r };
	}

	// èúéZ
	inline Vec3 operator/(const Vec3& _v3) const {
		return Vec3{ this->x / _v3.x, this->y / _v3.y,this->z / _v3.z };
	}
	// èúéZÅié¿êîÅj
	inline Vec3 operator/(const float _r) const {
		if (_r == 0) {
			return V_ZERO;	//É[ÉçèúéZÇÃèÍçáÇÕÉ[ÉçÉxÉNÉgÉãÇï‘Ç∑
		}
		return	Vec3{ this->x / _r ,this->y / _r, this->z / _r };
	}

	// é©å»ë„ì¸ââéZéq
	// â¡éZ(v1 += v2)
	inline Vec3& operator+=(const Vec3& _v) {
		this->x += _v.x;
		this->y += _v.y;
		this->z += _v.z;
		return	*this;
	}
	// â¡éZ(v1 += é¿êî)
	inline Vec3& operator+=(const float _r) {
		this->x += _r;
		this->y += _r;
		this->z += _r;
		return	*this;
	}

	// èÊéZ(v1 *= v2)
	inline Vec3& operator*=(const Vec3& _v) {
		this->x *= _v.x;
		this->y *= _v.y;
		this->z *= _v.z;
		return	*this;
	}
	// èÊéZ(v1 *= é¿êî)
	inline Vec3& operator*=(const float _r) {
		this->x *= _r;
		this->y *= _r;
		this->z *= _r;
		return	*this;
	}

	// î‰ärââéZ
	inline bool operator!=(const Vec3& _v){
		
		return	this->x != _v.x || this->y != _v.y || this->z != _v.z;
	}

	// ì‡êœ
	inline float Dot(const Vec3& _v3) const
	{
		return (_v3.x * _v3.x) + (_v3.y * _v3.y) + (_v3.z * _v3.z);
	}

	// äOêœ
	inline Vec3 Cross(const Vec3& _v3) const
	{
		return{ ((this->y * _v3.z) - (this->z * _v3.y)),((this->z * _v3.x) - (this->x * _v3.z)), ((this->x * _v3.y) - (this->y * _v3.x)) };
	}

	// í∑Ç≥
	inline float Length(void) const
	{
		return sqrtf(this->Dot(*this));
	}

	// íPà ÉxÉNÉgÉã
	inline Vec3 Normalize(void) const
	{
		return *this / this->Length();
	}

	static const Vec3 V_ZERO;	// É[ÉçÉxÉNÉgÉã

	static DirectX::XMFLOAT3 LerpXMFLOAT3(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, float t)
	{
		// XMFLOAT3 ÇÃê¸å`ï‚ä‘
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