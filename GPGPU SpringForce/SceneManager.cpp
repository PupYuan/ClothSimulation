#include <ClothSimulation\SceneManager.h>
Camera SceneManager::camera(glm::vec3(0.0, 0.0, 6.0f));//¶¨Òå

void SceneManager::CreateCloth()
{
	cloth = new Cloth(4, 4, 21, 21);
	cloth->SetScene(this);
}

void SceneManager::SceneInit() {
	// build and compile shaders
		// -------------------------
	ourShader = ResourcesManager::loadShader("lightingMaps", "../Resource/Shader/lightingMaps.vs", "../Resource/Shader/lightingMaps.fs");

	// load models
	// -----------
	ourModel = new Model("../Resource/Model/Sphere-Bot_Basic/Armature_001-(FBX 7.4 binary mit Animation).FBX");
	CreateCloth();

	ball_pos = vec3(2.0f, -4.0f, cos(ball_time / 50.0) *2.0f);
	ball_collider = new SphereCollider(ball_pos, 1.2f);

	diffuseMap = loadTexture("../Resource/Textures/ClothTextures.png");
}

void SceneManager::RenderScene() {
	// don't forget to enable shader before setting uniforms
	ourShader->use();
	ourShader->setVec3("light.position", light.lightPos);
	ourShader->setVec3("viewPos", camera.Position);

	ourShader->setVec3("light.ambient", light.ambientColor);
	ourShader->setVec3("light.diffuse", light.diffuseColor);
	ourShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	ourShader->setFloat("material.shininess", 32.0f);


	// view/projection transformations
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	ourShader->setMat4("projection", projection);
	ourShader->setMat4("view", view);

	// render the loaded model
	glm::mat4 model;
	model = glm::translate(model, ball_pos); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	ourShader->setMat4("model", model);
	ourModel->Draw(*ourShader);

	cloth->drawShaded();
}