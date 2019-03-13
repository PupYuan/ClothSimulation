#pragma once
#include "SceneManager.h"

class RenderSystem 
{
public:
	//static RenderSystem * Instance();
	static void Init();
	static void Render();
private:
	static RenderSystem* instance;
	//RenderSystem() {

	//}
};

//RenderSystem * RenderSystem::Instance() {
//	if (instance == nullptr) {
//		instance = new RenderSystem();
//	}
//	return instance;
//}

void RenderSystem::Init()
{
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void RenderSystem::Render()
{
	//‰÷»æœµÕ≥
	// render
	// ------
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//‰÷»æ—≠ª∑
	RenderScene();
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}
