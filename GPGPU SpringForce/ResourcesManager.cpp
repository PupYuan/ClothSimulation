#include <ClothSimulation\ResourcesManager.h>

string ResourcesManager::shaderLocation = "../Resource/Shader/";

Shader* ResourcesManager::loadShader(string shaderName, string vertexPath, string fragmentPath, string geometryPath) {
	Shader *shader = new Shader((shaderLocation+vertexPath).c_str(), (shaderLocation+fragmentPath).c_str(), geometryPath.empty()?nullptr:(shaderLocation+geometryPath).c_str());
	shaderList[shaderName] = *shader;
	return &shaderList[shaderName];
}

Shader* ResourcesManager::loadComputeShader(string shaderName, string Path) {
	Shader *shader = new Shader((shaderLocation+Path).c_str());
	shaderList[shaderName] = *shader;
	return &shaderList[shaderName];
}

//Òª³õÊ¼»¯
std::map<string, Shader> ResourcesManager::shaderList;

Shader* ResourcesManager::getShader(string shaderName)
{
	return &shaderList[shaderName];
}