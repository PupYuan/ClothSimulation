#include "ResourceManager.h"

#include <iostream>

// Instantiate static variables
std::map<std::string, Model>    ResourceManager::Models;
std::map<std::string, Shader>       ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name)
{
	Shaders[name] = Shader(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Model ResourceManager::LoadModel(const GLchar * file, std::string name)
{
	Models[name] = Model(file);
	return Models[name];
}

Model ResourceManager::GetModel(std::string name)
{
	return Models[name];
}

void ResourceManager::Clear()
{
	// (Properly) delete all shaders	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	// (Properly) delete all textures
	for (auto iter : Models);
		//glDeleteTextures(1, &iter.second.ID);
}


