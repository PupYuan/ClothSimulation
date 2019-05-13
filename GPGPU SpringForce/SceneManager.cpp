#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\Renderable.h>
Camera SceneManager::camera(glm::vec3(0.0, 0.0, 6.0f));//定义

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};


void SceneManager::CreateCloth()
{
	Cloth * cloth = new Cloth(4, 4, 130, 130);
	cloth->SetScene(this);
	simulateList.push_back(cloth);

	//GPUCloth * gpuCloth = new GPUCloth(4, 4, 90, 90);
	//gpuCloth->SetScene(this);
	//simulateList.push_back(gpuCloth);

	//ComputeShaderCloth * cCloth = new ComputeShaderCloth(4, 4, 90, 90);
	//cCloth->SetScene(this);
	//simulateList.push_back(cCloth);
}

void SceneManager::SceneInit() {

	CreateCloth();

	Renderable* renderable = new ShpereRenderable(this);
	renderable->scale = vec3(0.8f, 0.8f, 0.8f);
	renderableList.push_back(renderable);

	Renderable* renderable1 = new ShpereRenderable(this);
	renderable1->scale = vec3(0.8f, 0.8f, 0.8f);
	renderableList.push_back(renderable1);

	Renderable* plane = new Plane(this);
	plane->pos = vec3(0, -3.0f, 0);
	renderableList.push_back(plane);

	//ball_pos = vec3(0.0f, cos(ball_time / 50.0) *2.0f, 1.0f);
	//renderable->collider = new SphereCollider(vec3(0.0f, -3.0f, 2.0f + cos(ball_time / 50.0) *3.0f), 1.2f);
	renderable->collider = new SphereCollider(vec3(-1.0f, -3.5f, 1.0f), 1.0f);
	renderable1->collider = new SphereCollider(vec3(1.0f, -3.5f, 3.0f), 1.0f);
	plane->collider = new PlaneCollider(-3.0f);

	spherePos.push_back(vec3(-1.0f, -3.5f, 1.0f));
	spherePos.push_back(vec3(1.0f, -3.5f, 3.0f));
	radius.push_back(1.0f);
	radius.push_back(1.0f);
	diffuseMap = loadTexture("../Resource/Textures/ClothTextures.png");

	//初始化天空盒
	cubemapTexture = ResourcesManager::loadCubemap(faces);
	skyboxShader = ResourcesManager::loadShader("skybox","skybox.vs", "skybox.fs");
	skyboxShader->use();
	skyboxShader->setInt("skybox", 0);
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//这部分应该是物理系统的事
	
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxWorkGroup[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxWorkGroup[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxWorkGroup[2]);

}

void SceneManager::RenderSkyBox() {
	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->use();
	view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
	skyboxShader->setMat4("view", view);
	skyboxShader->setMat4("projection", projection);
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}
void SceneManager::RenderScene() {
	

	for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++) {
		if ((*iter)->collider) {
			(*iter)->pos = (*iter)->collider->getCenter();
		}
		(*iter)->render();
	}
	for (auto iter = simulateList.begin(); iter != simulateList.end(); iter++) {
		(*iter)->render();
	}

	
	
}