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
#include "Collider.h"

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
//glm::mat4 ellipsoid, inverse_ellipsoid;
//float fRadius = 1;
//int iStacks = 30;
//int iSlices = 30;

SphereCollider* ball_collider;
vec3 ball_pos(0,2,0); // the center of our one ball
float ball_time = 0;
float ball_radius = 2; // the radius of our one ball

glm::vec3 gravity = glm::vec3(0.0f, -0.981f, 0.0f);
Cloth cloth(4, 4, 20, 20);

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

void Init(GLvoid)
{
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

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

	//圆形碰撞体
	// Camera
	//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	ball_collider = new SphereCollider(ball_pos, 2);
	//create a basic ellipsoid object
	//ellipsoid = glm::translate(glm::mat4(1), glm::vec3(0, 2, 0));
	//ellipsoid = glm::rotate(ellipsoid, 45.0f, glm::vec3(1, 0, 0));
	//ellipsoid = glm::scale(ellipsoid, glm::vec3(fRadius, fRadius, fRadius / 2));
	//inverse_ellipsoid = glm::inverse(ellipsoid);
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

	//glPushMatrix();
	//glMultMatrixf(glm::value_ptr(ellipsoid));
	//glutWireSphere(fRadius, iSlices, iStacks);
	//glPopMatrix();

	glPushMatrix(); // to draw the ball we use glutSolidSphere, and need to draw the sphere at the position of the ball
	glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]); // hence the translation of the sphere onto the ball position
	glColor3f(0.4f, 0.8f, 0.5f);
	glutSolidSphere(ball_radius - 0.1, 50, 50); // draw the ball, but with a slightly lower radius, otherwise we could get ugly visual artifacts of cloth penetrating the ball slightly
	glPopMatrix();

	cloth.draw();
	glutSwapBuffers();
}

void StepPhysics(float dt) {
	//ball_time++;
	//ball_pos[2] = cos(ball_time / 50.0) * 7;
	//ball_collider->setPos(ball_pos);

	cloth.addForce(gravity);
	cloth.timeStep(dt);
	cloth.CollisionDetection(ball_collider);
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