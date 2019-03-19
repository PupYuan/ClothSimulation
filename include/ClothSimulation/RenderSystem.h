#pragma once
#include "SceneManager.h"
#include "RenderTarget.h"

class RenderSystem 
{
public:
	//static RenderSystem * Instance();
	static void Init();
	static void Render(SceneManager * _scene);
	static bool PolygonMode;
	static void SwitchPolygonMode();
	static RenderTarget renderTarget;
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

bool RenderSystem::PolygonMode = false;

void RenderSystem::SwitchPolygonMode() {
	if (PolygonMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	PolygonMode = !PolygonMode;
}

void RenderSystem::Init()
{
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void RenderSystem::Render(SceneManager* _scene)
{
	//äÖÈ¾ÏµÍ³
	// render
	// ------
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//äÖÈ¾Ñ­»·
	_scene->RenderScene();
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}
