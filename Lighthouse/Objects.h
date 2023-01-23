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
	bool operator<(float3 than);
	bool operator>=(float3 than);
	point operator+(point other);
	point operator/(float by);
} point;

typedef struct triangle{
	point vertices[3];

	triangle operator*=(float4x4 matrix);
	bool operator< (float3 than);
	bool operator>=(float3 than);
} triangle;

typedef struct object {
	std::vector<triangle> triangles;
	int color;

	object operator*(float4x4 matrix);
} object;

typedef struct AET {
	int y;
	int yStart;
	float x;
	int maxY;
	float m;
	float4 from;
	float4 to;
};

triangle PrepareForScreen(triangle _triangle, int width, int height);
AET MakeAET(float4 u, float4 v);
void PrepareAET(triangle _triangle, std::vector<AET> &aets);
void StepAET(AET& aet);
float zInterp(AET aet);