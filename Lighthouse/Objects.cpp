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
	return coordinates.x < than.x && coordinates.y < than.y;
}

bool point::operator>=(float3 than)
{
	return coordinates.x >= than.x && coordinates.y >= than.y;
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
	aet.zFrom = u.z;
	aet.zTo = v.z;
	return aet;
}

AET MakeAETwithColors(float4 u, float4 v, float3 cu, float3 cv)
{
	AET aet = MakeAET(u, v);
	aet.colorF = cu;
	aet.colorT = cv;
	return aet;
}

AET MakeAETwithPoints(float4 u, float4 v, point p1, point p2)
{
	AET aet = MakeAET(u, v);
	aet.from = p1;
	aet.to = p2;
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

void PrepareAETwithColors(triangle _triangle, std::vector<AET>& aets, int colors[3])
{
	float a, b, c;
	float3 ca, cb, cc;
	float4 min, mid, max;
	float3 cmin, cmid, cmax;
	a = _triangle.vertices[0].coordinates.y;
	b = _triangle.vertices[1].coordinates.y;
	c = _triangle.vertices[2].coordinates.y;
	ca = decodeColor(colors[0]);
	cb = decodeColor(colors[1]);
	cc = decodeColor(colors[2]);
	
	
	if (a < b)
	{
		if (a < c)
		{
			min = _triangle.vertices[0].coordinates;
			cmin = ca;
			if (b < c)
			{
				mid = _triangle.vertices[1].coordinates;
				cmid = cb;
				max = _triangle.vertices[2].coordinates;
				cmax = cc;
			}
			else
			{
				mid = _triangle.vertices[2].coordinates;
				cmid = cc;
				max = _triangle.vertices[1].coordinates;
				cmax = cb;
			}
		}
		else
		{
			min = _triangle.vertices[2].coordinates;
			cmin = cc;
			mid = _triangle.vertices[0].coordinates;
			cmid = ca;
			max = _triangle.vertices[1].coordinates;
			cmax = cb;
		}

	}
	else
	{
		if (b < c)
		{
			min = _triangle.vertices[1].coordinates;
			cmin = cb;
			if (a < c)
			{
				mid = _triangle.vertices[0].coordinates;
				cmid = ca;
				max = _triangle.vertices[2].coordinates;
				cmax = cc;
			}
			else
			{
				mid = _triangle.vertices[2].coordinates;
				cmid = cc;
				max = _triangle.vertices[0].coordinates;
				cmax = ca;
			}
		}
		else
		{
			min = _triangle.vertices[2].coordinates;
			cmin = cc;
			mid = _triangle.vertices[1].coordinates;
			cmid = cb;
			max = _triangle.vertices[0].coordinates;
			cmax = ca;
		}
	}
	int miny = min.y;
	int midy = mid.y;
	int maxy = max.y;
	AET minMid = MakeAETwithColors(min, mid, cmin, cmid);
	AET minMax = MakeAETwithColors(min, max, cmin, cmax);
	if (miny == midy)
	{
		if (min.x < mid.x)
		{
			aets.push_back(minMax);
			aets.push_back(MakeAETwithColors(mid, max, cmid, cmax));
		}
		else
		{
			aets.push_back(MakeAETwithColors(mid, max, cmid, cmax));
			aets.push_back(minMax);
		}
		return;
	}
	if (minMid.m < minMax.m)
	{
		if (midy != miny) aets.push_back(MakeAETwithColors(min, mid, cmin, cmid));
		if (maxy != miny) aets.push_back(MakeAETwithColors(min, max, cmin, cmax));
	}
	else
	{
		if (maxy != miny) aets.push_back(MakeAETwithColors(min, max, cmin, cmax));
		if (midy != miny) aets.push_back(MakeAETwithColors(min, mid, cmin, cmid));
	}
	if (maxy != miny) aets.push_back(MakeAETwithColors(mid, max, cmid, cmax));
	return;
}

void PrepareAETwithPoints(triangle _triangle, triangle worldTriangle, std::vector<AET>& aets)
{
	float a, b, c;
	point pa, pb, pc;
	float4 min, mid, max;
	point pmin, pmid, pmax;
	a = _triangle.vertices[0].coordinates.y;
	b = _triangle.vertices[1].coordinates.y;
	c = _triangle.vertices[2].coordinates.y;
	pa = worldTriangle.vertices[0];
	pb = worldTriangle.vertices[1];
	pc = worldTriangle.vertices[2];



	if (a < b)
	{
		if (a < c)
		{
			min = _triangle.vertices[0].coordinates;
			pmin = pa;
			if (b < c)
			{
				mid = _triangle.vertices[1].coordinates;
				pmid = pb;
				max = _triangle.vertices[2].coordinates;
				pmax = pc;
			}
			else
			{
				mid = _triangle.vertices[2].coordinates;
				pmid = pc;
				max = _triangle.vertices[1].coordinates;
				pmax = pb;
			}
		}
		else
		{
			min = _triangle.vertices[2].coordinates;
			pmin = pc;
			mid = _triangle.vertices[0].coordinates;
			pmid = pa;
			max = _triangle.vertices[1].coordinates;
			pmax = pb;
		}

	}
	else
	{
		if (b < c)
		{
			min = _triangle.vertices[1].coordinates;
			pmin = pb;
			if (a < c)
			{
				mid = _triangle.vertices[0].coordinates;
				pmid = pa;
				max = _triangle.vertices[2].coordinates;
				pmax = pc;
			}
			else
			{
				mid = _triangle.vertices[2].coordinates;
				pmid = pc;
				max = _triangle.vertices[0].coordinates;
				pmax = pa;
			}
		}
		else
		{
			min = _triangle.vertices[2].coordinates;
			pmin = pc;
			mid = _triangle.vertices[1].coordinates;
			pmid = pb;
			max = _triangle.vertices[0].coordinates;
			pmax = pa;
		}
	}
	int miny = min.y;
	int midy = mid.y;
	int maxy = max.y;
	AET minMid = MakeAETwithPoints(min, mid, pmin, pmid);
	AET minMax = MakeAETwithPoints(min, max, pmin, pmax);
	if (miny == midy)
	{
		if (min.x < mid.x)
		{
			aets.push_back(minMax);
			aets.push_back(MakeAETwithPoints(mid, max, pmid, pmax));
		}
		else
		{
			aets.push_back(MakeAETwithPoints(mid, max, pmid, pmax));
			aets.push_back(minMax);
		}
		return;
	}
	if (minMid.m < minMax.m)
	{
		if (midy != miny) aets.push_back(MakeAETwithPoints(min, mid, pmin, pmid));
		if (maxy != miny) aets.push_back(MakeAETwithPoints(min, max, pmin, pmax));
	}
	else
	{
		if (maxy != miny) aets.push_back(MakeAETwithPoints(min, max, pmin, pmax));
		if (midy != miny) aets.push_back(MakeAETwithPoints(min, mid, pmin, pmid));
	}
	if (maxy != miny) aets.push_back(MakeAETwithPoints(mid, max, pmid, pmax));
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
	return aet.zFrom * (1 - ratio) + aet.zTo * (ratio);
}

float3 colorInterp(AET aet)
{
	float ratio = ((float)(aet.y - aet.yStart)) / (aet.maxY - aet.yStart);
	return lerp(aet.colorF, aet.colorT, ratio);
}



float3 decodeColor(int color)
{
	return float3((color >> 16) & 255, (color >> 8) & 255, color & 255);
}

point pointInterp(AET aet)
{
	float ratio = ((float)(aet.y - aet.yStart)) / (aet.maxY - aet.yStart);
	point inter = { lerp(aet.from.coordinates, aet.to.coordinates, ratio), lerp(aet.from.normal, aet.to.normal, ratio) };
	return inter;
}



