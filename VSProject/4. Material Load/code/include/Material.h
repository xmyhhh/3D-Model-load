#pragma once
#include "assimp/scene.h"
#include <iostream>
#include <map>
#include <vector>
using std::string;
using std::map;
using std::vector;
struct Texture {
	unsigned int id;
	string type;
	string path;
};
class Material {
public:
	vector<Texture> diffuseMaps;
	vector<Texture> specularMaps;
	std::vector<Texture> normalMaps;
	std::vector<Texture> heightMaps;
	Material(aiMaterial* pMaterial, const string& directory);

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const string& directory, string typeName);

	unsigned int TextureFromFile(const char* path, const string& directory);
};