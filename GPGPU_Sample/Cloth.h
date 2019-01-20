#pragma once

#ifdef _WIN32
#include <windows.h> 
#endif
#include <vector>
#include "Particles.h"
#include <GL/glut.h> 
#include "Spring.h"
#define Spring_ITERATIONS 1 // how many iterations of Spring satisfaction each frame (more is rigid, less is soft)
using namespace std;
using namespace glm;
//绘制模式
enum Mode { CPU, GPU };
class Collider;
class Cloth {
private:
	Mode current_mode = CPU;
	vector<GLushort> indices;
	int num_particles_width = 20; // number of particles in "width" direction
	int num_particles_height = 20; // number of particles in "height" direction
	const size_t total_points = (num_particles_width + 1)*(num_particles_height + 1);
	int sizeX = 4,
		sizeY = 4;
	float hsize = sizeX / 2.0f;
	int selected_index = -1;
	std::vector<Particle> particles; // all particles that are part of this cloth
	std::vector<Spring> Springs; // alle Springs between particles as part of this cloth

	Particle* getParticle(int x, int y) {
		return &particles[y*(num_particles_width + 1) + x];
	}
	void makeSpring(Particle *p1, Particle *p2) { Springs.push_back(Spring(p1, p2)); }
	/* A private method used by drawShaded(), that draws a single triangle p1,p2,p3 with a color*/
	void drawTriangle(Particle *p1, Particle *p2, Particle *p3, const vec3 color);
	void RenderCPU();
	void RenderGPU();
public:
	Cloth(float width, float height, int num_particles_width, int num_particles_height);

	std::vector<Particle>& getParticles() {
		return particles;
	}
	void draw();
	/* used to add gravity (or any other arbitrary vector) to all particles*/
	void addForce(const vec3 direction);

	//物理模拟
	void timeStep(float dt);
	//碰撞检测
	void CollisionDetection(Collider * collider);
	void AddSpring(Particle* a, Particle* b, float ks, float kd);
};
