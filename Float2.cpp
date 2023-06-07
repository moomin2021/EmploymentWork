#include "Float2.h"

// 引数無しのコンストラクタ
Float2::Float2() : x(0.0f), y(0.0f) {}

// 引数ありのコンストラクタ
Float2::Float2(float x, float y) : x(x), y(y) {}

// 単項演算子+を使えるようにする
Float2 Float2::operator+() const
{
    return *this;
}

// 単項演算子-を使えるようにする
Float2 Float2::operator-() const
{
    return Float2(-x, -y);
}

// +=を使用できるようにする
Float2& Float2::operator+=(const Float2& f)
{
    x += f.x;
    y += f.y;
    return *this;
}

// -=を使用できるようにする
Float2& Float2::operator-=(const Float2& f)
{
    x -= f.x;
    y -= f.y;
    return *this;
}

// ベクトルをs倍にする
Float2& Float2::operator*=(float s)
{
    x *= s;
    y *= s;
    return *this;
}

// ベクトルを1/s倍にする
Float2& Float2::operator/=(float s)
{
    x /= s;
    y /= s;
    return *this;
}

const Float2 operator+(const Float2& f1, const Float2& f2)
{
    Float2 tmp(f1);
    return tmp += f2;
}

const Float2 operator-(const Float2& f1, const Float2& f2)
{
    Float2 tmp(f1);
    return tmp -= f2;
}

const Float2 operator*(const Float2& f, float s)
{
    Float2 tmp(f);
    return tmp *= s;
}

const Float2 operator*(float s, const Float2& f)
{
    return f * s;
}

const Float2 operator/(const Float2& f, float s)
{
    Float2 tmp(f);
    return tmp /= s;
}
