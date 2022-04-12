#pragma once
#include "assimp/scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glad/glad.h>
#include <iostream>
#include <map>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
	Material(aiMaterial* pMaterial, const string& directory) {

		// 1. diffuse maps
		diffuseMaps = Material::loadMaterialTextures(pMaterial, aiTextureType_DIFFUSE, directory, "texture_diffuse");

		// 2. specular maps
		specularMaps = Material::loadMaterialTextures(pMaterial, aiTextureType_SPECULAR, directory, "texture_specular");

		// 3. normal maps
		normalMaps = Material::loadMaterialTextures(pMaterial, aiTextureType_HEIGHT, directory, "texture_normal");

		// 4. height maps
		heightMaps = Material::loadMaterialTextures(pMaterial, aiTextureType_AMBIENT, directory, "texture_height");

	}

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const string& directory, string typeName)
	{
		vector<Texture> textures;
		unsigned int textureNum = mat->GetTextureCount(type);
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before
			bool skip = false;
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
		}
		return textures;
	}

	unsigned int TextureFromFile(const char* path, const string& directory)
	{
		string filename = string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
};