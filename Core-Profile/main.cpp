#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ClothSimulation/Particles.h>
#include "Cloth.h"
#include <iostream>
#include "WindowsManager.h"
#include "Physics.h"
#include "RenderSystem.h"


int Init() {
	if (WindowsInit() == -1) {
		return -1;
	}
	RenderSystem::Init();
	SceneInit();
}


void tick() {
	//ͳ��֡��
	CalcFPS(window);

	//����ϵͳ
	StepPhysics();

	//��Ⱦϵͳ
	RenderSystem::Render();
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