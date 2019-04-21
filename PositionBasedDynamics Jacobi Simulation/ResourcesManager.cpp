#include <ClothSimulation\ResourcesManager.h>

Shader* ResourcesManager::loadShader(string shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	Shader *shader = new Shader(vertexPath, fragmentPath, geometryPath);
	shaderList[shaderName] = *shader;
	return &shaderList[shaderName];
}

//Òª³õÊ¼»¯
std::map<string, Shader> ResourcesManager::shaderList;

Shader* ResourcesManager::getShader(string shaderName)
{
	return &shaderList[shaderName];
}