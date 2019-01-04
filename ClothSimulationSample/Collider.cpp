#include "Collider.h"

void SphereCollider::ClothCollisionSimulate(Cloth* c)
{
	std::vector<Particle>* particles =  & (c->getParticles());
	std::vector<Particle>::iterator particle;
	//�ж�particle�Ƿ��ڴ���ײ����
	for (particle = particles->begin(); particle != particles->end(); particle++)
	{
		if ((particle->getPos() - center).length() < radius) {
			Vec3 moveOffset = (particle->getPos() - center).normalized() * radius;
			particle->offsetPos(moveOffset);
		}
	}
}

void Collider::ClothCollisionSimulate(Cloth *c)
{

}
