#pragma once
// GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <learnopengl/camera.h>
class InputManager {
private:
	GLFWwindow* window;
	Camera &camera;
public:
	InputManager(GLFWwindow* _window, Camera& _camera)
		:window(_window), camera(_camera) 
	{
	
	}

	// Function prototypes
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void Do_Movement(GLfloat deltaTime);

	bool keys[1024];
	bool keysPressed[1024];
};


// Moves/alters the camera positions based on user input
//传入每帧花的时间
void InputManager::Do_Movement(GLfloat deltaTime)
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
// Is called whenever a key is pressed/released via GLFW
void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
			keysPressed[key] = false;
		}
	}
}

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
