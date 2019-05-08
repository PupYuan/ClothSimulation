#pragma once
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <string>
#include <map>
using namespace std;

class ResourcesManager {
public:
	static map<string, Shader>shaderList;
	static Shader* loadShader(string shaderName, string vertexPath, string fragmentPath, string geometryPath = "");
	static Shader* loadComputeShader(string shaderName, string Path);
	static Shader* getShader(string shaderName);
	static string shaderLocation;
};



