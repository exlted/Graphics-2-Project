#include "pch.h"
#include <vector>

#pragma once

struct VertexPTN
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 UV;
	DirectX::XMFLOAT3 norm;
};

inline VertexPTN* LoadModel(char* fileName)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<DirectX::XMFLOAT3> verts;
	std::vector<DirectX::XMFLOAT3> uvs;
	std::vector<DirectX::XMFLOAT3> norms;
	FILE * file;
	fopen_s(&file, fileName, "r");
	if(file == NULL)
	{
		printf("Impossible to open the file\n");
		return false;
	}
	while(true)
	{
		char lineHeader[128];
		int res = fscanf_s(file, "%s ", lineHeader, 128);
		if (res == EOF)
			break;
		if(strcmp(lineHeader, "v") == 0)
		{
			DirectX::XMFLOAT3 vert;
			fscanf_s(file, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
			verts.push_back(vert);
		}
		else if(strcmp(lineHeader, "vt") == 0)
		{
			DirectX::XMFLOAT3 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			uvs.push_back(uv);
		}
		else if(strcmp(lineHeader, "vn") == 0)
		{
			DirectX::XMFLOAT3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			norms.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	VertexPTN* rv = new VertexPTN[vertexIndices.size()];
	for(unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normIndex = normalIndices[i];
		rv[i].pos = verts[vertexIndex - 1];
		rv[i].norm = norms[normIndex - 1];
		rv[i].pos = uvs[uvIndex - 1];
	}
	return rv;
}