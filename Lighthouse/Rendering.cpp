#include "Rendering.h"
#include <algorithm>

const int VIT1 = 51;
const int VIT2 = 106;

void TransformObjects(object objects[4], object currObjects[4], object worldObjects[4], camera &view, float boatAngle, int cameraType)
{
	float4x4 scaleM;
	float4x4 moveM;
	float4x4 rotateM;
	float4x4 viewM;
	float4x4 projectionM; 

	float4 lookAt = objects[1].triangles.at(51).vertices[2].coordinates + float4(0, 0.2, 0, 0);
	float4 lookFrom = lookAt - float4(5, 0, 0, 0);


	scaleM = make_float4x4_scale(40);
	worldObjects[0] = objects[0] * scaleM;

	scaleM = make_float4x4_scale(25);
	moveM = make_float4x4_translation(float3(0, 0, 20));
	rotateM = make_float4x4_rotation_y(boatAngle);
	worldObjects[1] = objects[1] * scaleM * moveM * rotateM;
	lookAt = transform(lookAt,scaleM * moveM * rotateM);
	lookFrom = transform(lookFrom,scaleM * moveM * rotateM);

	scaleM = make_float4x4_scale(62.5);
	moveM = make_float4x4_translation(float3(50, 0, 0));
	worldObjects[2] = objects[2] * scaleM * moveM;

	scaleM = make_float4x4_scale(200);
	worldObjects[3] = objects[3] * scaleM;


	switch (cameraType)
	{
	case 0:
		view.currOrigin = view.origin;
		view.currAim = view.aim;
		break;
	case 1:
		float4 pointOfInterest = worldObjects[1].triangles.at(51).vertices[2].coordinates;
		view.currAim = float3(pointOfInterest.x, pointOfInterest.y, pointOfInterest.z);
		view.currOrigin = view.origin;
		break;
	case 2:
		view.currOrigin = float3(lookFrom.x, lookFrom.y, lookFrom.z);
		view.currAim = float3(lookAt.x, lookAt.y, lookAt.z);
		break;
	default:
		break;
	}
	
	
	viewM = make_float4x4_look_at(view.currOrigin, view.currAim, float3(0, 1, 0));

	//float4 lookFrom = currObjects[1].triangles.at(51).vertices[2].coordinates;
	//float4 lookAt = currObjects[1].triangles.at(106).vertices[2].coordinates;
	//float4 lookFrom = lookAt - float4(20, 0, 0, 0);

	projectionM = make_float4x4_perspective_field_of_view(view.fov, view.aspect, view.n, view.f);
	for (int i = 0; i < 4; i++)
	{
		currObjects[i] = worldObjects[i] * viewM * projectionM;
	}
	
}

void DrawObjects(object objects[4], object worldObjects[4], camera view, unsigned char* texture)
{
	int x,y;
	unsigned char* pixel;
	triangle _triangle;
	triangle worldTriangle;
	float* zBuff = (float*)malloc(800*800*sizeof(float));
	std::fill(zBuff, zBuff + 800 * 800, 20000.0f);
	int color;
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < objects[k].triangles.size(); i++)
		{
			_triangle = PrepareForScreen(objects[k].triangles.at(i), 800, 800);
			worldTriangle = worldObjects[k].triangles.at(i);
			if (!VisibleTriangle(_triangle)) continue;
			if (BackPlane(worldTriangle, view)) continue;
			float4 middle43 = worldTriangle.vertices[0].coordinates + worldTriangle.vertices[1].coordinates + worldTriangle.vertices[2].coordinates;
			float3 middle3 = float3(middle43.x, middle43.y, middle43.z)/3;
			float3 middleN = (worldTriangle.vertices[0].normal + worldTriangle.vertices[1].normal + worldTriangle.vertices[2].normal) / 3;
			point middle = (worldTriangle.vertices[0] + worldTriangle.vertices[1] + worldTriangle.vertices[2]) / 3;
			std::vector<AET> aets;
			//color = CalculateColor(middle, objects[k].color, view, float3(0, 100, 0));
			//PrepareAET(_triangle, aets);
			//DrawTriangle(aets, color, texture, zBuff);
			int colors[3];
			for (int i = 0; i < 3; i++)
			{
				colors[i] = CalculateColor(worldTriangle.vertices[i], objects[k].color, view, float3(0, 100, 0));
			}
			PrepareAETwithColors(_triangle, aets, colors);
			DrawTriangleGouraud(aets, texture, zBuff);
			for (int j = 0; j < 3; j++)
			{
				float4 coos = _triangle.vertices[j].coordinates;
				x = (int)coos.x;
				y = (int)coos.y;
				//if (x < 0 || x >= 800 || y < 0 || y >= 800 || coos.z < -1 || coos.z >= 1) continue;
				if (x < 0 || x >= 800 || y < 0 || y >= 800) continue;
				pixel = texture + (y) * 800 * 3 + x * 3;
				*pixel = 0;
				*(pixel + 1) = 0;
				*(pixel + 2) = 0;
			}
		}
	}
	free(zBuff);
}

void DrawTriangle(std::vector<AET> aets, int color, unsigned char* texture, float* zBuffer)
{
	int x;
	unsigned char* pixel;
	bool finished = true;
	float z,z1,z2;
	float q;
	if (aets.size() > 0)
	{
		AET from = aets.at(0);
		AET to = aets.at(1);
		do
		{
			while (from.y != from.maxY && to.y != to.maxY)
			{
				if (from.y < 0 || from.y >= 800)
				{
					StepAET(from);
					StepAET(to);
					continue;
				}
				z1 = zInterp(from);
				z2 = zInterp(to);
				for (int x = (int)from.x; x <= (int)to.x; x++)
				{
					if (x < 0 || x >= 800) continue;
					if (to.x == from.x) z = z1;
					else
					{
						q = ((float)(x - from.x)) / (to.x - from.x);
						z = z1 * (1 - q) + z2 * q;
					}
					if (z < zBuffer[from.y * 800 + x])
					{
						pixel = texture + from.y * 800 * 3 + x * 3;
						*pixel = (color >> 16) & 255;
						*(pixel + 1) = (color >> 8) & 255;
						*(pixel + 2) = color & 255;
						zBuffer[from.y * 800 +x] = z;
					}
				}
				StepAET(from);
				StepAET(to);
			}
			if (!finished) break;
			if (aets.size() > 2)
			{
				if (from.y == from.maxY) from = aets.at(2);
				else to = aets.at(2);
				finished = false;
			}
		} while (!finished);
	}
}

void DrawTriangleGouraud(std::vector<AET> aets, unsigned char* texture, float* zBuffer)
{
	int x;
	unsigned char* pixel;
	bool finished = true;
	float z, z1, z2;
	float q;
	float3 c1,c2,color;
	if (aets.size() > 0)
	{
		AET from = aets.at(0);
		AET to = aets.at(1);
		do
		{
			while (from.y != from.maxY && to.y != to.maxY)
			{
				if (from.y < 0 || from.y >= 800)
				{
					StepAET(from);
					StepAET(to);
					continue;
				}
				z1 = zInterp(from);
				z2 = zInterp(to);
				c1 = colorInterp(from);
				c2 = colorInterp(to);
				for (int x = (int)from.x; x <= (int)to.x; x++)
				{
					if (x < 0 || x >= 800) continue;
					if (to.x == from.x)
					{
						z = z1;
						color = c1;
					}
					else
					{
						q = ((float)(x - from.x)) / (to.x - from.x);
						z = z1 * (1 - q) + z2 * q;
						color = lerp(c1, c2, q);
					}
					if (z < zBuffer[from.y * 800 + x])
					{
						pixel = texture + from.y * 800 * 3 + x * 3;
						*pixel = ((int)color.x) & 255;
						*(pixel + 1) = ((int)color.y) & 255;
						*(pixel + 2) = ((int)color.z) & 255;
						zBuffer[from.y * 800 + x] = z;
					}
				}
				StepAET(from);
				StepAET(to);
			}
			if (!finished) break;
			if (aets.size() > 2)
			{
				if (from.y == from.maxY) from = aets.at(2);
				else to = aets.at(2);
				finished = false;
			}
		} while (!finished);
	}
}

int CalculateColor(point _point, int objColor, camera view, float3 light)
{
	int color = 0;
	float ks = 0.5;
	float kd = 0.5;
	float ka = 0.5;
	int alfa = 5;
	float Ia = 0;
	float3 coos = float3(_point.coordinates.x, _point.coordinates.y, _point.coordinates.z);
	float3 Li = normalize(light - coos);
	float3 N = normalize(_point.normal);
	float3 V = normalize(view.currOrigin - coos);
	float3 R = normalize(2 * dot(Li, N) * N - Li);
	float channel;
	float resultChannel;
	for (int i = 0; i < 3; i++)
	{
		channel = ((float)((objColor >> i * 8) & 255)) / 255;
		resultChannel = ka * Ia;
		resultChannel += kd * dot(Li, N) * channel;
		resultChannel += ks * pow(dot(R, V), alfa) * channel;

		if (resultChannel > 1) resultChannel = 1;
		if (resultChannel < 0) resultChannel = 0;
		color |= ((int)(resultChannel * 255) & 255) << i * 8;
	}
	return color;
}

bool VisibleTriangle(triangle _triangle)
{
	float4 a = _triangle.vertices[0].coordinates;
	float4 b = _triangle.vertices[1].coordinates;
	float4 c = _triangle.vertices[2].coordinates;

	if (a.x < 0 && b.x < 0 && c.x < 0) return false;
	if (a.x >= 800 && b.x >= 800 && c.x >= 800) return false;
	if (a.y < 0 && b.y < 0 && c.y < 0) return false;
	if (a.y >= 800 && b.y >= 800 && c.y >= 800) return false;
	if (a.z < -1 && b.z < -1 && c.z < -1) return false;
	if (a.z >= 1 && b.z >= 1 && c.z >= 1) return false;
}

bool BackPlane(triangle _triangle, camera view)
{
	float4 v = _triangle.vertices[0].coordinates;
	float3 v0 = float3(v.x, v.y, v.z);
	v = _triangle.vertices[1].coordinates;
	float3 v1 = float3(v.x, v.y, v.z);
	v = _triangle.vertices[2].coordinates;
	float3 v2 = float3(v.x, v.y, v.z);
	float3 N = cross(v1 - v0, v2 - v0);
	return dot((v0 - view.currOrigin),N) >= 0;
}
