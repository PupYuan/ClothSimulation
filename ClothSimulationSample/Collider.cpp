#include "Collider.h"

void SphereCollider::ClothCollisionSimulate(Cloth* c)
{
	std::vector<Particle>* particles =  & (c->getParticles());
	std::vector<Particle>::iterator particle;
	//判断particle是否在此碰撞体内
	for (particle = particles->begin(); particle != particles->end(); particle++)
	{
		Vec3 v = (*particle).getPos() - center;
		if (v.length() < radius) {
			Vec3 moveOffset = v.normalized() * (radius - v.length());
			particle->offsetPos(moveOffset);
		}
	}
}

void Collider::ClothCollisionSimulate(Cloth *c)
{

}
