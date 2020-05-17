#pragma once

#include "Mesh.h"

namespace GeometryGenerator
{
	void CreateBox(float width, float height, float depth, Mesh* mesh);

	void CreateSphere(float radius, WORD sliceCount, WORD stackCount, Mesh* mesh);
}