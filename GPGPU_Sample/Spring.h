#pragma once
#include "Particles.h"

class Spring {
private:
public:
	float restDistance;
	Particle *p1, *p2;
	//ks ÊÇ spring stiffness £¬kdÊÇdamping coefficient
	float Ks = 50.0;
	float Kd = 0.25;
	Spring(Particle *p1, Particle *p2):p1(p1),p2(p2) {
		vec3 vec = p1->getPos() - p2->getPos();
		restDistance = vec.length();
	}
	Spring(){}
	void satisfySpring(float dt);
};
