#pragma once
#include "util.h"
#define DAMPING 0.01
#define TIME_STEPSIZE2 0.5*0.5

class Particle
{
private:
	Vec3 pos;
	Vec3 old_pos;//上次模拟步长时所在的位置
	Vec3 accumulated_normal; // an accumulated normal (i.e. non normalized), used for OpenGL soft shading
	Vec3 acceleration;
	float mass;//质量
	bool movable;//是否可以移动
public:
	Particle(Vec3 pos) : pos(pos), old_pos(pos), acceleration(Vec3(0, 0, 0)), mass(1), movable(true), accumulated_normal(Vec3(0, 0, 0)) {}
	Particle() {}
	Vec3& getPos() { return pos; }
	void resetNormal() { accumulated_normal = Vec3(0, 0, 0); }
	Vec3& getNormal() { return accumulated_normal; } // notice, the normal is not unit length
	void addToNormal(Vec3 normal)
	{
		accumulated_normal += normal.normalized();
	}

	void addForce(Vec3 f)
	{
		acceleration += f / mass;
	}

	//每次步长的一次模拟，采用verlet积分方法，可以省去速度，并且稳定
	void timeStep(float dt)
	{
		if (movable)
		{
			Vec3 temp = pos;
			pos = pos + (pos - old_pos)*(1.0 - DAMPING) + acceleration * dt*dt;
			old_pos = temp;
			acceleration = Vec3(0, 0, 0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)
		}
	}
	void makeUnmovable() { movable = false; }
	void offsetPos(const Vec3 v) { if (movable) pos += v; }
};
