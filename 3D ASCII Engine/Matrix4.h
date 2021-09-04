#pragma once

class Vec4;

class Matrix4
{
private:
	float values[4 * 4];
public:
	// intializes a 0 matrix
	Matrix4();

	void setIdentity(); // sets this matrix to the identity matrix

	void print(); // prints out this matrix in an easy to read format // for debugging

	Matrix4& multiply(Matrix4& rh);

	// operations to handle transformations
	void scale(float scalar);
	void translate(float dx, float dy, float dz);
	void rotate(float angle, float rx, float ry, float rz);

	void perspective(float angle, float aspect, float near, float far); // set this matrix to a perspective matrix
	void view(const Vec4& right, const Vec4& up, const Vec4& forward, const Vec4& position);
	void lookAt(const Vec4& eye, const Vec4& center, const Vec4& up);

	void set(int row, int column, float value);
	float get(int row, int column) const;
};