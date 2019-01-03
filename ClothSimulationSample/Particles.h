#pragma once
#include "util.h"

class Particle
{
private:
	Vec3 pos;
	Vec3 accumulated_normal; // an accumulated normal (i.e. non normalized), used for OpenGL soft shading
public:
	Particle(Vec3 pos) :pos(pos)
	{

	}
	Particle() {}
	Vec3& getPos() { return pos; }
	void resetNormal() { accumulated_normal = Vec3(0, 0, 0); }
	Vec3& getNormal() { return accumulated_normal; } // notice, the normal is not unit length
	void addToNormal(Vec3 normal)
	{
		accumulated_normal += normal.normalized();
	}
};
