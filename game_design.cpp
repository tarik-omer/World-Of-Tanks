#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "components/simple_scene.h"
#include "lab_m1/project2/game_design.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <assimp/postprocess.h>


using namespace std;

GameDesign::MeshInfo GameDesign::GetMeshInfo(string file_path)
{
	GameDesign::MeshInfo mesh_info;
	// get vertices and incides for tank body from file using assimp
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(file_path, aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return mesh_info;
	}

	aiMesh* mesh = scene->mMeshes[0];

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// process vertex positions and indices
		glm::vec3 vector;

		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;

		mesh_info.vertices.push_back(VertexFormat(vector));
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		// indices
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			mesh_info.indices.push_back(face.mIndices[j]);
	}

	mesh_info.num_vertices_per_face = mesh->mFaces[0].mNumIndices;

	return mesh_info;
}