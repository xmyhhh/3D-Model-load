#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Shader.h"

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

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Mesh;
class Model
{
public:

	vector<Mesh> meshes;
	vector<Material> materials;
	vector<aiAnimation*> animations;
	string directory;
	bool gammaCorrection;
	unsigned int numTotalBones; //整个场景的Bone数
	unsigned int numTotalVetices; //整个场景的vertex数

	Model(string const& path, bool gamma = false);


	void Draw(Shader shader);

	void PlayAnimation(aiAnimation& animation, float TimeInSeconds);

private:
	const aiScene* scene;
	Assimp::Importer importer;


	void ProcessNode(aiNode* node, const aiScene* scene);



};

