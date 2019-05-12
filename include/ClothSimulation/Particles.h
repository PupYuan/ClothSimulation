#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> //for matrices
#include <glm/gtc/type_ptr.hpp>

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
	float * x;
	float * y;
	float * z;
	float * normal_x;
	float * normal_y;
	float * normal_z;
	Particle(vec3 pos,float _mass=1) : pos(pos), old_pos(pos), acceleration(vec3(0, 0, 0)), mass(_mass), movable(true), accumulated_normal(vec3(0, 0, 0)) {}
	Particle() {}
	vec3& getPos() { return pos; }
	vec3 getCurrentPos() { return pos; }
	vec3& getLastPos() { return old_pos; }
	float getMass() { return mass; };
	void resetNormal() { accumulated_normal = vec3(0, 0, 0); }
	vec3& getNormal() { return accumulated_normal; } // notice, the normal is not unit length
	vec3 getAcceleration() {
		return acceleration;
	}
	vec3& getVelocity() {
		return velocity;
	}
	void setVelocity(vec3 V) {
		velocity = V;
	}
	void addToNormal(vec3 normal)
	{
		accumulated_normal = accumulated_normal + normalize(normal);
		*normal_x = accumulated_normal.x;
		*normal_y = accumulated_normal.y;
		*normal_z = accumulated_normal.z;
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
			pos = pos + velocity * dt;
			//pos = pos + velocity* dt + acceleration * dt*dt;
			//pos = pos + (pos - old_pos) + acceleration * dt*dt;
			old_pos = temp;
			acceleration = vec3(0, 0, 0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)

			//然后把数据传递给之前的顶点数据
			*x = pos.x;
			*y = pos.y;
			*z = pos.z;
		}
	}
	void makeUnmovable() { movable = false; }
	bool isMovable() { return movable; }
	void offsetPos(const vec3 v) 
	{ 
		if (movable) pos += v;
		*x = pos.x;
		*y = pos.y;
		*z = pos.z;
	}

	void setPos(const vec3 p)
	{
		if (movable) pos = p;
		*x = pos.x;
		*y = pos.y;
		*z = pos.z;
	}
};
