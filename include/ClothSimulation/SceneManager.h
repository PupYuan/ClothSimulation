#pragma once
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <ClothSimulation\Collider.h>
#include "ResourcesManager.h"
#include <learnopengl/camera.h>

Shader * ourShader;
Model *ourModel;
Cloth * cloth;
Shader * ClothShader;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
float ball_time = 0;
vec3 ball_pos;
SphereCollider* ball_collider;

// camera
Camera camera(glm::vec3(0.0, 0.0, 6.0f));
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int diffuseMap;

void SceneInit() {
	// build and compile shaders
		// -------------------------
	ourShader = ResourcesManager::loadShader("lightingMaps", "../Resource/Shader/lightingMaps.vs", "../Resource/Shader/lightingMaps.fs");
	ClothShader = ResourcesManager::loadShader("ClothShader", "../Resource/Shader/Simple.vs", "../Resource/Shader/Simple.fs");

	// load models
	// -----------
	ourModel = new Model("../Resource/Model/Sphere-Bot_Basic/Armature_001-(FBX 7.4 binary mit Animation).FBX");

	cloth = new Cloth(4, 4, 10, 10);

	ball_pos = vec3(2.0f, -4.0f, cos(ball_time / 50.0) *2.0f);
	ball_collider = new SphereCollider(ball_pos, 1.2f);

	diffuseMap = loadTexture("../Resource/Textures/ClothTextures.png");
}

void RenderScene() {
	// don't forget to enable shader before setting uniforms
	ourShader->use();
	ourShader->setVec3("light.position", lightPos);
	ourShader->setVec3("viewPos", camera.Position);

	// light properties
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
	glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
	ourShader->setVec3("light.ambient", ambientColor);
	ourShader->setVec3("light.diffuse", diffuseColor);
	ourShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	ourShader->setFloat("material.shininess", 32.0f);


	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	ourShader->setMat4("projection", projection);
	ourShader->setMat4("view", view);

	// render the loaded model
	glm::mat4 model;
	model = glm::translate(model, ball_pos); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	ourShader->setMat4("model", model);
	ourModel->Draw(*ourShader);

	ClothShader->use();
	ClothShader->setMat4("projection", projection);
	ClothShader->setMat4("view", view);
	glm::mat4 model1;
	//model1 = glm::translate(model1, glm::vec3(-6.0f, 2.0f, -5.0f)); // translate it down so it's at the center of the scene
	ClothShader->setMat4("model", model1);

	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	ClothShader->setVec3("light.position", lightPos);
	ClothShader->setVec3("viewPos", camera.Position);
	ClothShader->setVec3("light.ambient", ambientColor);
	ClothShader->setVec3("light.diffuse", diffuseColor);
	ClothShader->setVec3("light.specular", 0.2f, 0.2f, 0.2f);

	ClothShader->setFloat("material.shininess", 16.0f);
	ClothShader->setVec3("material.specular", vec3(0.2f, 0.2f, 0.2f));

	cloth->drawShaded();
}
