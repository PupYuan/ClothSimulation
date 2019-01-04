#pragma once
#include "Particles.h"

class Constraint {
private:
	float rest_distance;
public:
	Particle *p1, *p2;
	Constraint(Particle *p1, Particle *p2):p1(p1),p2(p2) {
		Vec3 vec = p1->getPos() - p2->getPos();
		rest_distance = vec.length();
	}

	void satisfyConstraint() {
		Vec3 p1_to_p2 = p2->getPos() - p1->getPos();
		float current_distance = p1_to_p2.length();
		Vec3 correctionVectorHalf = p1_to_p2 * (1 - rest_distance / current_distance)*0.5;
		
		p1->offsetPos(correctionVectorHalf); // correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
		p2->offsetPos(-correctionVectorHalf); // we must move p2 the negative direction of correctionVectorHalf since it points from p2 to p1, and not p1 to p2.	

	}
};
