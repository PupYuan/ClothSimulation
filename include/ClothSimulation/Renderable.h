#pragma once
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <ClothSimulation\SceneManager.h>

//�ڳ����п���Ⱦ��ʵ��
class Renderable {
protected:
	//��ʱ��Model����
	Model *model;
	Shader *shader;
	SceneManager* scene;
	glm::mat4 modelMat;
public:
	void render();
	void translate(glm::vec3 delta) {
		glm::mat4 originMat;
		originMat = glm::translate(originMat, delta);
		modelMat = originMat;
	}
};

class ShpereRenderable:public Renderable {
public:
	ShpereRenderable(SceneManager* _scene) {
		scene = _scene;
		shader = ResourcesManager::loadShader("lightingMaps", "../Resource/Shader/lightingMaps.vs", "../Resource/Shader/lightingMaps.fs");

		model = new Model("../Resource/Model/Sphere-Bot_Basic/Armature_001-(FBX 7.4 binary mit Animation).FBX");
	}
};