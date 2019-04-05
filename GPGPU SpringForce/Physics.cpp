#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\Renderable.h>

void SceneManager::Simulation(float dt) {
	ball_time++;
	for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++){
		vec3 CurPos = (*iter)->ball_collider->getCenter();
		CurPos.y = cos(ball_time / 50.0) *2.0f;
		(*iter)->ball_collider->setPos(CurPos);
	}
	cloth->addForce(gravity);
	cloth->timeStep(dt);
	for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++) {
		(*iter)->ball_collider->ClothCollisionSimulate(cloth);
	}
}
void SceneManager::StepPhysics() {
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