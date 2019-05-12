#pragma once
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <ClothSimulation\SceneManager.h>

//在场景中可渲染的实体
class Renderable {
protected:
	//暂时用Model代替
	Model *model;
	Shader *shader;
	SceneManager* scene;
public:
	glm::vec3 pos = vec3(0,0,0);
	glm::vec3 scale = vec3(1,1,1);
	virtual void render();
	Collider* collider;
};

class ShpereRenderable:public Renderable {
public:
	ShpereRenderable(SceneManager* _scene) {
		scene = _scene;
		shader = ResourcesManager::loadShader("lightingMaps", "lightingMaps.vs", "lightingMaps.fs");

		model = new Model("../Resource/Model/Sphere-Bot_Basic/Armature_001-(FBX 7.4 binary mit Animation).FBX");
	}
};

class Plane :public Renderable {
public:
	
	GLuint planeVAO;
	GLuint woodTexture;
	
	Plane(SceneManager * _scene);
	virtual void render();
};