#pragma once
#include <ClothSimulation/Particles.h>

class Spring {
private:
public:
	float restDistance;
	Particle *p1, *p2;
	//ks �� spring stiffness ��kd��damping coefficient
	float Ks;
	float Kd;
	Spring(Particle *p1, Particle *p2) :p1(p1), p2(p2) {
		vec3 vec = p1->getPos() - p2->getPos();
		restDistance = vec.length();
	}
	Spring() {}
	void satisfySpring(float dt);
};
