#pragma once

#ifdef _WIN32
#include <windows.h> 
#endif
#include <vector>
#include "Particles.h"
#include "util.h"
#include <GL/glut.h> 
#include "Constraint.h"
#define CONSTRAINT_ITERATIONS 15 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)

class Collider;
class Cloth {
private:
	
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	// total number of particles is num_particles_width*num_particles_height

	std::vector<Particle> particles; // all particles that are part of this cloth
	std::vector<Constraint> constraints; // alle constraints between particles as part of this cloth

	Particle* getParticle(int x, int y) { return &particles[y*num_particles_width + x]; }
	void makeConstraint(Particle *p1, Particle *p2) { constraints.push_back(Constraint(p1, p2)); }
	/* A private method used by drawShaded(), that draws a single triangle p1,p2,p3 with a color*/
	void drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vec3 color);
public:
	Cloth(float width, float height, int num_particles_width, int num_particles_height);

	std::vector<Particle>& getParticles() {
		return particles;
	}
	void drawShaded();
	/* used to add gravity (or any other arbitrary vector) to all particles*/
	void addForce(const Vec3 direction);
	//����ģ��
	void timeStep();
	//��ײ���
	void CollisionDetection(Collider * collider);
};
