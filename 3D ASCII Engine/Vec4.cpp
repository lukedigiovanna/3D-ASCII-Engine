#include "Vec4.h"
#include <iostream>

float Vec4::applyDot(const Matrix4& mat, int row)
{
	return mat.get(row, 0) * this->x + mat.get(row, 1) * this->y + mat.get(row, 2) * this->z + mat.get(row, 3);
}

void Vec4::multiply(const Matrix4& transformation)
{
	float nx = this->applyDot(transformation, 0),
		ny = this->applyDot(transformation, 1),
		nz = this->applyDot(transformation, 2),
		w = this->applyDot(transformation, 3);
	this->x = nx;
	this->y = ny;
	this->z = nz;
	this->w = w;
}

Vec4& Vec4::operator+(const Vec4& rh) const
{
	Vec4 vec = Vec4(this->x + rh.x, this->y + rh.y, this->z + rh.z);
	return vec;
}

Vec4& Vec4::operator-(const Vec4& rh) const
{
	Vec4 vec = Vec4(this->x - rh.x, this->y - rh.y, this->z - rh.z);
	return vec;
}

Vec4& Vec4::operator*(const float rh) const
{
	Vec4 vec = Vec4(this->x * rh, this->y * rh, this->z * rh);
	return vec;
}

void Vec4::operator+=(const Vec4& rh)
{
	this->x += rh.x;
	this->y += rh.y;
	this->z += rh.z;
}

void Vec4::operator-=(const Vec4& rh)
{
	this->x -= rh.x;
	this->y -= rh.y;
	this->z -= rh.z;
}

void Vec4::operator*=(const float rh)
{
	this->x *= rh;
	this->y *= rh;
	this->z *= rh;
}

Vec4& Vec4::cross(const Vec4& rh) const
{
	Vec4 vec = Vec4(this->y * rh.z - this->z * rh.y, this->z * rh.x - this->x * rh.z, this->x * rh.y - this->y * rh.x);
	return vec;
}

float Vec4::dot(const Vec4& rh) const
{
	return this->x * rh.x + this->y * rh.y + this->z * rh.z;
}

void Vec4::normalize()
{
	float mag = std::sqrt(x * x + y * y + z * z);
	x /= mag;
	y /= mag;
	z /= mag;
}

void Vec4::print() const
{
	printf("[ %f %f %f %f ]", this->x, this->y, this->z, 1.0f);
}
