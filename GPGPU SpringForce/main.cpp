#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ClothSimulation/Particles.h>
#include <ClothSimulation\Cloth.h>
#include <iostream>
#include <ClothSimulation\WindowsManager.h>
#include <ClothSimulation\Physics.h>
#include <ClothSimulation\RenderSystem.h>
#include <ClothSimulation\SceneManager.h>

SceneManager* scene;
int Init() {
	if (WindowsInit() == -1) {
		return -1;
	}
	RenderSystem::Init();
	scene = new SceneManager();
}


void tick() {
	//ͳ��֡��
	CalcFPS(window);

	//����ϵͳ
	scene->StepPhysics();

	//��Ⱦϵͳ
	RenderSystem::Render(scene);
}

int main()
{
	Init();
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// draw
		// -----
		tick();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}