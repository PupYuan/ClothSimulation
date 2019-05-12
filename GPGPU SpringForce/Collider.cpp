#include <ClothSimulation\Collider.h>

void SphereCollider::ClothCollisionSimulate(PositionBasedUnit* c)
{
	std::vector<Particle>* particles =  & (c->getParticles());
	std::vector<Particle>::iterator particle;
	//判断particle是否在此碰撞体内
	for (particle = particles->begin(); particle != particles->end(); particle++)
	{
		vec3 v = (*particle).getPos() - center;
		//int temp = v.length();
		//v.length()有问题。v.length返回的是向量的维度
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
	//判断particle是否在此碰撞体内
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
