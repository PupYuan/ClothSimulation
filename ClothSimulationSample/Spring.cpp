#include "Spring.h"

#define Ks 0.0575f
#define Kd -0.25f
//Spring Force
void Spring::satisfySpring(float dt) {
	//vec3 p1_to_p2 = p2->getPos() - p1->getPos();
	//vec3 v1_to_v2 = p2->getVelocity() - p1->getVelocity();
	//float currentLength = p1_to_p2.length();
	////spring forces
	//vec3 fs = ks * normalize(p1_to_p2) * (currentLength - restDistance);
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