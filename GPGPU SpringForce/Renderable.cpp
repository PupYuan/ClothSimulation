#include <ClothSimulation\Renderable.h>

void Renderable::render()
{
	// don't forget to enable shader before setting uniforms
	shader->use();
	shader->setVec3("light.position", scene->light.lightPos);
	shader->setVec3("viewPos", scene->camera.Position);

	shader->setVec3("light.ambient", scene->light.ambientColor);
	shader->setVec3("light.diffuse", scene->light.diffuseColor);
	shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	shader->setFloat("material.shininess", 32.0f);

	// view/projection transformations
	scene->projection = glm::perspective(glm::radians(scene->camera.Zoom), (float)scene->SCR_WIDTH / (float)scene->SCR_HEIGHT, 0.1f, 100.0f);
	scene->view = scene->camera.GetViewMatrix();
	shader->setMat4("projection", scene->projection);
	shader->setMat4("view", scene->view);

	glm::mat4 modelMat;
	modelMat = glm::translate(modelMat, pos);
	modelMat = glm::scale(modelMat, scale);
	shader->setMat4("model", modelMat);
	model->Draw(*shader);
}

void Plane::render()
{
	// don't forget to enable shader before setting uniforms
	shader->use();
	shader->setVec3("light.position", scene->light.lightPos);
	shader->setVec3("viewPos", scene->camera.Position);

	shader->setVec3("light.ambient", scene->light.ambientColor);
	shader->setVec3("light.diffuse", scene->light.diffuseColor);
	shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	shader->setFloat("material.shininess", 32.0f);

	// view/projection transformations
	scene->projection = glm::perspective(glm::radians(scene->camera.Zoom), (float)scene->SCR_WIDTH / (float)scene->SCR_HEIGHT, 0.1f, 100.0f);
	scene->view = scene->camera.GetViewMatrix();
	shader->setMat4("projection", scene->projection);
	shader->setMat4("view", scene->view);

	glm::mat4 modelMat;
	modelMat = glm::translate(modelMat, pos);
	modelMat = glm::scale(modelMat, scale);
	shader->setMat4("model", modelMat);

	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	// bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, woodTexture);

	// Floor
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

GLfloat planeVertices[] = {
	// Positions          // Normals         // Texture Coords
	25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
	-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
	-25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

	25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
	25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
	-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f
};

Plane::Plane(SceneManager* _scene) {
	scene = _scene;
	shader = ResourcesManager::loadShader("lightingMaps", "lightingMaps.vs", "lightingMaps.fs");
	// Setup plane VAO
	GLuint planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);

	// Load textures
	woodTexture = loadTexture("../resource/textures/marble.jpg");
	shader->use();
	shader->setInt("material.diffuse", 0);
	shader->setInt("material.specular", 1);
}


