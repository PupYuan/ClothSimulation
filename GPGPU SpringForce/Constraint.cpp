#include <ClothSimulation\Constraint.h>

void Constraint::satisfyConstraint(float dt)
{
}

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
	vec3 deltaP1 = (-w1) / (w1 + w2) * (distanceP1P2)*(p1 - p2) / (distance(p1, p2))*k_prime;
	vec3 deltaP2 = (w1) / (w1 + w2) * (distanceP1P2)*(p1 - p2) / (distance(p1, p2))*k_prime;

	particle1->offsetPos(deltaP1);
	particle2->offsetPos(deltaP2);
}



void BendingConstraint::satisfyConstraint(float dt)
{
	//Using the dihedral angle approach of the position based dynamics		
	float d = 0, phi = 0, i_d = 0;
	glm::vec3 n1 = glm::vec3(0), n2 = glm::vec3(0);

	glm::vec3 p1 = particle1->getPos();
	glm::vec3 p2 = particle2->getPos();
	glm::vec3 p3 = particle3->getPos();
	glm::vec3 p4 = particle4->getPos();

	glm::vec3 p2p3 = glm::cross(p2, p3);
	glm::vec3 p2p4 = glm::cross(p2, p4);

	float lenp2p3 = glm::length(p2p3);

	if (lenp2p3 == 0.0) { return; } //need to handle this case.

	float lenp2p4 = glm::length(p2p4);

	if (lenp2p4 == 0.0) { return; } //need to handle this case.

	n1 = glm::normalize(p2p3);
	n2 = glm::normalize(p2p4);

	d = glm::dot(n1, n2);
	phi = acos(d);

	//try to catch invalid values that will return NaN.
	// sqrt(1 - (1.0001*1.0001)) = NaN 
	// sqrt(1 - (-1.0001*-1.0001)) = NaN 
	if (d < -1.0)
		d = -1.0;
	else if (d > 1.0)
		d = 1.0; //d = clamp(d,-1.0,1.0);

	//in both case sqrt(1-d*d) will be zero and nothing will be done.
	//0?case, the triangles are facing in the opposite direction, folded together.
	if (d == -1.0) {
		phi = PI;  //acos(-1.0) == PI
		if (phi == phi0)
			return; //nothing to do 

	   //in this case one just need to push 
	   //vertices 1 and 2 in n1 and n2 directions, 
	   //so the constrain will do the work in second iterations.
		if (particle1->isMovable())
			particle3->offsetPos(n1 / 100.0f);

		if (particle2->isMovable())
			particle4->offsetPos(n2 / 100.0f);

		return;
	}
	if (d == 1.0) { //180?case, the triangles are planar
		phi = 0.0;  //acos(1.0) == 0.0
		if (phi == phi0)
			return; //nothing to do 
	}

	i_d = sqrt(1 - (d*d))*(phi - phi0);

	glm::vec3 p2n1 = glm::cross(p2, n1);
	glm::vec3 p2n2 = glm::cross(p2, n2);
	glm::vec3 p3n2 = glm::cross(p3, n2);
	glm::vec3 p4n1 = glm::cross(p4, n1);
	glm::vec3 n1p2 = -p2n1;
	glm::vec3 n2p2 = -p2n2;
	glm::vec3 n1p3 = glm::cross(n1, p3);
	glm::vec3 n2p4 = glm::cross(n2, p4);

	glm::vec3 q3 = (p2n2 + n1p2 * d) / lenp2p3;
	glm::vec3 q4 = (p2n1 + n2p2 * d) / lenp2p4;
	glm::vec3 q2 = (-(p3n2 + n1p3 * d) / lenp2p3) - ((p4n1 + n2p4 * d) / lenp2p4);

	glm::vec3 q1 = -q2 - q3 - q4;

	float q1_len2 = glm::dot(q1, q1);// glm::length(q1)*glm::length(q1);
	float q2_len2 = glm::dot(q2, q2);// glm::length(q2)*glm::length(q1);
	float q3_len2 = glm::dot(q3, q3);// glm::length(q3)*glm::length(q1);
	float q4_len2 = glm::dot(q4, q4);// glm::length(q4)*glm::length(q1); 

	float W1 = 1/(particle1->getMass());
	float W2 = 1 / (particle2->getMass());
	float W3 = 1 / (particle3->getMass());
	float W4 = 1 / (particle4->getMass());
	float sum = W1 * (q1_len2)+
		W2 * (q2_len2)+
		W3 * (q3_len2)+
		W4 * (q4_len2);

	glm::vec3 dP1 = -((W1 * i_d) / sum)*q1;
	glm::vec3 dP2 = -((W2 * i_d) / sum)*q2;
	glm::vec3 dP3 = -((W3 * i_d) / sum)*q3;
	glm::vec3 dP4 = -((W4 * i_d) / sum)*q4;

	if (W1 > 0.0) {
		particle1->offsetPos(dP1*k_prime);
	}
	if (W2 > 0.0) {
		particle2->offsetPos(dP2*k_prime);
	}
	if (W3 > 0.0) {
		particle3->offsetPos(dP3*k_prime);
	}
	if (W4 > 0.0) {
		particle4->offsetPos(dP4*k_prime);
	}
}

void BendingConstraint2::satisfyConstraint(float dt)
{
	size_t i = 0;

	//global_k is a percentage of the global dampening constant 
	float global_k = global_dampening * 0.01f;
	glm::vec3 center = 0.3333f * (particle1->getPos() + particle2->getPos() + particle3->getPos());
	glm::vec3 dir_center = particle3->getPos() - center;
	float dist_center = glm::length(dir_center);

	float W1 = 1 / (particle1->getMass());
	float W2 = 1 / (particle2->getMass());
	float W3 = 1 / (particle3->getMass());

	float diff = 1.0f - ((global_k + rest_length) / dist_center);
	glm::vec3 dir_force = dir_center * diff;
	glm::vec3 fa = k_prime * ((2.0f*W1) / w) * dir_force;
	glm::vec3 fb =k_prime * ((2.0f*W2) /w) * dir_force;
	glm::vec3 fc = -k_prime * ((4.0f*W3) / w) * dir_force;

	if (W1 > 0.0) {
		particle1->offsetPos(fa);
	}
	if (W2 > 0.0) {
		particle2->offsetPos(fb);
	}
	if (W3 > 0.0) {
		particle3->offsetPos(fb);
	}
}
