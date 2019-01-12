#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> //for matrices
#include <glm/gtc/type_ptr.hpp>
#define DAMPING 0.01f
#define TIME_STEPSIZE2 0.5f*0.5f
using namespace glm;

class Particle
{
private:
	vec3 pos;
	vec3 old_pos;//上次模拟步长时所在的位置
	vec3 accumulated_normal; // an accumulated normal (i.e. non normalized), used for OpenGL soft shading
	vec3 acceleration;
	float mass;//质量
	bool movable;//是否可以移动
public:
	Particle(vec3 pos) : pos(pos), old_pos(pos), acceleration(vec3(0, 0, 0)), mass(1), movable(true), accumulated_normal(vec3(0, 0, 0)) {}
	Particle() {}
	vec3& getPos() { return pos; }
	void resetNormal() { accumulated_normal = vec3(0, 0, 0); }
	vec3& getNormal() { return accumulated_normal; } // notice, the normal is not unit length
	void addToNormal(vec3 normal)
	{
		accumulated_normal = accumulated_normal + normalize(normal);
	}

	void addForce(vec3 f)
	{
		acceleration += f / mass;
	}

	//每次步长的一次模拟，采用verlet积分方法，可以省去速度，并且稳定
	void timeStep()
	{
		if (movable)
		{
			vec3 temp = pos;
			pos = pos + (pos - old_pos)*(1.0f - DAMPING) + acceleration * TIME_STEPSIZE2;
			old_pos = temp;
			acceleration = vec3(0, 0, 0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)
		}
	}
	void makeUnmovable() { movable = false; }
	void offsetPos(const vec3 v) { if (movable) pos += v; }
};
