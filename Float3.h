#pragma once
struct Float3
{
	float x;// -> x成分
	float y;// -> y成分
	float z;// -> z成分

	// コンストラクタ
	Float3();// --------------------------> 引数なし
	Float3(float x, float y, float z);// -> 引数あり

	// 単項演算子オーバーロード
	Float3 operator+() const;
	Float3 operator-() const;

	// 代入演算子オーバーロード
	Float3& operator+=(const Float3& f);
	Float3& operator-=(const Float3& f);
	Float3& operator*=(float s);
	Float3& operator/=(float s);
};

// 2項演算子オーバーロード
// ※いろんな引数のパターンに対応(引数の順序)するため、以下のように準備している
const Float3 operator+(const Float3& f1, const Float3& f2);
const Float3 operator-(const Float3& f1, const Float3& f2);
const Float3 operator*(const Float3& f, float s);
const Float3 operator*(float s, const Float3& f);
const Float3 operator/(const Float3& f, float s);