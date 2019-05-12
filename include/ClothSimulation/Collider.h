#pragma once
#include <ClothSimulation\Cloth.h>
//碰撞体，用于碰撞检测
class Collider {
public:
	virtual void ClothCollisionSimulate(PositionBasedUnit* c) ;
	vec3 center;
	void setPos(vec3 _pos) {
		center = _pos;
	}
	vec3 getCenter() {
		return center;
	}
	bool kinematicStatic = false;
};

class SphereCollider :public Collider {
private:
	float radius;
public:
	SphereCollider(vec3 _center, float _radius) :radius(_radius) {
		setPos(_center);
	}
	virtual void ClothCollisionSimulate(PositionBasedUnit* c);
	
};

class PlaneCollider :public Collider {
private:
	float y;
public:
	PlaneCollider(float _y) :y(_y) {
		kinematicStatic = true;//平台是静态的
		center = vec3(0, _y, 0);
	}
	virtual void ClothCollisionSimulate(PositionBasedUnit* c);
};