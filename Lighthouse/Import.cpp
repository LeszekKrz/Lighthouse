#include "Import.h"
#include <sstream>
#include <fstream>

using namespace std;

void ImportScene(object objects[4])
{
	objects[0] = ImportObject("ScaledLighthouse.obj");
	objects[0].color = 255 << 16 | 255;

	objects[1] = ImportObject("ScaledBoat.obj");
	objects[1].color = 255 << 16;

	objects[2] = ImportObject("ScaledIsland.obj");
	objects[2].color = 255 << 8;

	objects[3] = ImportObject("ScaledOcean.obj");
	objects[3].color = 255;
}

object ImportObject(std::string filename)
{
	ifstream readFile(filename);
	vector<float4> vertices;
	vector<float3> normals;
	vector<triangle> triangles;
	string line, value;
	while (getline(readFile, line))
	{
		stringstream stream(line);
		getline(stream, value, ' ');
		if (value == "v")
		{
			float4 vertex;
			
			getline(stream, value, ' ');
			vertex.x = stof(value);

			getline(stream, value, ' ');
			vertex.y = stof(value);
			
			getline(stream, value, ' ');
			vertex.z = stof(value);

			vertex.w = 1;

			vertices.push_back(vertex);
		}
		else if (value == "vn")
		{
			float3 vector;

			getline(stream, value, ' ');
			vector.x = stof(value);

			getline(stream, value, ' ');
			vector.y = stof(value);

			getline(stream, value, ' ');
			vector.z = stof(value);

			normals.push_back(vector);
		}

		else if (value == "f")
		{
			triangle _triangle;
			getline(stream, value, ' ');
			stringstream inds(value);
			string index;

			getline(inds, index, '/');
			float4 a = vertices.at(stoi(index) - 1);
			getline(inds, index, '/');
			getline(inds, index, '/');
			float3 na = normals.at(stoi(index) - 1);
			_triangle.vertices[0] = { a,na };

			getline(stream, value, ' ');
			inds = stringstream(value);
			
			getline(inds, index, '/');
			float4 b = vertices.at(stoi(index) - 1);
			getline(inds, index, '/');
			getline(inds, index, '/');
			float3 nb = normals.at(stoi(index) - 1);
			_triangle.vertices[1] = { b, nb };

			getline(stream, value, ' ');
			inds = stringstream(value);

			getline(inds, index, '/');
			float4 c = vertices.at(stoi(index) - 1);
			getline(inds, index, '/');
			getline(inds, index, '/');
			float3 nc = normals.at(stoi(index) - 1);
			_triangle.vertices[2] = { c,nc };

			triangles.push_back(_triangle);
		}
	}
	object _object = { triangles, 0 };
	return _object;
}
