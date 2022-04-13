#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Material.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

using namespace std;

#define NUM_BONES_PER_VERTEX 4
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define INVALID_MATERIAL 0xFFFFFFFF

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[NUM_BONES_PER_VERTEX];
	//weights from each bone
	float m_Weights[NUM_BONES_PER_VERTEX];

	Vertex()
	{
		Reset();
	};

	void Reset()
	{
		for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; ++i)
		{
			m_BoneIDs[i] = 0;
			m_Weights[i] = 0;
		}
	}

	void AddBoneData(unsigned int BoneID, float Weight)
	{
		for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; i++) {
			if (m_Weights[i] == 0.0) {
				m_BoneIDs[i] = BoneID;
				m_Weights[i] = Weight;
				return;
			}
		}
	}
};



struct BoneInfo {
	string mName;
	glm::mat4 offset;
	glm::mat4 finalTransformation;
	BoneInfo()
	{
		mName = "";
		offset = glm::mat4(1.0f);
		finalTransformation = glm::mat4(1.0f);
	}
	BoneInfo(aiBone* mBone)
	{
		mName = string(mBone->mName.data);

		aiMatrix4x4 tp1 = mBone->mOffsetMatrix;
		offset = glm::transpose(glm::make_mat4(&tp1.a1));

		finalTransformation = glm::mat4(1.0f);
	}
};


class Mesh {
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	int materialIndex;
	vector<BoneInfo> bones;

	// constructor
	Mesh(aiMesh* mesh);

	// render the mesh
	void Draw(Shader shader);

private:
	/*  Render data  */
	unsigned int VAO, VBO, EBO;

	bool drawInit = false;
	void initDraw();

	void shaderSetBone(Shader shader);
	void shaderSetTexture(Shader shader);
};

