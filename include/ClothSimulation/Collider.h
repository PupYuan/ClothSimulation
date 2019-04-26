#pragma once
#include <ClothSimulation\Cloth.h>
//Åö×²Ìå£¬ÓÃÓÚÅö×²¼ì²â
class Collider {
public:
	virtual void ClothCollisionSimulate(PositionBasedUnit* c) ;
};

class SphereCollider :public Collider {
private:
	vec3 center;
	float radius;
public:
	SphereCollider(vec3 _center, float _radius) :center(_center), radius(_radius) {

	}
	virtual void ClothCollisionSimulate(PositionBasedUnit* c);
	void setPos(vec3 _pos) {
		center = _pos;
	}
	vec3 getCenter() {
		return center;
	}
};