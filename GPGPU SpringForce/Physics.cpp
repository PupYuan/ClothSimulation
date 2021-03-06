#include <ClothSimulation\SceneManager.h>
#include <ClothSimulation\Renderable.h>

void SceneManager::Simulation(float dt) {
	ball_time++;
	auto shperePos_iter = spherePos.begin();
	//不移动
	//for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++){
	//	if ((*iter)->collider) {
	//		if (!(*iter)->collider->kinematicStatic) {
	//			vec3 CurPos = (*iter)->collider->getCenter();
	//			CurPos.z = 2.0f + cos(ball_time / 50.0) *3.0f;
	//			//CurPos.y = 2.0f + cos(ball_time / 50.0) *3.0f;
	//			(*iter)->collider->setPos(CurPos);
	//			*shperePos_iter = CurPos;
	//			shperePos_iter++;
	//		}
	//	}
	//}
	for (auto iter2 = simulateList.begin(); iter2 != simulateList.end(); iter2++) {
		(*iter2)->timeStep(timeStep);
		for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++) {
			if ((*iter)->collider) {
				(*iter)->collider->ClothCollisionSimulate(*iter2);
			}
		}
	}
	
}

void SceneManager::BeginPhysicsSimulation() {
	//等到所有都加载完成之后
	float newTime = (float)glfwGetTime();
	lastFrameTime = newTime;
}
void SceneManager::StepPhysics() {
	//物理模拟有自己单独的模拟频率
	float newTime = (float)glfwGetTime();
	deltaTime = newTime - lastFrameTime;
	lastFrameTime = newTime;
	accumulator += deltaTime;

	//StepPhycis调用频率和渲染一致，若渲染过慢，则两次渲染帧之间可能有多次物理模拟
	//若渲染很快，则可能accumulator
	while (accumulator >= timeStep) {
		Simulation(timeStep);
		accumulator -= timeStep;
	}

	////布料太慢，参数不好调，暂时这样
	//for (auto iter2 = simulateList.begin(); iter2 != simulateList.end(); iter2++) {
	//	(*iter2)->timeStep(timeStep);
	//	for (auto iter = renderableList.begin(); iter != renderableList.end(); iter++) {
	//		if ((*iter)->collider) {
	//			(*iter)->collider->ClothCollisionSimulate(*iter2);
	//		}
	//	}
	//}
}