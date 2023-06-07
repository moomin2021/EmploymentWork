#pragma once
struct Float2
{
	float x;// -> x成分
	float y;// -> y成分

	// コンストラクタ
	Float2();// -----------------> 引数なし
	Float2(float x, float y);// -> 引数あり

	// 単項演算子オーバーロード
	Float2 operator+() const;
	Float2 operator-() const;

	// 代入演算子オーバーロード
	Float2& operator+=(const Float2& f);
	Float2& operator-=(const Float2& f);
	Float2& operator*=(float s);
	Float2& operator/=(float s);
};

// 2項演算子オーバーロード
// ※いろんな引数のパターンに対応するため、以下のように準備している
const Float2 operator+(const Float2& f1, const Float2& f2);
const Float2 operator-(const Float2& f1, const Float2& f2);
const Float2 operator*(const Float2& f, float s);
const Float2 operator*(float s, const Float2& f);
const Float2 operator/(const Float2& f, float s);