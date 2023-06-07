#pragma once
struct Float4
{
	float x;// -> x成分
	float y;// -> y成分
	float z;// -> z成分
	float w;// -> w成分

	// コンストラクタ
	Float4();// -----------------------------------> 引数なし
	Float4(float x, float y, float z, float w);// -> 引数あり

	// 単項演算子オーバーロード
	Float4 operator+() const;
	Float4 operator-() const;

	// 代入演算子オーバーロード
	Float4& operator+=(const Float4& f);
	Float4& operator-=(const Float4& f);
	Float4& operator*=(float s);
	Float4& operator/=(float s);
};

// 2項演算子オーバーロード
// ※いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const Float4 operator+(const Float4& f1, const Float4& f2);
const Float4 operator-(const Float4& f1, const Float4& f2);
const Float4 operator*(const Float4& f, float s);
const Float4 operator*(float s, const Float4& f);
const Float4 operator/(const Float4& f, float s);