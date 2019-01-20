//ToDo1:��ʵ��CPU��������ģ��

//��ʼ����չ
#include <GL/glew.h>

//glm��ѧ��
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> //for matrices
#include <glm/gtc/type_ptr.hpp>

//������صĿ�
#include <GL/freeglut.h>

#include <iostream>
using namespace std;

//���ڲ���
const int width = 1024, height = 1024;

//�۲����ͶӰ����Ĳ���
float dist = -23;
float rX = 15, rY = 0;
GLdouble MV[16];
GLint viewport[4];
GLdouble P[16];
glm::vec3 Up = glm::vec3(0, 1, 0), Right, viewDir;

void Init(GLvoid)
{
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	//ball_collider = new SphereCollider(ball_pos, 2);

	//�ڻ��ƶ����ʱ����Ĭ�ϵ���䷽ʽ, ������ʹ�õ����
	//ʹ��glPolygonMode����������ģʽ
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPointSize(5);

}

void OnIdle() {
	glutPostRedisplay();
}

void OnKey(unsigned char key, int, int) {

}

void OnShutdown() {
	//���������
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

//ͳ��֡���õ���Ϣ
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

//����Ĵ�С
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
//����Բ��һЩ����
glm::mat4 ellipsoid, inverse_ellipsoid;
float fRadius = 1;
int iStacks = 30;
int iSlices = 30;

void OnRender() {
	CalcFPS();
	//����Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//����۲����ͶӰ�����һЩ����
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

	//������


	glutSwapBuffers();
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