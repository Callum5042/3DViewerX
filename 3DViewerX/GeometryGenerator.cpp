#include "GeometryGenerator.h"
#include <DirectXMath.h>

void GeometryGenerator::CreateBox(float width, float height, float depth, Mesh* mesh)
{
	SimpleVertex v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face SimpleSimpleVertex data.
	v[0] = SimpleVertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = SimpleVertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = SimpleVertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = SimpleVertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face SimpleSimpleVertex data.
	v[4] = SimpleVertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = SimpleVertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = SimpleVertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = SimpleVertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face SimpleSimpleVertex data.
	v[8] = SimpleVertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = SimpleVertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = SimpleVertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = SimpleVertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face SimpleSimpleVertex data.
	v[12] = SimpleVertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = SimpleVertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = SimpleVertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = SimpleVertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face SimpleSimpleVertex data.
	v[16] = SimpleVertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = SimpleVertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = SimpleVertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = SimpleVertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face SimpleSimpleVertex data.
	v[20] = SimpleVertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = SimpleVertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = SimpleVertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = SimpleVertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	mesh->vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	WORD i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	mesh->indices.assign(&i[0], &i[36]);
}

void GeometryGenerator::CreateSphere(float radius, WORD sliceCount, WORD stackCount, Mesh* mesh)
{
	mesh->vertices.clear();
	mesh->indices.clear();

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	SimpleVertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	SimpleVertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	mesh->vertices.push_back(topVertex);

	float phiStep = DirectX::XM_PI / stackCount;
	float thetaStep = 2.0f * DirectX::XM_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (WORD i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (WORD j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			SimpleVertex v;

			// spherical to cartesian
			v.x = radius * sinf(phi) * cosf(theta);
			v.y = radius * cosf(phi);
			v.z = radius * sinf(phi) * sinf(theta);

			// Partial derivative of P with respect to theta
			v.tx = -radius * sinf(phi) * sinf(theta);
			v.ty = 0.0f;
			v.tz = +radius * sinf(phi) * cosf(theta);

			DirectX::XMFLOAT3 tangent(v.tx, v.ty, v.tz);
			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&tangent);
			DirectX::XMStoreFloat3(&tangent, DirectX::XMVector3Normalize(T));

			DirectX::XMFLOAT3 position(v.x, v.y, v.z);
			DirectX::XMFLOAT3 normal(v.nx, v.ny, v.nz);
			DirectX::XMVECTOR p = XMLoadFloat3(&position);
			DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(p));

			v.x = position.x;
			v.y = position.y;
			v.z = position.z;

			// Partial derivative of P with respect to theta
			v.tx = tangent.x;
			v.ty = tangent.y;
			v.tz = tangent.z;

			v.nx = normal.x;
			v.ny = normal.y;
			v.nz = normal.z;

			v.u = theta / DirectX::XM_2PI;
			v.v = phi / DirectX::XM_PI;

			mesh->vertices.push_back(v);
		}
	}

	mesh->vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the SimpleVertex buffer
	// and connects the top pole to the first ring.
	//

	for (WORD i = 1; i <= sliceCount; ++i)
	{
		mesh->indices.push_back(0);
		mesh->indices.push_back(i + 1);
		mesh->indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first SimpleVertex in the first ring.
	// This is just skipping the top pole vertex.
	WORD baseIndex = 1;
	WORD ringVertexCount = sliceCount + 1;
	for (WORD i = 0; i < stackCount - 2; ++i)
	{
		for (WORD j = 0; j < sliceCount; ++j)
		{
			mesh->indices.push_back(baseIndex + i * ringVertexCount + j);
			mesh->indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			mesh->indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				  
			mesh->indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			mesh->indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			mesh->indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the SimpleVertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole SimpleVertex was added last.
	WORD southPoleIndex = (WORD)mesh->vertices.size() - 1;

	// Offset the indices to the index of the first SimpleVertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (WORD i = 0; i < sliceCount; ++i)
	{
		mesh->indices.push_back(southPoleIndex);
		mesh->indices.push_back(baseIndex + i);
		mesh->indices.push_back(baseIndex + i + 1);
	}
}