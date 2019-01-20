//ToDo1:先实现CPU进行物理模拟
#include <GL/glew.h>//初始化扩展
#include <glm/glm.hpp>//glm数学库
#include <glm/gtc/matrix_transform.hpp> //for matrices
#include <glm/gtc/type_ptr.hpp>
#include <GL/freeglut.h>//窗口相关的库
#include <iostream>
#include <vector>
using namespace std;
using namespace glm;

#include "Spring.h"
#include "Particles.h"
#include "Cloth.h"

//窗口参数
const int width = 1024, height = 1024;

//观察矩阵、投影矩阵的参数
float dist = -23;
float rX = 15, rY = 0;
GLdouble MV[16];
GLint viewport[4];
GLdouble P[16];
glm::vec3 Up = glm::vec3(0, 1, 0), Right, viewDir;

//画椭圆的一些参数
glm::mat4 ellipsoid, inverse_ellipsoid;
float fRadius = 1;
int iStacks = 30;
int iSlices = 30;

// 物理模拟   Spring 
const int STRUCTURAL_SPRING = 0;
const int SHEAR_SPRING = 1;
const int BEND_SPRING = 2;
int spring_count = 0;

int num_particles_width = 20; // number of particles in "width" direction
int num_particles_height = 20; // number of particles in "height" direction
const size_t total_points = (num_particles_width + 1)*(num_particles_height + 1);
int sizeX = 4,
sizeY = 4;
float hsize = sizeX / 2.0f;
std::vector<Particle> particles; // all particles that are part of this cloth

const int NUM_ITER = 1;
int selected_index = -1;
vector<GLushort> indices;
vector<Spring> springs;

glm::vec3 gravity = glm::vec3(0.0f, -0.981f, 0.0f);
float mass = 1.0f;

Cloth cloth(20, 20, 20, 20);

//统计帧率用的信息
LARGE_INTEGER frequency;        // ticks per second
LARGE_INTEGER t1, t2;           // ticks
double frameTimeQP = 0;
float frameTime = 0;
float startTime = 0, fps = 0;
int totalFrames = 0;
float timeStep = 1.0f / 60.0f;
float currentTime = 0;
double accumulator = timeStep;
#define MAX_PATH 100
char info[MAX_PATH] = { 0 };
void CalcFPS() {
	float newTime = (float)glutGet(GLUT_ELAPSED_TIME);
	frameTime = newTime - currentTime;
	currentTime = newTime;
	//accumulator += frameTime;

	//Using high res. counter
	QueryPerformanceCounter(&t2);
	// compute and print the elapsed time in millisec
	frameTimeQP = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	t1 = t2;
	accumulator += frameTimeQP;

	++totalFrames;
	if ((newTime - startTime) > 1000)
	{
		float elapsedTime = (newTime - startTime);
		fps = (totalFrames / elapsedTime) * 1000;
		startTime = newTime;
		totalFrames = 0;
	}

	sprintf_s(info, "FPS: %3.2f, Frame time (GLUT): %3.4f msecs, Frame time (QP): %3.3f", fps, frameTime, frameTimeQP);
	glutSetWindowTitle(info);
}

//void AddSpring(Particle* a, Particle* b, float ks, float kd) {
//	Spring spring;
//	spring.p1 = a;
//	spring.p2 = b;
//	spring.Ks = ks;
//	spring.Kd = kd;
//	glm::vec3 deltaP = vec3(a->getPos() - b->getPos());
//	spring.restDistance = sqrt(glm::dot(deltaP, deltaP));
//	springs.push_back(spring);
//}
//
//Particle* getParticle(int x, int y) { 
//	return &particles[y*(num_particles_width+1) + x]; 
//}
void Init(GLvoid)
{
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	//ball_collider = new SphereCollider(ball_pos, 2);

	//在绘制多边形时除了默认的填充方式, 还可以使用点和线
	//使用glPolygonMode函数来设置模式
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	startTime = (float)glutGet(GLUT_ELAPSED_TIME);
	// get ticks per second
	QueryPerformanceFrequency(&frequency);

	// start timer
	QueryPerformanceCounter(&t1);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPointSize(5);


	//int i = 0, j = 0, count = 0;
	//int l1 = 0, l2 = 0;
	//int v = num_particles_height + 1;
	//int u = num_particles_width + 1;

	//particles.resize(total_points);
	//indices.resize(num_particles_width*num_particles_height * 2 * 3);

	////fill in positions
	//for (j = 0; j <= num_particles_height; j++) {
	//	for (i = 0; i <= num_particles_width; i++) {
	//		vec3 pos = glm::vec3(((float(i) / (u - 1)) * 2 - 1)* hsize, sizeX + 1, ((float(j) / (v - 1))* sizeY));
	//		particles[count] = Particle(pos); // insert particle in column x at y'th row
	//		count++;
	//	}
	//}

	////fill in indices
	//GLushort* id = &indices[0];
	//for (i = 0; i < num_particles_height; i++) {
	//	for (j = 0; j < num_particles_width; j++) {
	//		int i0 = i * (num_particles_width + 1) + j;
	//		int i1 = i0 + 1;
	//		int i2 = i0 + (num_particles_width + 1);
	//		int i3 = i2 + 1;
	//		if ((j + i) % 2) {
	//			*id++ = i0; *id++ = i2; *id++ = i1;
	//			*id++ = i1; *id++ = i2; *id++ = i3;
	//		}
	//		else {
	//			*id++ = i0; *id++ = i2; *id++ = i3;
	//			*id++ = i0; *id++ = i3; *id++ = i1;
	//		}
	//	}
	//}
	//// Setup springs
	//// 添加Springs
	//for (int x = 0; x <= num_particles_width; x++)
	//{
	//	for (int y = 0; y <= num_particles_height; y++)
	//	{
	//		// Structure Springs
	//		if (x + 1 <= num_particles_width)
	//			AddSpring(getParticle(x, y), getParticle(x + 1, y),KsStruct,KdStruct);
	//		if (y + 1 <= num_particles_height)
	//			AddSpring(getParticle(x, y), getParticle(x, y + 1), KsStruct,KdStruct);

	//		//Shear Springs
	//		if (y + 1 <= num_particles_height && x + 1 <= num_particles_width)
	//			AddSpring(getParticle(x, y), getParticle(x + 1, y + 1),KsShear,KdShear);
	//		if (y - 1 >= 0 && x - 1 >= 0)
	//			AddSpring(getParticle(x, y), getParticle(x - 1, y - 1), KsShear,KdShear);

	//		//Bending Springs
	//		if (x + 2 <= num_particles_width)
	//			AddSpring(getParticle(x, y), getParticle(x + 2, y), KsBend,KdBend);
	//		if (y + 2 <= num_particles_height)
	//			AddSpring(getParticle(x, y), getParticle(x, y + 2), KsBend,KdBend);
	//		if (y + 2 <= num_particles_height && x + 2 <= num_particles_width)
	//			AddSpring(getParticle(x, y), getParticle(x + 2, y + 2), KsBend,KdBend);
	//		if (y - 2 >= 0 && x - 2 >= 0)
	//			AddSpring(getParticle(x, y), getParticle(x - 2, y - 2), KsBend,KdBend);
	//	}
	//}
	//// making the upper left most three and right most three particles unmovable
	//for (int i = 0; i < 3; i++)
	//{
	//	getParticle(0 + i, 0)->makeUnmovable();
	//	getParticle(num_particles_width- i, 0)->makeUnmovable();
	//}

	//create a basic ellipsoid object
	ellipsoid = glm::translate(glm::mat4(1), glm::vec3(0, 2, 0));
	ellipsoid = glm::rotate(ellipsoid, 45.0f, glm::vec3(1, 0, 0));
	ellipsoid = glm::scale(ellipsoid, glm::vec3(fRadius, fRadius, fRadius / 2));
	inverse_ellipsoid = glm::inverse(ellipsoid);
}

void OnKey(unsigned char key, int, int) {

}

void OnShutdown() {
	//清理掉数据
}

void OnMouseDown(int button, int s, int x, int y) {

}

void OnMouseMove(int x, int y) {

}

void OnReshape(int nw, int nh) {
	glViewport(0, 0, nw, nh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)nw / (GLfloat)nh, 1.f, 100.0f);

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_PROJECTION_MATRIX, P);

	glMatrixMode(GL_MODELVIEW);
}

//网格的大小
const int GRID_SIZE = 10;
void DrawGrid()
{
	glBegin(GL_LINES);
	glColor3f(0.5f, 0.5f, 0.5f);
	for (int i = -GRID_SIZE; i <= GRID_SIZE; i++)
	{
		glVertex3f((float)i, 0, (float)-GRID_SIZE);
		glVertex3f((float)i, 0, (float)GRID_SIZE);

		glVertex3f((float)-GRID_SIZE, 0, (float)i);
		glVertex3f((float)GRID_SIZE, 0, (float)i);
	}
	glEnd();
}
void RenderCPU() {
	//draw polygons
	glColor3f(1, 1, 1);
	int i;

	glBegin(GL_TRIANGLES);
	for (i = 0; i<indices.size(); i += 3) {
		glm::vec3 p1 = particles[indices[i]].getPos();
		glm::vec3 p2 = particles[indices[i+1]].getPos();
		glm::vec3 p3 = particles[indices[i + 2]].getPos();
		/*glm::vec3 p1 = vec3(X[indices[i]]);
		glm::vec3 p2 = vec3(X[indices[i + 1]]);
		glm::vec3 p3 = vec3(X[indices[i + 2]]);*/
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);
	}
	glEnd();

	//draw points	
	glBegin(GL_POINTS);
	for (i = 0; i<total_points; i++) {
		//glm::vec3 p = vec3(X[i]);
		glm::vec3 p = particles[i].getPos();
		int is = (i == selected_index);
		glColor3f((float)!is, (float)is, (float)is);
		glVertex3f(p.x, p.y, p.z);
	}
	glEnd();
}
void RenderGPU() {}
void OnRender() {
	CalcFPS();
	//清理Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//传入观察矩阵、投影矩阵的一些参数
	glLoadIdentity();
	glTranslatef(0, 0, dist);
	glRotatef(rX, 1, 0, 0);
	glRotatef(rY, 0, 1, 0);

	glGetDoublev(GL_MODELVIEW_MATRIX, MV);
	viewDir.x = (float)-MV[2];
	viewDir.y = (float)-MV[6];
	viewDir.z = (float)-MV[10];
	Right = glm::cross(viewDir, Up);

	//draw grid
	DrawGrid();

	//draw ellipsoid
	glColor3f(0, 1, 0);
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(ellipsoid));
	glutWireSphere(fRadius, iSlices, iStacks);
	glPopMatrix();

	cloth.draw();
	glutSwapBuffers();
}
inline glm::vec3 GetVerletVelocity(glm::vec3 x_i, glm::vec3 xi_last, float dt) {
	return  (x_i - xi_last) / dt;
}
void ComputeForces(float dt) {
	//size_t i = 0;

	//for (i = 0; i<total_points; i++) {
	//	glm::vec3 V = GetVerletVelocity(particles[i].getPos(), particles[i].getLastPos(), dt);
	//	//add gravity force
	//	if (i != 0 && i != (num_particles_width))
	//		particles[i].addForce(gravity);
	//	//add force due to damping of velocity
	//	particles[i].addForce(DEFAULT_DAMPING * V);
	//}


	//for (i = 0; i<springs.size(); i++) {
	//	glm::vec3 p1 = vec3(springs[i].p1->getPos());
	//	glm::vec3 p1Last = vec3(springs[i].p1->getLastPos());
	//	glm::vec3 p2 = vec3(springs[i].p2->getPos());
	//	glm::vec3 p2Last = vec3(springs[i].p2->getLastPos());

	//	glm::vec3 v1 = GetVerletVelocity(p1, p1Last, dt);
	//	glm::vec3 v2 = GetVerletVelocity(p2, p2Last, dt);

	//	glm::vec3 deltaP = p1 - p2;
	//	glm::vec3 deltaV = v1 - v2;
	//	float dist = glm::length(deltaP);

	//	float leftTerm = -springs[i].Ks * (dist - springs[i].restDistance);
	//	float rightTerm = springs[i].Kd * (glm::dot(deltaV, deltaP) / dist);
	//	glm::vec3 springForce = (leftTerm + rightTerm)*glm::normalize(deltaP);

	//	springs[i].p1->addForce(springForce);
	//	springs[i].p2->addForce(-springForce);
	//}
}
void IntegrateVerlet(float deltaTime) {
	float deltaTime2 = (deltaTime*deltaTime);
	size_t i = 0;

	float inv_mass = 1.0f / mass;
	for (i = 0; i<total_points; i++) {
		particles[i].timeStep(deltaTime);
	}
}
void StepPhysics(float dt) {
	cloth.addForce(gravity);
	cloth.timeStep(dt);
	//IntegrateVerlet(dt);
}
void OnIdle() {
	//Fixed time stepping + rendering at different fps
	if (accumulator >= timeStep)
	{
		StepPhysics(timeStep);
		accumulator -= timeStep;
	}
	glutPostRedisplay();
}
void main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("GLUT Cloth Demo [GLSL based Verlet Integration using GPGPU approach]");

	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnReshape);
	glutIdleFunc(OnIdle);

	glutMouseFunc(OnMouseDown);
	glutMotionFunc(OnMouseMove);
	glutKeyboardFunc(OnKey);
	glutCloseFunc(OnShutdown);

	glewInit();
	Init();

	glutMainLoop();
}