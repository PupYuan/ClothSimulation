#pragma once
#include <ClothSimulation\Cloth.h>
//��ײ�壬������ײ���
class Collider {
public:
	virtual void ClothCollisionSimulate(Cloth* c) ;
};

class SphereCollider :public Collider {
private:
	vec3 center;
	float radius;
public:
	SphereCollider(vec3 _center, float _radius) :center(_center), radius(_radius) {

	}
	virtual void ClothCollisionSimulate(Cloth* c);
	void setPos(vec3 _pos) {
		center = _pos;
	}
	vec3 getCenter() {
		return center;
	}
};