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
