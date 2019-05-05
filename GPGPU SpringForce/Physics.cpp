#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\Renderable.h>

void SceneManager::Simulation(float dt) {
	ball_time++;
	auto shperePos_iter = spherePos.begin();
	for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++, shperePos_iter++){
		vec3 CurPos = (*iter)->ball_collider->getCenter();
		CurPos.z = 2.0f + cos(ball_time / 50.0) *3.0f;
		//CurPos.y = 2.0f + cos(ball_time / 50.0) *3.0f;
		(*iter)->ball_collider->setPos(CurPos);
		*shperePos_iter = CurPos;
	}

	for (auto iter2 = simulateList.begin(); iter2 != simulateList.end(); iter2++) {
		(*iter2)->timeStep(dt);
		for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++) {
			(*iter)->ball_collider->ClothCollisionSimulate(*iter2);
		}
	}
	
}

void SceneManager::BeginPhysicsSimulation() {
	//�ȵ����ж��������֮��
	float newTime = (float)glfwGetTime();
	lastFrameTime = newTime;
}
void SceneManager::StepPhysics() {
	//����ģ�����Լ�������ģ��Ƶ��
	float newTime = (float)glfwGetTime();
	deltaTime = newTime - lastFrameTime;
	lastFrameTime = newTime;
	accumulator += deltaTime;

	//StepPhycis����Ƶ�ʺ���Ⱦһ�£�����Ⱦ��������������Ⱦ֮֡������ж������ģ��
	//����Ⱦ�ܿ죬�����accumulator
	while (accumulator >= timeStep) {
		Simulation(timeStep);
		accumulator -= timeStep;
	}

	//Simulation(timeStep);
	//����̫�����������õ�����ʱ����
	/*for (auto iter2 = simulateList.begin(); iter2 != simulateList.end(); iter2++) {
		(*iter2)->timeStep(timeStep);
		for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++) {
			(*iter)->ball_collider->ClothCollisionSimulate(*iter2);
		}
	}*/
}