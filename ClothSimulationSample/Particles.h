#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> //for matrices
#include <glm/gtc/type_ptr.hpp>
const float DEFAULT_DAMPING = -0.0125f;
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
			//时间积分之前添加摩擦力，摩擦力跟速度相关
			velocity = (pos - old_pos) / dt;
			addForce(velocity*DEFAULT_DAMPING);

			vec3 temp = pos;
			pos = pos + (pos - old_pos) + acceleration * dt*dt;
			old_pos = temp;
			velocity = (pos - old_pos) / dt;
			acceleration = vec3(0, 0, 0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)

			if (pos.y <0) {
				pos.y = 0;
			}
		}
	}
	void makeUnmovable() { movable = false; }
	void offsetPos(const vec3 v) 
	{ 
		if (movable) pos += v;
	}
};
