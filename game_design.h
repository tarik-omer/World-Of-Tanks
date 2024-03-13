#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

#include <vector>
#include <string>
#include <iostream>

#include "components/simple_scene.h"

using namespace std;

namespace GameDesign
{
	typedef struct MeshInfo
	{
		vector<VertexFormat> vertices;
		vector<unsigned int> indices;
		int num_vertices_per_face = -1;
	} MeshInfo;

	MeshInfo GetMeshInfo(string file_path);

}   // namespace game_design
