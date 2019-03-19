#pragma once
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <string>
#include <map>
using namespace std;

class ResourcesManager {
public:
	static map<string, Shader>shaderList;
	static Shader* loadShader(string shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	static Shader* getShader(string shaderName);
};

