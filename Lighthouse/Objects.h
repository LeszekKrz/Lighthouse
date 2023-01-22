#pragma once
#include <iostream>
#include <cmath>
#include <WindowsNumerics.h>
#include <string>
#include <vector>

using namespace Windows::Foundation::Numerics;

typedef struct point {
	float4 coordinates;
	float3 normal;

	point operator*=(float4x4 matrix);
} point;

typedef struct triangle{
	point vertices[3];

	triangle operator*=(float4x4 matrix);
} triangle;

typedef struct object {
	std::vector<triangle> triangles;
	int color;

	object operator*(float4x4 matrix);
} object;
