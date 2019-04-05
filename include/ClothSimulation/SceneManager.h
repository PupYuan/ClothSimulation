#pragma once
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <ClothSimulation\Collider.h>
#include "ResourcesManager.h"
#include <learnopengl/camera.h>
class Renderable;

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
	Cloth * cloth;
	Renderable* renderable;
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

	float timeStep = 1.0f / 60.0f;
	//Simulation
	glm::vec3 gravity = glm::vec3(0.0f, -0.00981f, 0.0f);
	double accumulator = timeStep;
	//统计帧率用的信息
	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	double frameTimeQP = 0;

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
