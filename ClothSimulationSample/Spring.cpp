#include "Spring.h"

//Spring Force
void Spring::satisfySpring() {
	vec3 p1_to_p2 = p2->getPos() - p1->getPos();
	float currentLength = p1_to_p2.length();
	//spring forces
	vec3 fs1 = ks * p1_to_p2 / currentLength * (currentLength - restDistance);
	vec3 fs2 = -fs1;
	//damping forces
	p1->addForce(fs1);
	p2->addForce(fs2);
}