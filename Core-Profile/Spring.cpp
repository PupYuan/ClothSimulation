#include <ClothSimulation\Spring.h>

//#define Ks 80.0f
//#define Kd -2.0f
//Spring Force
void Spring::satisfySpring(float dt) {
	//�������ӵ��ٶ�
	vec3 v1 = (p1->getPos() - p1->getLastPos()) / dt;
	vec3 v2 = (p2->getPos() - p2->getLastPos()) / dt;

	vec3 deltaP = p1->getPos() - p2->getPos();
	vec3 deltaV = v1 - v2;
	float dist = length(deltaP);
	//leftTerm�ǵ���ϵ��Ks*��ǰ�����뾲ֹ����֮���Ϊ��������
	//rightTerm��Ϊ�����������һ���ٶȲ���ϵ�Ħ��������ͶӰ������λ��������
	float leftTerm = -Ks * (dist - restDistance);
	float rightTerm = Kd * (dot(deltaV, deltaP) / dist);
	vec3 springForce = (leftTerm + rightTerm)*normalize(deltaP);

	p1->addForce(springForce);
	p2->addForce(-springForce);
}