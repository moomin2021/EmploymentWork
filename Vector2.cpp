#include "Vector2.h"
#include <cmath>    // sqrt

// 引数なしのコンストラクタ
Vector2::Vector2() : x(0), y(0) {}

// 引数2つのコンストラクタ
Vector2::Vector2(float x, float y) : x(x), y(y) {}

// ベクトルの長さを求める
float Vector2::length() const
{
    return sqrt((x * x) + (y * y));
}

// ベクトルの正規化
Vector2& Vector2::normalize()
{
    float len = length();
    if (len != 0)
    {
        return *this /= len;
    }
    return *this;
}

// 内積を求める
float Vector2::dot(const Vector2& v) const
{
    return x * v.x + y * v.y;
}

// 外積を求める
float Vector2::cross(const Vector2& v) const
{
    return x * v.y - y * v.x;
}

// 単項演算子+を使えるようにする
Vector2 Vector2::operator+() const
{
    return *this;
}

// 単項演算子-を使えるようにする
Vector2 Vector2::operator-() const
{
    return Vector2(-x, -y);
}

// +=を使用できるようにする
Vector2& Vector2::operator+=(const Vector2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

// -=を使用できるようにする
Vector2& Vector2::operator-=(const Vector2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

// ベクトルをs倍にする
Vector2& Vector2::operator*=(float s)
{
    x *= s;
    y *= s;
    return *this;
}

// ベクトルを1/s倍にする
Vector2& Vector2::operator/=(float s)
{
    x /= s;
    y /= s;
    return *this;
}

// Vector2 クラスに属さない関数軍
// 二項演算子
const Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
    Vector2 temp(v1);
    return temp += v2;
}

// ベクトルの引き算ができるようにする
const Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
    Vector2 temp(v1);
    return temp -= v2;
}

// ベクトルのs倍を計算できるようにする
const Vector2 operator*(const Vector2& v, float s)
{
    Vector2 temp(v);
    return temp *= s;
}

// ベクトルのs倍を計算できるようにする
const Vector2 operator*(float s, const Vector2& v)
{
    return v * s;
}

// ベクトルの1/s倍を計算できるようにする
const Vector2 operator/(const Vector2& v, float s)
{
    Vector2 temp(v);
    return temp /= s;
}