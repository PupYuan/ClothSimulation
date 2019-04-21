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
#include <learnopengl/shader.h>
#include "Constraint.h"
class SceneManager;

enum Mode { CPU, GPU };
class Cloth
{
private:
	Mode current_mode = CPU;
	int width;
	int height;
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	// total number of particles is num_particles_width*num_particles_height
	float kDamp = 0.00125f;
	std::vector<glm::vec3> Ri; //Ri = Xi-Xcm 
	std::vector<Particle> particles; // all particles that are part of this cloth
	std::vector<Spring> Springs; // alle Springs between particles as part of this cloth
	std::vector<Constraint*> Constraints; // alle Springs between particles as part of this cloth
	Particle* getParticle(int x, int y) { return &particles[y*num_particles_width + x]; }
	//在内存中一份顶点数据
	std::vector<float>vertices;
	std::vector<unsigned int>indices;
	//绘制相关
	unsigned int VBO, VAO, EBO;

	//GPGPU相关
	//位置数据
	const size_t total_points = (num_particles_width)*(num_particles_height);
	std::vector<glm::vec4> X;
	std::vector<glm::vec4> X_last;
	std::vector<glm::vec3> Normal;
	std::vector<glm::vec2> TexCoord;

	//GPGPU Constraint
	std::vector<glm::vec4> DistancePos;
	std::vector<glm::vec2> DistanceIndex;

	int texture_size_x = 0;
	int texture_size_y = 0;

	GLuint fboID[2];
	GLuint attachID[4];
	int readID = 0, writeID = 1;
	GLuint vboID;
	GLuint vboID2;
	GLuint vboID3;
	GLenum mrt[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };


	float* _data[2];
	size_t i = 0;
	GLfloat vRed[4] = { 1,0,0,1 };
	GLfloat vWhite[4] = { 1,1,1,1 };

	GLuint t_query;
	GLuint64 elapsed_time;
	float delta_time = 0;

	//每次渲染的时候做几次物理迭代
	const int NUM_ITER = 2;

	Shader *verletShader;
	Shader *renderShader;//渲染用的Shader

	//Scene
	SceneManager * scene;
	//render for GPGPU
	unsigned int quadVAO, quadVBO;
	unsigned int vaoID;
	void InitCPU();
	void InitGPU();
	const float global_dampening = 0.98f; //DevO: 24.07.2011  //global velocity dampening !!!
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
	void RenderCPU();
	void RenderGPU();
	void drawShaded();
	void addForce(const vec3 direction);
	void timeStep(float dt);
	void GroundCollision();
	void AddSpring(Particle* a, Particle* b, float ks, float kd);
	void AddConstraint(Particle * a, Particle * b, float k);
	void SetScene(SceneManager* _scene) {
		scene = _scene;
	}
};
