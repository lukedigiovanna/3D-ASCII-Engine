#pragma once
#include "Matrix4.h"

class Vec4
{
private:
	float applyDot(const Matrix4& mat, int row);

public:
	float x, y, z;

	Vec4() : x(0), y(0), z(0) {}
	Vec4(float x, float y, float z) : x(x), y(y), z(z) {}

	void multiply(const Matrix4& transformation);

	Vec4& operator+(const Vec4& rh) const; // calculates and returns the sum of the two vectors
	Vec4& operator-(const Vec4& rh) const;
	Vec4& operator*(const float rh) const;
	void operator+=(const Vec4& rh); // calculates and returns the sum of the two vectors
	void operator-=(const Vec4& rh);
	void operator*=(const float rh);

	Vec4& cross(const Vec4& rh) const;
	float dot(const Vec4& rh) const;

	void normalize();

	void print() const;
};
