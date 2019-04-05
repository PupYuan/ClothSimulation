#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\Renderable.h>
Camera SceneManager::camera(glm::vec3(0.0, 0.0, 6.0f));//¶¨Òå

void SceneManager::CreateCloth()
{
	cloth = new Cloth(4, 6, 15, 21);
	cloth->SetScene(this);
}

void SceneManager::SceneInit() {

	CreateCloth();
	renderable = new ShpereRenderable(this);
	ball_pos = vec3(0.0f, cos(ball_time / 50.0) *2.0f, 1.0f);
	ball_collider = new SphereCollider(ball_pos, 1.2f);

	diffuseMap = loadTexture("../Resource/Textures/ClothTextures.png");
}

void SceneManager::RenderScene() {
	renderable->translate(ball_pos);
	renderable->render();

	cloth->drawShaded();
}