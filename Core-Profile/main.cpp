#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ClothSimulation/Particles.h>
#include "Cloth.h"
#include <iostream>
#include "WindowsManager.h"
#include"SceneManager.h"
#include "Collider.h"

SphereCollider* ball_collider;

int Init() {
	if (WindowsInit() == -1) {
		return -1;
	}
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	ball_pos = vec3(2.0f, -4.0f, cos(ball_time / 50.0) *2.0f);
	ball_collider = new SphereCollider(ball_pos,1.2f);
	SceneInit();
	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Simulation(float dt) {
	ball_time++;
	ball_pos.z = cos(ball_time / 50.0) * 2.0f;
	ball_collider->setPos(ball_pos);

	cloth->addForce(gravity);
	cloth->timeStep(dt);
	cloth->CollisionDetection(ball_collider);
}
void StepPhysics() {
	//Using high res. counter
	QueryPerformanceCounter(&t2);
	// compute and print the elapsed time in millisec
	frameTimeQP = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	t1 = t2;
	accumulator += frameTimeQP;

	//Fixed time stepping + rendering at different fps
	if (accumulator >= timeStep)
	{
		Simulation(timeStep);
		accumulator -= timeStep;
	}
	//glfwPollEvents();
	//glutPostRedisplay();
}
void render() {
	CalcFPS(window);
	StepPhysics();
	// render
	// ------
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderScene();
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
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
		render();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}