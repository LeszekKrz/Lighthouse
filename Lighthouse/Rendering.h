#pragma once
#include "Objects.h"

typedef struct {
	float3 origin;
	float3 aim;
	float3 currOrigin;
	float3 currAim;
	float n;
	float f;
	float fov;
	float aspect;
} camera;

void TransformObjects(object objects[4], object currObjects[4], object worldObjects[4], camera &view, point& reflector, float boatAngle, int cameraType, float angle);
void DrawObjects(object objects[4], object worldObjects[4], camera view, point reflector, unsigned char* texture, int shadingType);
void DrawTriangle(std::vector<AET> aets, int color, unsigned char* texture, float* zBuffer);
void DrawTriangleGouraud(std::vector<AET> aets, unsigned char* texture, float* zBuffer);
void DrawTrianglePhong(std::vector<AET> aets, int objColor, camera view, float3 light, point reflector, unsigned char* texture, float* zBuffer);
int CalculateColor(point _point, int objColor, camera view, float3 light, point reflector);
bool VisibleTriangle(triangle _triangle);
bool BackPlane(triangle _triangle, camera view);