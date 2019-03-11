#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>

class Scene {
private:
	// camera
	Camera *camera;
	// lighting
	glm::vec3 lightPos;
	Shader * ourShader;
	Model *ourModel;
public:
	Scene() {
		camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		// build and compile shaders
	// -------------------------
		ourShader = new Shader("./Shader/lightingMaps.vs", "./Shader/lightingMaps.fs");

		// load models
		// -----------
		ourModel = new Model("./Model/Sphere-Bot_Basic/Armature_001-(FBX 7.4 binary mit Animation).FBX");

		lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	}
	Camera* GetCamera() {
		return camera;
	}

	void Draw();
	
};
