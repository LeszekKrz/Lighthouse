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

	float4 lookAt = objects[1].triangles.at(51).vertices[2].coordinates + float4(0, 0.5, 0, 0);
	float4 lookFrom = lookAt - float4(3, 0, 0, 0);


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

	scaleM = make_float4x4_scale(250);
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
	for (int k = 0; k < 4; k++)
	{
		for (int i = 0; i < objects[k].triangles.size(); i++)
		{
			_triangle = PrepareForScreen(objects[k].triangles.at(i), 800, 800);
			//if (!VisibleTriangle(_triangle))
			worldTriangle = worldObjects[k].triangles.at(i);
			float4 middle43 = worldTriangle.vertices[0].coordinates + worldTriangle.vertices[1].coordinates + worldTriangle.vertices[2].coordinates;
			float3 middle3 = float3(middle43.x, middle43.y, middle43.z)/3;
			float3 middleN = (worldTriangle.vertices[0].normal + worldTriangle.vertices[1].normal + worldTriangle.vertices[2].normal) / 3;
			std::vector<AET> aets;
			PrepareAET(_triangle, aets);
			point middle = (worldTriangle.vertices[0] + worldTriangle.vertices[1] + worldTriangle.vertices[2]) / 3;
			color = CalculateColor(middle, objects[k].color, view, float3(0, 100, 0));
			DrawTriangle(aets, color, texture, zBuff);
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
		//} while (false);
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
	return false;
}
