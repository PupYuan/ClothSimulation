#include <ClothSimulation\Spring.h>

//#define Ks 80.0f
//#define Kd -2.0f
//Spring Force
void Spring::satisfySpring(float dt) {
	//两个粒子的速度
	vec3 v1 = (p1->getPos() - p1->getLastPos()) / dt;
	vec3 v2 = (p2->getPos() - p2->getLastPos()) / dt;

	vec3 deltaP = p1->getPos() - p2->getPos();
	vec3 deltaV = v1 - v2;
	float dist = length(deltaP);
	//leftTerm是弹性系数Ks*当前距离与静止距离之差，即为弹力部分
	//rightTerm是为两个粒子添加一个速度差方向上的摩擦力，并投影到它们位置向量上
	float leftTerm = -Ks * (dist - restDistance);
	float rightTerm = Kd * (dot(deltaV, deltaP) / dist);
	vec3 springForce = (leftTerm + rightTerm)*normalize(deltaP);

	p1->addForce(springForce);
	p2->addForce(-springForce);
}