#include "pch.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>

#pragma once

struct VertexPTN
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 norm;
	DirectX::XMFLOAT3 UV;
};

struct ModelReturn
{
	VertexPTN* vert;
	uint16_t *indices;
	unsigned int numVerts;
	unsigned int numIndices;
	wchar_t* texture;
};

// L"ok.obj"

inline ModelReturn LoadModel(char* fileName)
{
	ModelReturn rv1;
	rv1.numVerts = 0;
	rv1.numIndices = 0;
	rv1.texture = new wchar_t[128];
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<DirectX::XMFLOAT3> verts;
	std::vector<DirectX::XMFLOAT3> uvs;
	std::vector<DirectX::XMFLOAT3> norms;
	std::map<const long long, int> uniqueVerts;
	FILE * file;
	fopen_s(&file, fileName, "r");
	if (file == NULL)
	{
		printf("Impossible to open the file\n");
		return rv1;
	}
	while (true)
	{
		char lineHeader[128];
		int res = fscanf_s(file, "%s ", lineHeader, 128);
		if (res == EOF)
			break;
		if (strcmp(lineHeader, "v") == 0)
		{
			DirectX::XMFLOAT3 vert;
			fscanf_s(file, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
			vert.z *= -1.0f;
			verts.push_back(vert);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			DirectX::XMFLOAT3 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			uv.y *= -1;
			uv.y += 1;
			uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			DirectX::XMFLOAT3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			norms.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);
			//if (matches != 9) {
			//	printf("File can't be read by our simple parser : ( Try exporting with other options\n");
			//	return rv1;
			//}
			if(matches == 9)
			{
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
			if(matches == 12)
			{
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[2]);
				vertexIndices.push_back(vertexIndex[3]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[2]);
				uvIndices.push_back(uvIndex[3]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[2]);
				normalIndices.push_back(normalIndex[3]);
			}
		}
		else if(strcmp(lineHeader, "mtllib") == 0)
		{
			FILE *file2;
			std::string loc = "Assets\\";
			fscanf_s(file, "%s ", lineHeader, 128);
			loc += lineHeader;
			fopen_s(&file2, loc.c_str(), "r");
			if(file2 != NULL)
			{
				while(true)
				{
					res = fscanf_s(file2, "%s ", lineHeader, 128);
					if(strcmp(lineHeader, "map_Kd") == 0)
					{
						fscanf_s(file2, "%s ", lineHeader, 128);
						std::string loc = "Assets\\";
						loc += lineHeader;
						size_t i;
						mbstowcs_s(&i, rv1.texture, 128, loc.c_str(), loc.size() + 1);

						break;
					}
					else if(res == 0)
					{
						break;
					}
				}
			}
		}
	}
	rv1.vert = new VertexPTN[vertexIndices.size()];
	rv1.indices = new uint16_t[vertexIndices.size()];
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normIndex = normalIndices[i];

		std::string keyString = std::to_string(vertexIndex) + '0' + std::to_string(uvIndex) + '0' + std::to_string(normIndex);
		long long Key = std::stoll(keyString, nullptr);

		if(uniqueVerts.find(Key) != uniqueVerts.end())
		{
			rv1.indices[i] = uniqueVerts[Key];
			rv1.numIndices++;
			continue;
		}
		rv1.numVerts += 1;
		rv1.numIndices++;
		rv1.indices[i] = rv1.numVerts - 1;
		uniqueVerts.emplace(Key, rv1.numVerts - 1);
		rv1.vert[rv1.numVerts - 1].pos = verts[vertexIndex - 1];
		rv1.vert[rv1.numVerts - 1].norm = norms[normIndex - 1];
		rv1.vert[rv1.numVerts - 1].UV = uvs[uvIndex - 1];
	}
	return rv1;
}