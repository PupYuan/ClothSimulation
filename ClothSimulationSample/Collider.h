#pragma once
#include "util.h"
#include "Cloth.h"
//Åö×²Ìå£¬ÓÃÓÚÅö×²¼ì²â
class Collider {
public:
	virtual void ClothCollisionSimulate(Cloth* c) ;
};

class SphereCollider :public Collider {
private:
	Vec3 center;
	float radius;
public:
	SphereCollider(Vec3 _center, float _radius) :center(_center), radius(_radius) {

	}
	virtual void ClothCollisionSimulate(Cloth* c);
	void setPos(Vec3 _pos) {
		center = _pos;
	}
};