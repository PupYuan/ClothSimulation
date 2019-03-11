#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Scene.h"

class WindowsManager {
private:
	static WindowsManager *instance;
	WindowsManager() {
		Init();
	};
	int Init();
	GLFWwindow *window;
	Scene* mScene;
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;
public:
	static WindowsManager* Instance();
	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;
	bool firstMouse = true;

	//Function--------------------------------------------
	bool windowsShouldClose() {
		return glfwWindowShouldClose(window);
	}
	void processInput();
	void Draw();
	void SetUpScene(Scene * _scene);
	Scene* GetScene() {
		return mScene;
	}
};



