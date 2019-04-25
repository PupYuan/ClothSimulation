#include <ClothSimulation\ResourcesManager.h>

Shader* ResourcesManager::loadShader(string shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	Shader *shader = new Shader(vertexPath, fragmentPath, geometryPath);
	shaderList[shaderName] = *shader;
	return &shaderList[shaderName];
}

Shader* ResourcesManager::loadComputeShader(string shaderName, const char* Path) {
	Shader *shader = new Shader(Path);
	shaderList[shaderName] = *shader;
	return &shaderList[shaderName];
}

//Ҫ��ʼ��
std::map<string, Shader> ResourcesManager::shaderList;

Shader* ResourcesManager::getShader(string shaderName)
{
	return &shaderList[shaderName];
}