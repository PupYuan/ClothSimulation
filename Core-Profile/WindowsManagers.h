#pragma once
#include <GLFW/glfw3.h>

//统计帧率用的信息
int totalFrames = 0;

// timing
float deltaTime = 0;
float startTime = 0, fps = 0;
float lastFrameTime = 0;

#define MAX_PATH 100
char info[MAX_PATH] = { 0 };
void CalcFPS(GLFWwindow * windows) {
	// per-frame time logic
// --------------------
	float newTime = (float)glfwGetTime();
	deltaTime = newTime - lastFrameTime;
	lastFrameTime = newTime;

	++totalFrames;
	if ((newTime - startTime) > 1.0f) {
		float elapsedTime = (newTime - startTime);
		fps = (totalFrames / elapsedTime);
		startTime = newTime;
		totalFrames = 0;
	}

	sprintf_s(info, "FPS: %3.2f", fps);
	glfwSetWindowTitle(windows,info);
}