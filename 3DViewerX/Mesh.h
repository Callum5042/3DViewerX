#pragma once

#include <vector>
#include <string>
#include <d3d11_4.h>
typedef unsigned short WORD;

struct SimpleVertex
{
	SimpleVertex() {}
	SimpleVertex(float x, float y, float z) : x(x), y(y), z(z) {}
	SimpleVertex(float x, float y, float z,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v) : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz), tx(tx), ty(ty), tz(tz), u(u), v(v)
	{}

	float x;
	float y;
	float z;

	float nx = 0;
	float ny = 0;
	float nz = 0;

	float u = 0;
	float v = 0;

	float tx = 0;
	float ty = 0;
	float tz = 0;

	float bx = 0;
	float by = 0;
	float bz = 0;
};

struct Mesh
{
	std::vector<SimpleVertex> vertices;
	std::vector<WORD> indices;

	int startIndex = 0;
	int startVertex = 0;
	std::string name;

	ID3D11ShaderResourceView* m_DiffuseMapSRV = nullptr;
	ID3D11ShaderResourceView* m_NormalMapSRV = nullptr;
};