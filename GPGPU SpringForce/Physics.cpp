#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\Renderable.h>

void SceneManager::Simulation(float dt) {
	ball_time++;
	for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++){
		vec3 CurPos = (*iter)->ball_collider->getCenter();
		CurPos.y = cos(ball_time / 50.0) *2.0f;
		(*iter)->ball_collider->setPos(CurPos);
	}
	//cloth->addForce(gravity);
	//cloth->timeStep(dt);
	for (auto iter2 = simulateList.begin(); iter2 != simulateList.end(); iter2++) {
		(*iter2)->timeStep(dt);
		for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++) {
			(*iter)->ball_collider->ClothCollisionSimulate(*iter2);
		}
	}
	
}


void SceneManager::StepPhysics() {
	//物理模拟有自己单独的模拟频率
	float newTime = (float)glfwGetTime();
	deltaTime = newTime - lastFrameTime;
	accumulator += deltaTime;
	lastFrameTime = newTime;
	//Fixed time stepping + rendering at different fps
	if (accumulator >= timeStep)
	{
		Simulation(timeStep);
		accumulator -= timeStep;
	}
}