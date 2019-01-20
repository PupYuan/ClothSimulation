#include "Spring.h"

#define Ks 50.75f
#define Kd -0.25f
//Spring Force
void Spring::satisfySpring(float dt) {
	////damping forces
	//vec3 fd = kd * v1_to_v2*normalize(p1_to_p2);
	vec3 v1 = (p1->getPos() - p1->getLastPos())/dt;
	vec3 v2 = (p2->getPos() - p2->getLastPos())/dt;
	
	vec3 deltaP = p1->getPos() - p2->getPos();
	vec3 deltaV = v1 - v2;
	float dist = length(deltaP);
	float leftTerm = -Ks * (dist - restDistance);
	float rightTerm = Kd * (dot(deltaV, deltaP) / dist);
	vec3 springForce = (leftTerm+ rightTerm)*normalize(deltaP);

	p1->addForce(springForce);
	p2->addForce(-springForce);


}