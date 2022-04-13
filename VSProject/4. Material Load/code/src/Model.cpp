#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "Animation.h"
#include "Material.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

using std::string;

Model::Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
{
	scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);


	//Load animations
	for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* pAnimation = scene->mAnimations[i];

		animations.push_back(pAnimation);
	}

	//Load material
	for (int i = 0; i < scene->mNumMaterials; i++) {
		materials.push_back(Material(scene->mMaterials[i], directory));
	}




	//init other data
	numTotalBones = 0;
	numTotalVetices = 0;
	for (int i = 0; i < meshes.size(); i++) {
		numTotalBones += meshes[i].bones.size();
		numTotalVetices += meshes[i].vertices.size();
	}

}

void Model::Draw(Shader shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::PlayAnimation(aiAnimation& animation, float TimeInSeconds)
{

	unsigned int numPosKeys = animation.mChannels[0]->mNumPositionKeys;

	float TicksPerSecond = scene->mAnimations[0]->mTicksPerSecond != 0 ?
		scene->mAnimations[0]->mTicksPerSecond : 25.0f;

	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, scene->mAnimations[0]->mChannels[0]->mPositionKeys[numPosKeys - 1].mTime);

	Animation::SetBoneTransform(animation, AnimationTime, meshes, scene->mRootNode, glm::mat4(1.0f));  //修改 每个BoneInfo,需要把aiNode的层次关系传进去

}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		meshes.push_back(Mesh(mesh));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}


