#pragma once
#include "SceneManager.h"

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
}