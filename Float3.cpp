#include "Float3.h"

// 引数なしのコンストラクタ
Float3::Float3() : x(0.0f), y(0.0f), z(0.0f) {}

// 引数ありのコンストラクタ
Float3::Float3(float x, float y, float z) : x(x), y(y), z(z) {}

Float3 Float3::operator+() const
{
	return *this;
}

Float3 Float3::operator-() const
{
	return Float3(-x, -y, -z);
}

Float3& Float3::operator+=(const Float3& f)
{
	x += f.x;
	y += f.y;
	z += f.z;
	return *this;
}

Float3& Float3::operator-=(const Float3& f)
{
	x -= f.x;
	y -= f.y;
	z -= f.z;
	return *this;
}

Float3& Float3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Float3& Float3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

const Float3 operator+(const Float3& f1, const Float3& f2)
{
	Float3 tmp(f1);
	return tmp += f2;
}

const Float3 operator-(const Float3& f1, const Float3& f2)
{
	Float3 tmp(f1);
	return tmp += f2;
}

const Float3 operator*(const Float3& f, float s)
{
	Float3 tmp(f);
	return tmp *= s;
}

const Float3 operator*(float s, const Float3& f)
{
	return f * s;
}

const Float3 operator/(const Float3& f, float s)
{
	Float3 tmp(f);
	return tmp /= s;
}
