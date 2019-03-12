#pragma once
#include <ClothSimulation\Particles.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Spring.h"
class Collider;
class Cloth
{
private:
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	// total number of particles is num_particles_width*num_particles_height

	std::vector<Particle> particles; // all particles that are part of this cloth
	std::vector<Spring> Springs; // alle Springs between particles as part of this cloth
	Particle* getParticle(int x, int y) { return &particles[x*num_particles_height + y]; }
	//在内存中一份顶点数据
	std::vector<float>vertices;
	std::vector<unsigned int>indices;
	//绘制相关
	unsigned int VBO, VAO, EBO;

public:
	Cloth(float width, float height, int num_particles_width, int num_particles_height);
	~Cloth() {
		// optional: de-allocate all resources once they've outlived their purpose:
	    // ------------------------------------------------------------------------
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	vec3 calcTriangleNormal(Particle *p1, Particle *p2, Particle *p3)
	{
		vec3 pos1 = p1->getPos();
		vec3 pos2 = p2->getPos();
		vec3 pos3 = p3->getPos();

		vec3 v1 = pos2 - pos1;
		vec3 v2 = pos3 - pos1;

		return cross(v1, v2);
		
	}
	std::vector<Particle>& getParticles() {
		return particles;
	}
	void drawShaded();
	void addForce(const vec3 direction);
	void timeStep(float dt);
	void AddSpring(Particle* a, Particle* b, float ks, float kd);
	void CollisionDetection(Collider * collider);

};
