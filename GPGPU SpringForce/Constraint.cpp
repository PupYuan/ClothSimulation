#include <ClothSimulation\Constraint.h>

void DistanceConstraint::satisfyConstraint(float dt)
{
	float d = restDistance;
	vec3 p1 = particle1->getPos();
	vec3 p2 = particle2->getPos();
	float w1 = 1 / particle1->getMass();
	float w2 = 1 / particle2->getMass();

	float distanceP1P2 = distance(p1, p2)-d;
	if (distanceP1P2 == 0)
		return;
	vec3 deltaP1 = (-w1) / (w1 + w2) * (distanceP1P2)*(p1 - p2) / (distance(p1, p2));
	vec3 deltaP2 = (w1) / (w1 + w2) * (distanceP1P2)*(p1 - p2) / (distance(p1, p2));

	particle1->offsetPos(deltaP1);
	particle2->offsetPos(deltaP2);
}

void Constraint::satisfyConstraint(float dt)
{
}
