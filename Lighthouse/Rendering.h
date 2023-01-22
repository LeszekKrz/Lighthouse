#pragma once
#include "Objects.h"

typedef struct {
	float3 origin;
	float3 aim;
	float n;
	float f;
	float fov;
	float aspect;
} camera;

void TransformObjects(object objects[4], object currObjects[4], camera view, float boatAngle, int cameraType);
void DrawObjects(object objects[4], unsigned char* texture);
