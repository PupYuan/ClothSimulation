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
	/*Cloth * cloth;
	GPUCloth* gpuCloth;*/
	vector<PositionBasedUnit*>simulateList;
	vector<Renderable*>renderableList;
	vector<vec3>spherePos;
	vector<float>radius;
	// lighting
	Light light;
	//Shader vao vbo
	unsigned int skyboxVAO, skyboxVBO;
	Shader *skyboxShader;
	float ball_time = 0;
	//SphereCollider* ball_collider;
	// camera
	static Camera camera;//����
	//��ͼ
	unsigned int diffuseMap;
	//������ͼ
	vector<std::string> faces
	{
		("../Resource/textures/skybox/right.jpg"),
		("../Resource/textures/skybox/left.jpg"),
		("../Resource/textures/skybox/top.jpg"),
		("../Resource/textures/skybox/bottom.jpg"),
		("../Resource/textures/skybox/front.jpg"),
		("../Resource/textures/skybox/back.jpg")
	};
	unsigned int cubemapTexture;
	// settings
	static const unsigned int SCR_WIDTH = 1600;
	static const unsigned int SCR_HEIGHT = 900;

	float timeStep = 1.0f / 50.0f;
	//Simulation
	static glm::vec3 gravity;
	double accumulator = timeStep;
	//ͳ��֡���õ���Ϣ
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
	void BeginPhysicsSimulation();
	void StepPhysics();
	//�ɳ������𴴽����壬ͬʱ�ѳ�����Ϣ��������
private:
	// timing
	float deltaTime = 0;
	float lastFrameTime = 0;
	void RenderSkyBox();
};
