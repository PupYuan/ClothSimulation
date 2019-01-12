#pragma once
#include "Particles.h"

//参考用的弹簧系数
//const float DEFAULT_DAMPING = -0.0125f;
//float	KsStruct = 50.75f, KdStruct = -0.25f;
//float	KsShear = 50.75f, KdShear = -0.25f;
//float	KsBend = 50.95f, KdBend = -0.25f;
class Spring {
private:
	float restDistance;
public:
	Particle *p1, *p2;
	//ks 是 spring stiffness ，kd是damping coefficient
	float ks = 50.0;
	float kd = 0.25;
	Spring(Particle *p1, Particle *p2):p1(p1),p2(p2) {
		vec3 vec = p1->getPos() - p2->getPos();
		restDistance = vec.length();
	}

	//void satisfySpring() {
	//	vec3 p1_to_p2 = p2->getPos() - p1->getPos();
	//	float current_distance = p1_to_p2.length();
	//	vec3 correctionVectorHalf = p1_to_p2 * (1 - restDistance / current_distance)*0.5;
	//	
	//	p1->offsetPos(correctionVectorHalf); // correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the Spring.
	//	p2->offsetPos(-correctionVectorHalf); // we must move p2 the negative direction of correctionVectorHalf since it points from p2 to p1, and not p1 to p2.	
	//}

	//Spring Force
	void satisfySpring(float dt);
};
