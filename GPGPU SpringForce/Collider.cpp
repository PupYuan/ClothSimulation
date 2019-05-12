#include <ClothSimulation\Collider.h>

void SphereCollider::ClothCollisionSimulate(PositionBasedUnit* c)
{
	std::vector<Particle>* particles =  & (c->getParticles());
	std::vector<Particle>::iterator particle;
	//�ж�particle�Ƿ��ڴ���ײ����
	for (particle = particles->begin(); particle != particles->end(); particle++)
	{
		vec3 v = (*particle).getPos() - center;
		//int temp = v.length();
		//v.length()�����⡣v.length���ص���������ά��
		if (length(v) < radius) {
			vec3 moveOffset = normalize(v) * (radius - length(v));
			particle->offsetPos(moveOffset);
		}
	}
}

void Collider::ClothCollisionSimulate(PositionBasedUnit *c)
{

}

void PlaneCollider::ClothCollisionSimulate(PositionBasedUnit * c)
{
	std::vector<Particle>* particles = &(c->getParticles());
	std::vector<Particle>::iterator particle;
	//�ж�particle�Ƿ��ڴ���ײ����
	for (particle = particles->begin(); particle != particles->end(); particle++)
	{
		vec3 v = (*particle).getPos();
		if (v.y < y) {
			v.y = y;
			//particle->offsetPos(moveOffset);
			particle->setPos(v);
		}
	}
}
