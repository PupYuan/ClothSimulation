#pragma once
#include "WindowsManager.h"


Scene scene;
WindowsManager *windowsMgr;

void init() {
	windowsMgr = WindowsManager::Instance();
	windowsMgr->SetUpScene(&scene);
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

int main()
{
	init();
	// render loop
	// -----------
	while (!windowsMgr->windowsShouldClose())
	{
		// input
		// -----
		windowsMgr->processInput();
		// draw
		// -----
		windowsMgr->Draw();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}