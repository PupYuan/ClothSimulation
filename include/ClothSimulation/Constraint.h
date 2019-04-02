#pragma once
#include <ClothSimulation/Particles.h>
class Constraint {
public:
	virtual void satisfyConstraint(float dt);
};

class DistanceConstraint :public Constraint 
{
public:
	float restDistance;
	Particle *particle1, *particle2;
	virtual void satisfyConstraint(float dt);
};