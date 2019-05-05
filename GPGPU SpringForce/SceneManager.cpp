#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\Renderable.h>
Camera SceneManager::camera(glm::vec3(0.0, 0.0, 6.0f));//¶¨Òå

void SceneManager::CreateCloth()
{
	//Cloth * cloth = new Cloth(4, 4, 20, 20);
	//cloth->SetScene(this);
	//simulateList.push_back(cloth);

	//GPUCloth * gpuCloth = new GPUCloth(4, 6, 20, 20);
	//gpuCloth->SetScene(this);
	//simulateList.push_back(gpuCloth);

	ComputeShaderCloth * cCloth = new ComputeShaderCloth(4, 6, 20, 20);
	cCloth->SetScene(this);
	simulateList.push_back(cCloth);
}

void SceneManager::SceneInit() {

	CreateCloth();

	Renderable* renderable = new ShpereRenderable(this);
	renderable->scale = vec3(1.0,1.0,1.0);
	renderableList.push_back(renderable);

	Renderable* renderable1 = new ShpereRenderable(this);
	renderable1->scale = vec3(1.0, 1.0, 1.0);
	renderableList.push_back(renderable1);

	//ball_pos = vec3(0.0f, cos(ball_time / 50.0) *2.0f, 1.0f);
	renderable->ball_collider = new SphereCollider(vec3(0.0f, -4.0f, 2.0f + cos(ball_time / 50.0) *3.0f), 1.2f);
	renderable1->ball_collider = new SphereCollider(vec3(0.0f, -4.0f, 2.0f + cos(ball_time / 50.0) *3.0f), 1.2f);

	spherePos.push_back(vec3(0.0f, -4.0f, 2.0f + cos(ball_time / 50.0) *3.0f));
	spherePos.push_back(vec3(0.0f, -4.0f, 2.0f + cos(ball_time / 50.0) *3.0f));
	radius.push_back(1.2f);
	radius.push_back(1.2f);
	diffuseMap = loadTexture("../Resource/Textures/ClothTextures.png");

}

void SceneManager::RenderScene() {
	for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++) {
		(*iter)->pos = (*iter)->ball_collider->getCenter();
		(*iter)->render();
	}
	for (auto iter = simulateList.begin(); iter != simulateList.end(); iter++) {
		(*iter)->render();
	}
}