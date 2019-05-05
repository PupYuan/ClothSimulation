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
//pbdģ��Ļ�����Ԫ���������ϡ�ɳ��������
class PositionBasedUnit {
public:
	std::vector<Particle> particles; // all particles that are part of this cloth
	virtual void timeStep(float dt) =0;
	virtual void render() = 0;
	virtual std::vector<Particle>& getParticles() {
		return particles;
	}
	glm::vec3 gravity = glm::vec3(0.0f, -0.00981f, 0.0f);
};
enum Mode { CPU, GPU };
class Cloth:public PositionBasedUnit
{
private:
	float kBend = 0.5f;
	float kStretch = 0.95f;
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
	//���ڴ���һ�ݶ�������
	std::vector<float>vertices;
	std::vector<unsigned int>indices;
	//�������
	unsigned int VBO, VAO, EBO;

	Shader *renderShader;//��Ⱦ�õ�Shader

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
	//���ڴ���һ�ݶ�������
	std::vector<float>vertices;
	std::vector<unsigned int>indices;
	//�������
	unsigned int VBO, VAO, EBO;
	int width;
	int height;
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	//GPGPU���
	//λ������
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

	//ÿ����Ⱦ��ʱ���������������
	const int NUM_ITER = 2;

	Shader *verletShader;
	Shader *renderShader;//��Ⱦ�õ�Shader

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
	void calcNormal();
	virtual void render();
	void readNormal();
private:
	//ÿ����������ʱ��Լ���ĵ�������
	const int NUM_ITER = 2;
	int vertice_data_length = 8;
	std::vector<float>vertices;
	std::vector<unsigned int>indices;
	//�������
	unsigned int VBO, VAO, EBO;
	int width;
	int height;
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	//GPGPU���
	//λ������
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
	Shader *renderShader;//��Ⱦ�õ�Shader
	Shader *DistanceConstraintCompute;
	Shader *SuccessiveOverRelaxationCompute;
	Shader* IntegrationShader;
	Shader* NormalCalcShader;
	Shader* BendingConstraintCompute;
	//Scene
	SceneManager * scene;
	const float global_dampening = 0.7f;
	
	//Լ�����
	//Distance Constraint
	GLuint DistanceTexID1;
	GLuint DistanceTexID2;
	GLuint RestDistanceTexID;
	std::vector<i32vec2>DistanceConstraintIndexData1;
	std::vector<i32vec2>DistanceConstraintIndexData2;
	std::vector<float>RestDistanceData;
	float kStretch = 0.9f;
	//Bending Constraint
	GLuint BendingTexID1;
	GLuint BendingTexID2;
	GLuint BendingTexID3;
	std::vector<i32vec2>BendingConstraintIndexData1;
	std::vector<i32vec2>BendingConstraintIndexData2;
	std::vector<i32vec2>BendingConstraintIndexData3;
	float kBend = 0.5f;
	GLuint RestDistanceTexID2;
	std::vector<float>RestDistanceData2;
	//����������
	GLuint DeltaTexXID;
	GLuint DeltaTexYID;
	GLuint DeltaTexZID;
	//�������
	GLuint NormalTexID[3];
	GLuint NormalVboID[3];
	std::vector<int>NormalX;
	std::vector<int>NormalY;
	std::vector<int>NormalZ;
	//SOR���
	GLuint NiTexID;//SOR���ʱ��ȥ������
	std::vector<int>Ni;
	float SuccessiveW = 1.5f;
	//�����ܶ�
	float texDensityX = 10.0f;
	float texDensityY = 10.0f;
};
