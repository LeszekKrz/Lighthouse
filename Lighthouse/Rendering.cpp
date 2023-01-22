#include "Rendering.h"

const int VIT1 = 51;
const int VIT2 = 106;

void TransformObjects(object objects[4], object currObjects[4], camera view, float boatAngle, int cameraType)
{
	float4x4 scaleM;
	float4x4 moveM;
	float4x4 rotateM;
	float4x4 viewM;
	float4x4 projectionM; 

	float4 lookAt = objects[1].triangles.at(51).vertices[2].coordinates + float4(0, 0.5, 0, 0);
	float4 lookFrom = lookAt - float4(3, 0, 0, 0);


	scaleM = make_float4x4_scale(40);
	currObjects[0] = objects[0] * scaleM;

	scaleM = make_float4x4_scale(25);
	moveM = make_float4x4_translation(float3(0, 0, 20));
	rotateM = make_float4x4_rotation_y(boatAngle);
	currObjects[1] = objects[1] * scaleM * moveM * rotateM;
	lookAt = transform(lookAt,scaleM * moveM * rotateM);
	lookFrom = transform(lookFrom,scaleM * moveM * rotateM);

	scaleM = make_float4x4_scale(62.5);
	moveM = make_float4x4_translation(float3(50, 0, 0));
	currObjects[2] = objects[2] * scaleM * moveM;


	switch (cameraType)
	{
	case 0:
		viewM = make_float4x4_look_at(view.origin, view.aim, float3(0, 1, 0));
		break;
	case 1:
		float4 pointOfInterest = currObjects[1].triangles.at(51).vertices[2].coordinates;
		viewM = make_float4x4_look_at(view.origin, float3(pointOfInterest.x, pointOfInterest.y, pointOfInterest.z), float3(0, 1, 0));
		break;
	case 2:
		viewM = make_float4x4_look_at(float3(lookFrom.x, lookFrom.y, lookFrom.z), float3(lookAt.x, lookAt.y, lookAt.z), float3(0, 1, 0));
		break;
	default:
		break;
	}
	
	

	//float4 lookFrom = currObjects[1].triangles.at(51).vertices[2].coordinates;
	//float4 lookAt = currObjects[1].triangles.at(106).vertices[2].coordinates;
	//float4 lookFrom = lookAt - float4(20, 0, 0, 0);

	projectionM = make_float4x4_perspective_field_of_view(view.fov, view.aspect, view.n, view.f);
	for (int i = 0; i < 3; i++)
	{
		currObjects[i] = currObjects[i] * viewM * projectionM;
	}
	
}

void DrawObjects(object objects[4], unsigned char* texture)
{
	int x,y;
	unsigned char* pixel;
	triangle _triangle;
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < objects[k].triangles.size(); i++)
		{
			_triangle = objects[k].triangles.at(i);
			for (int j = 0; j < 3; j++)
			{
				float4 coos = _triangle.vertices[j].coordinates;
				coos /= coos.w;
				if (coos.x < -1 || coos.y < -1 || coos.z < -1 || coos.x > 1 || coos.y > 1 || coos.z > 1) continue;
				x = (int)((coos.x + 1) / 2 * 800);
				y = (int)((-coos.y + 1) / 2 * 800);
				pixel = texture + (y) * 800 * 3 + x * 3;
				*pixel = 0;
				*(pixel + 1) = 0;
				*(pixel + 2) = 0;
			}
		}
	}
}