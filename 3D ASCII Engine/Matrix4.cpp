#include "Matrix4.h"
#include "Vec4.h"
#include <iostream>

Matrix4::Matrix4()
{
	for (int i = 0; i < 16; i++) values[i] = 0;
}

void Matrix4::setIdentity()
{
	for (int i = 0; i < 16; i += 5) values[i] = 1;
}

void Matrix4::print()
{
	for (int i = 0; i < 4; i++)
		printf("%f %f %f %f\n", this->values[i * 4 + 0], this->values[i * 4 + 1], this->values[i * 4 + 2], this->values[i * 4 + 3]);
}

Matrix4& Matrix4::multiply(Matrix4& rh)
{
	Matrix4 out;
	for (int i = 0; i < 4; i++) { // row of out matrix
		for (int j = 0; j < 4; j++) { // column of out matrix
			float sum = 0;
			for (int k = 0; k < 4; k++) {
				sum += this->values[i * 4 + k] * rh.values[j + k * 4];
			}
			out.values[i * 4 + j] = sum;
		}
	}
	return out;
}

void Matrix4::scale(float scalar)
{
	this->values[0] *= scalar;
	this->values[5] *= scalar;
	this->values[10] *= scalar;
}

void Matrix4::translate(float dx, float dy, float dz)
{
	this->values[3] = dx;
	this->values[7] = dy;
	this->values[11] = dz;
}

void Matrix4::rotate(float angle, float rx, float ry, float rz)
{
	float mag = std::sqrt(rx * rx + ry * ry + rz * rz);
	rx /= mag;
	ry /= mag;
	rz /= mag;
	float cos = std::cosf(angle);
	float sin = std::sinf(angle);
	float oneMinus = 1 - cos;
	this->set(0, 0, cos + rx * rx * oneMinus);
	this->set(0, 1, rx * ry * oneMinus - rz * sin);
	this->set(0, 2, rx * rz * oneMinus + ry * sin);
	this->set(1, 0, ry * rx * oneMinus + rz * sin);
	this->set(1, 1, cos + ry * ry * oneMinus);
	this->set(1, 2, ry * rz * oneMinus - rz * sin);
	this->set(2, 0, rz * rx * oneMinus - ry * sin);
	this->set(2, 1, rz * ry * oneMinus + rx * sin);
	this->set(2, 2, cos + rz * rz * oneMinus);
	this->set(3, 3, 1);
}

// sets a perspective matrix
void Matrix4::perspective(float angle, float aspect, float near, float far)
{
	float scale = 1 / std::tanf(angle * 0.5f * 3.1415965f / 180);
	this->set(0, 0, scale / aspect);
	this->set(1, 1, scale);
	this->set(2, 2, -far / (far - near));
	this->set(3, 2, -(2 * far * near) / (far - near));
	this->set(2, 3, -1);
	this->set(3, 3, 0);
}

void Matrix4::view(const Vec4& right, const Vec4& up, const Vec4& forward, const Vec4& position)
{
	this->set(0, 0, right.x);
	this->set(1, 0, right.y);
	this->set(2, 0, right.z);
	this->set(3, 0, -right.dot(position));

	this->set(0, 1, up.x);
	this->set(1, 1, up.y);
	this->set(2, 1, up.z);
	this->set(3, 1, -up.dot(position));

	this->set(0, 2, forward.x);
	this->set(1, 2, forward.y);
	this->set(2, 2, forward.z);
	this->set(3, 2, -forward.dot(position));

	this->set(0, 3, position.x);
	this->set(1, 3, position.y);
	this->set(2, 3, position.z);
	this->set(3, 3, 1);
}

void Matrix4::lookAt(const Vec4& eye, const Vec4& center, const Vec4& up)
{
	Vec4 z = eye - center;
	z.normalize();
	Vec4 y = up;
	Vec4 x = y.cross(z);
	y = z.cross(x);
	x.normalize();
	y.normalize();
	this->set(0, 0, x.x);
	this->set(1, 0, x.y);
	this->set(2, 0, x.z);
	this->set(3, 0, -x.dot(eye));
	this->set(0, 1, y.x);
	this->set(1, 1, y.y);
	this->set(2, 1, y.z);
	this->set(3, 1, -y.dot(eye));
	this->set(0, 2, z.x);
	this->set(1, 2, z.y);
	this->set(2, 2, z.z);
	this->set(3, 2, -z.dot(eye));
	this->set(0, 3, 0);
	this->set(1, 3, 0);
	this->set(2, 3, 0);
	this->set(3, 3, 1);
}



void Matrix4::set(int row, int column, float value)
{
	if (row < 0 || row > 3 || column < 0 || column > 3) return;
	this->values[row * 4 + column] = value;
}

float Matrix4::get(int row, int column) const
{
	if (row >= 0 && row < 4 && column >= 0 && column < 4)
		return this->values[row * 4 + column];
	else
		return 0.0f;
}
