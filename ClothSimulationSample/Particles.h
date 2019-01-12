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
	vec3 old_pos;
	vec3 accumulated_normal; // an accumulated normal (i.e. non normalized), used for OpenGL soft shading
	vec3 acceleration;
	vec3 velocity;
	float mass;
	bool movable;
public:
	Particle(vec3 pos) : pos(pos), old_pos(pos), acceleration(vec3(0, 0, 0)), mass(1), movable(true), accumulated_normal(vec3(0, 0, 0)) {}
	Particle() {}
	vec3& getPos() { return pos; }
	vec3& getLastPos() { return old_pos; }
	void resetNormal() { accumulated_normal = vec3(0, 0, 0); }
	vec3& getNormal() { return accumulated_normal; } // notice, the normal is not unit length
	vec3& getVelocity() {
		return velocity;
	}
	void addToNormal(vec3 normal)
	{
		accumulated_normal = accumulated_normal + normalize(normal);
	}

	void addForce(vec3 f)
	{
		acceleration += f / mass;
	}

	void timeStep(float dt)
	{
		if (movable)
		{
			vec3 temp = pos;
			pos = pos + (pos - old_pos)*(1.0f - DAMPING) + acceleration * dt*dt;
			old_pos = temp;
			velocity = (pos - old_pos) / dt;
			acceleration = vec3(0, 0, 0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)
		}
	}
	void makeUnmovable() { movable = false; }
	void offsetPos(const vec3 v) 
	{ 
		if (movable) pos += v;
	}
};
