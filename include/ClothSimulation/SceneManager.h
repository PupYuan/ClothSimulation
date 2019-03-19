#pragma once
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <ClothSimulation\Collider.h>
#include "ResourcesManager.h"
#include <learnopengl/camera.h>

class Light {
public:
	glm::vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
	glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
};

class SceneManager {
public:
	glm::mat4 projection;
	glm::mat4 view;
	Shader * ourShader;
	Model *ourModel;
	Cloth * cloth;
	// lighting
	Light light;

	float ball_time = 0;
	vec3 ball_pos;
	SphereCollider* ball_collider;
	// camera
	static Camera camera;//声明
	//贴图
	unsigned int diffuseMap;
	// settings
	static const unsigned int SCR_WIDTH = 800;
	static const unsigned int SCR_HEIGHT = 600;
	//Function
	SceneManager() {
		SceneInit();
	}
	void SceneInit();
	void RenderScene();
	void CreateCloth();
	void Simulation(float dt);
	void StepPhysics();
	//由场景负责创建物体，同时把场景信息传给物体
};
