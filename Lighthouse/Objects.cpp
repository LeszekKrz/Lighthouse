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

bool triangle::operator<(float3 than)
{
	return vertices[0] < than && vertices[1] < than && vertices[2] < than;
}

bool triangle::operator>=(float3 than)
{
	return vertices[0] >= than && vertices[1] >= than && vertices[2] >= than;
}

point point::operator*=(float4x4 matrix)
{
	this->coordinates = transform(this->coordinates, matrix);
	this->normal = transform_normal(this->normal, matrix);
	return *this;
}

bool point::operator<(float3 than)
{
	return coordinates.x < than.x && coordinates.y < than.y && coordinates.x < than.z;
}

bool point::operator>=(float3 than)
{
	return coordinates.x >= than.x && coordinates.y >= than.y && coordinates.x >= than.z;
}

point point::operator+(point other)
{
	point result = { coordinates + other.coordinates, normalize(normal + other.normal) };
	return result;
}

point point::operator/(float by)
{
	point result = { coordinates / by, normal };
	return result;
}

triangle PrepareForScreen(triangle _triangle, int width, int height)
{
	float4 coos;
	triangle screenTriangle;
	for (int i = 0; i < 3; i++)
	{
		coos = _triangle.vertices[i].coordinates;
		coos /= coos.w;
		coos = (coos + float4(1, -1, 0, 0)) / 2;
		coos.x *= width;
		coos.y *= -height;
		screenTriangle.vertices[i].coordinates = coos;
	}
	return screenTriangle;
}

AET MakeAET(float4 u, float4 v)
{
	AET aet;
	aet.x = u.x;
	aet.y = (int)u.y;
	aet.yStart = aet.y;
	aet.maxY = (int)v.y;
	if (v.y == u.y) aet.m = 0;
	else aet.m = (v.x - u.x) / (v.y - u.y);
	aet.from = u;
	aet.to = v;
	return aet;
}

void PrepareAET(triangle _triangle, std::vector<AET> &aets)
{
	float a, b, c;
	float4 min, mid, max;
	a = _triangle.vertices[0].coordinates.y;
	b = _triangle.vertices[1].coordinates.y;
	c = _triangle.vertices[2].coordinates.y;
	if (a < b)
	{
		if (a < c)
		{
			min = _triangle.vertices[0].coordinates;
			if (b < c)
			{
				mid = _triangle.vertices[1].coordinates;
				max = _triangle.vertices[2].coordinates;
			}
			else
			{
				mid = _triangle.vertices[2].coordinates;
				max = _triangle.vertices[1].coordinates;
			}
		}
		else
		{
			min = _triangle.vertices[2].coordinates;
			mid = _triangle.vertices[0].coordinates;
			max = _triangle.vertices[1].coordinates;
		}

	}
	else
	{
		if (b < c)
		{
			min = _triangle.vertices[1].coordinates;
			if (a < c)
			{
				mid = _triangle.vertices[0].coordinates;
				max = _triangle.vertices[2].coordinates;
			}
			else
			{
				mid = _triangle.vertices[2].coordinates;
				max = _triangle.vertices[0].coordinates;
			}
		}
		else
		{
			min = _triangle.vertices[2].coordinates;
			mid = _triangle.vertices[1].coordinates;
			max = _triangle.vertices[0].coordinates;
		}
	}
	int miny = min.y;
	int midy = mid.y;
	int maxy = max.y;
	AET minMid = MakeAET(min, mid);
	AET minMax = MakeAET(min, max);
	if (miny == midy)
	{
		if (min.x < mid.x)
		{
			aets.push_back(minMax);
			aets.push_back(MakeAET(mid, max));
		}
		else
		{
			aets.push_back(MakeAET(mid, max));
			aets.push_back(minMax);
		}
		return;
	}
	if (minMid.m  < minMax.m)
	{
		if (midy != miny) aets.push_back(MakeAET(min, mid));
		if (maxy != miny) aets.push_back(MakeAET(min, max));
	}
	else
	{
		if (maxy != miny) aets.push_back(MakeAET(min, max));
		if (midy != miny) aets.push_back(MakeAET(min, mid));
	}
	if (maxy != miny) aets.push_back(MakeAET(mid, max));
	return;
}

void StepAET(AET& aet)
{
	aet.y++;
	aet.x += aet.m;
}

float zInterp(AET aet)
{
	float ratio = ((float)(aet.y - aet.yStart)) / (aet.maxY - aet.yStart);
	return aet.from.z * (1 - ratio) + aet.to.z * (ratio);
}



