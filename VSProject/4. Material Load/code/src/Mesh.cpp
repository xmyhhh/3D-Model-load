#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Material.h"
#include "Model.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

using namespace std;

Mesh::Mesh(aiMesh* mesh) {
	//model pointer


	//mVertices mNormals mTextureCoords 
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;

		//retreive positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		//retreive normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		//retreive texture coordinates
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	//mFaces
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//mBones
	for (unsigned int i = 0; i < mesh->mNumBones; i++) {
		aiBone* mBone = mesh->mBones[i];
		BoneInfo boneInfo;
		bones.push_back(BoneInfo(mBone));

		//add weight to vertices
		for (unsigned int n = 0; n < mBone->mNumWeights; n++) {

			float weight = mBone->mWeights[n].mWeight;
			unsigned int vid = mBone->mWeights[n].mVertexId;
			vertices[vid].AddBoneData(i, weight);

		}
	}


	//materialIndex
	materialIndex = mesh->mMaterialIndex;
}

void Mesh::initDraw()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers

	/*
	layout(location = 0) in vec3 aPos;
	layout(location = 1) in vec3 aNormal;
	layout(location = 2) in vec2 aTexCoords;
	layout(location = 3) in ivec4 BoneIDs;
	layout(location = 4) in vec4 Weights;
	*/
	// vertex Positions   
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	//// vertex tangent
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	//// vertex bitangent
	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	// ids
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

	// weights
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));


	glBindVertexArray(0);

	drawInit = true;
}

void Mesh::shaderSetBone(Shader shader) {
	for (unsigned int i = 0; i < bones.size(); ++i)
	{
		const std::string name = "gBones[" + std::to_string(i) + "]";
		GLuint boneTransform = glGetUniformLocation(shader.ID, name.c_str());
		glUniformMatrix4fv(boneTransform, 1, GL_FALSE, glm::value_ptr(bones[i].finalTransformation));
	}
}

void Mesh::shaderSetTexture(Shader shader) {
	//texture_diffuse
	for (unsigned int i = 0; i < pModel->materials[materialIndex].diffuseMaps.size(); i++) {
		//const std::string name = "diffuseMaps[" + std::to_string(i) + "]";
		////std::to_string(i);
		//glActiveTexture(GL_TEXTURE0 + i);
		//// now set the sampler to the correct texture unit
		//glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), i);
		//// and finally bind the texture
		//glBindTexture(GL_TEXTURE_2D, pModel->materials[materialIndex].diffuseMaps[i].id);
	}
}

void Mesh::Draw(Shader shader)
{
	if (!drawInit) {
		initDraw();
	}
	//Set bones
	shaderSetBone(shader);
	//Set texture
	//shaderSetTexture(shader);


	//texture_diffuse
	for (unsigned int i = 0; i < pModel->materials[materialIndex].diffuseMaps.size(); i++) {
		const std::string name = "diffuseMaps[" + std::to_string(i) + "]";
		//	//std::to_string(i);
		//	//glActiveTexture(GL_TEXTURE0 + i);
		//	//// now set the sampler to the correct texture unit
		//	//glUniform1i(glGetUniformLocation(shader.ID, name.c_str()), i);
		//	//// and finally bind the texture
		//	//glBindTexture(GL_TEXTURE_2D, pModel->materials[materialIndex].diffuseMaps[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}