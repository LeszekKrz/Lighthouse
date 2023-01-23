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
	float zFrom;
	float zTo;
	float3 colorF;
	float3 colorT;
	point from;
	point to;

};

triangle PrepareForScreen(triangle _triangle, int width, int height);
AET MakeAET(float4 u, float4 v);
AET MakeAETwithColors(float4 u, float4 v, float3 cu, float3 cv);
AET MakeAETwithPoints(float4 u, float4 v, point p1, point p2);
void PrepareAET(triangle _triangle, std::vector<AET> &aets);
void PrepareAETwithColors(triangle _triangle, std::vector<AET>& aets, int colors[3]);
void PrepareAETwithPoints(triangle _triangle, triangle worldTriangle, std::vector<AET>& aets);
void StepAET(AET& aet);
float zInterp(AET aet);
float3 colorInterp(AET aet);
float3 decodeColor(int color);
point pointInterp(AET aet);