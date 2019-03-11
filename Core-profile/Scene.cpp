#include "Scene.h"
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
void Scene::Draw()
{
	// render
	// ------
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// don't forget to enable shader before setting uniforms
	ourShader->use();
	ourShader->setVec3("light.position", lightPos);
	ourShader->setVec3("viewPos", camera->Position);

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
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	ourShader->setMat4("projection", projection);
	ourShader->setMat4("view", view);

	// render the loaded model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	ourShader->setMat4("model", model);
	ourModel->Draw(*ourShader);
}
