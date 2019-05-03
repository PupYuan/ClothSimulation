#pragma once
#include <learnopengl/shader.h>
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
#include "Constraint.h"
class SceneManager;
#define CHECK_GL_ERRORS assert(glGetError()==GL_NO_ERROR);
//pbd模拟的基本单元，包括布料、沙粒、发束
class PositionBasedUnit {
public:
	std::vector<Particle> particles; // all particles that are part of this cloth
	virtual void timeStep(float dt) =0;
	virtual void render() = 0;
	virtual std::vector<Particle>& getParticles() {
		return particles;
	}
};
enum Mode { CPU, GPU };
class Cloth:public PositionBasedUnit
{
private:
	int vertice_data_length = 8;
	size_t total_points;
	int width;
	int height;
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	// total number of particles is num_particles_width*num_particles_height
	float kDamp = 0.00125f;
	std::vector<glm::vec3> Ri; //Ri = Xi-Xcm 
	std::vector<Spring> Springs; // alle Springs between particles as part of this cloth
	std::vector<Constraint*> Constraints; // alle Springs between particles as part of this cloth
	Particle* getParticle(int x, int y) { return &particles[y*num_particles_width + x]; }
	//在内存中一份顶点数据
	std::vector<float>vertices;
	std::vector<unsigned int>indices;
	//绘制相关
	unsigned int VBO, VAO, EBO;

	Shader *renderShader;//渲染用的Shader

	//Scene
	SceneManager * scene;
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
	virtual std::vector<Particle>& getParticles() {
		return particles;
	}
	void addForce(const vec3 direction);
	virtual void timeStep(float dt);
	void GroundCollision();
	void AddSpring(Particle* a, Particle* b, float ks, float kd);
	void AddConstraint(Particle * a, Particle * b, float k);
	void SetScene(SceneManager* _scene) {
		scene = _scene;
	}
	virtual void render();
};

class GPUCloth :public PositionBasedUnit 
{
public:
	virtual void timeStep(float dt);
	GPUCloth(float width, float height, int num_particles_width, int num_particles_height);
	void SetScene(SceneManager* _scene) {
		scene = _scene;
	}
	virtual void render();
private:
	int vertice_data_length = 8;
	//在内存中一份顶点数据
	std::vector<float>vertices;
	std::vector<unsigned int>indices;
	//绘制相关
	unsigned int VBO, VAO, EBO;
	int width;
	int height;
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	//GPGPU相关
	//位置数据
	const size_t total_points = (num_particles_width)*(num_particles_height);
	std::vector<glm::vec4> X;
	std::vector<glm::vec4> X_last;
	std::vector<glm::vec3> Normal;
	std::vector<glm::vec2> TexCoord;

	unsigned int vaoID;
	GLuint fboID[2];
	GLuint attachID[4];
	int readID = 0, writeID = 1;
	GLuint vboID;
	GLuint vboID2;
	GLuint vboID3;
	GLenum mrt[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	float* _data[2];

	GLuint t_query;
	GLuint64 elapsed_time;
	float delta_time = 0;

	//每次渲染的时候做几次物理迭代
	const int NUM_ITER = 4;

	Shader *verletShader;
	Shader *renderShader;//渲染用的Shader

	//Scene
	SceneManager * scene;
	//render for GPGPU
	unsigned int quadVAO, quadVBO;
	const float global_dampening = 0.98f; //DevO: 24.07.2011  //global velocity dampening !!!
};

class ComputeShaderCloth :public PositionBasedUnit {
public:
	virtual void timeStep(float dt);
	ComputeShaderCloth(float width, float height, int num_particles_width, int num_particles_height);
	void SetScene(SceneManager* _scene) {
		scene = _scene;
	}
	virtual void render();
private:
	int vertice_data_length = 8;
	std::vector<float>vertices;
	std::vector<unsigned int>indices;
	//绘制相关
	unsigned int VBO, VAO, EBO;
	int width;
	int height;
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	//GPGPU相关
	//位置数据
	const size_t total_points = (num_particles_width)*(num_particles_height);
	std::vector<glm::vec4> X;
	std::vector<glm::vec4> X_last;
	std::vector<glm::vec4> Normal;
	std::vector<glm::vec2> TexCoord;
	std::vector<int> Null_X;

	GLuint fboID[2];
	GLuint attachID[4];
	int readID = 0, writeID = 1;
	GLuint vboID;
	GLuint vboID2;
	GLuint vboID3;
	GLenum mrt[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	float* _data[2];
	unsigned int vaoID;

	Shader *computeShader;
	Shader *renderShader;//渲染用的Shader
	Shader *DistanceConstraintCompute;
	Shader *SuccessiveOverRelaxationCompute;
	Shader* IntegrationShader;
	Shader* NormalCalcShader;
	//Scene
	SceneManager * scene;
	const float global_dampening = 0.9f;
	
	//约束相关
	GLuint DistanceTexID1;
	GLuint DistanceTexID2;
	GLuint RestDistanceTexID;
	std::vector<i32vec2>DistanceConstraintIndexData1;
	std::vector<i32vec2>DistanceConstraintIndexData2;
	std::vector<float>RestDistanceData;
	float kStretch = 0.5f;
	std::vector<int>Ni;
	std::vector<int>NormalX;
	std::vector<int>NormalY;
	std::vector<int>NormalZ;
	GLuint DeltaTexXID;
	GLuint DeltaTexYID;
	GLuint DeltaTexZID;
	GLuint NiTexID;
	GLuint NormalTexID[3];
	GLuint NormalVboID[3];
};
