#include "Float4.h"

// 引数なしのコンストラクタ
Float4::Float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

// 引数ありのコンストラクタ
Float4::Float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Float4 Float4::operator+() const
{
	return *this;
}

Float4 Float4::operator-() const
{
	return Float4(-x, -y, -z, -w);
}

Float4& Float4::operator+=(const Float4& f)
{
	x += f.x;
	y += f.y;
	z += f.z;
	w += f.w;
	return *this;
}

Float4& Float4::operator-=(const Float4& f)
{
	x -= f.x;
	y -= f.y;
	z -= f.z;
	w -= f.w;
	return *this;
}

Float4& Float4::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

Float4& Float4::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return *this;
}

const Float4 operator+(const Float4& f1, const Float4& f2)
{
	Float4 tmp(f1);
	return tmp += f2;
}

const Float4 operator-(const Float4& f1, const Float4& f2)
{
	Float4 tmp(f1);
	return tmp += f2;
}

const Float4 operator*(const Float4& f, float s)
{
	Float4 tmp(f);
	return tmp *= s;
}

const Float4 operator*(float s, const Float4& f)
{
	return f * s;
}

const Float4 operator/(const Float4& f, float s)
{
	Float4 tmp(f);
	return tmp /= s;
}