#pragma once
#include <ClothSimulation/Particles.h>
#define PI 3.1415926536f
class Constraint {
public:
	virtual void satisfyConstraint(float dt);
	float k;
	float k_prime;
	static const size_t solver_iterations = 2; //number of solver iterations per step. PBD 
};

class DistanceConstraint :public Constraint 
{
public:
	float restDistance;
	Particle *particle1, *particle2;
	virtual void satisfyConstraint(float dt);
};

class BendingConstraint :public Constraint
{
public:
	BendingConstraint(Particle *_particle1, Particle *_particle2, Particle *_particle3, Particle *_particle4,float _k = 1):
	k(_k),particle1(_particle1),particle2(_particle2),particle3(_particle3),particle4(_particle4)
	{
		phi0 = GetDihedralAngle();
		k_prime = 1.0f - pow((1.0f - k), 1.0f / solver_iterations);  //1.0f-pow((1.0f-c.k), 1.0f/ns);
		if (k_prime > 1.0)
			k_prime = 1.0;
	}
	Particle *particle1, *particle2, *particle3, *particle4;
	float phi0;//相邻的两个三角形的初始夹角
	float k = 1;//stifness
	virtual void satisfyConstraint(float dt);
	inline glm::vec3 GetNormal(vec3 p1,vec3 p2,vec3 p3) {
		glm::vec3 e1 = p1 - p2;
		glm::vec3 e2 = p3 - p2;
		return glm::normalize(glm::cross(e1, e2));
	}
	inline float GetDihedralAngle() {
		vec3 n1 = GetNormal(particle1->getPos(), particle2->getPos(), particle3->getPos());
		vec3 n2 = GetNormal(particle1->getPos(), particle2->getPos(), particle4->getPos());
		float d = glm::dot(n1, n2);
		return acos(d);
	}
};