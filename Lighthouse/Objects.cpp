#include "Objects.h"

object object::operator*(float4x4 matrix)
{
	object result(*this);
	for (int i = 0; i < result.triangles.size(); i++)
	{
		result.triangles.at(i) *= matrix;
	}
	return result;
}

triangle triangle::operator*=(float4x4 matrix)
{
	for (int i = 0; i < 3; i++)
	{
		this->vertices[i] *= matrix;
	}
	return *this;
}

point point::operator*=(float4x4 matrix)
{
	this->coordinates = transform(this->coordinates, matrix);
	return *this;
}
