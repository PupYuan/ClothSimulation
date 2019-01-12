#ifdef _WIN32
#include <windows.h> 
#endif

#include "util.h"
#include <GL/glut.h>
#include "Cloth.h"
#include "Collider.h"


vec3 ball_pos(7, -5, 0); // the center of our one ball
float ball_radius = 2; // the radius of our one ball
SphereCollider* ball_collider;
Cloth cloth1(14, 10, 55, 45); // one Cloth object of the Cloth class

//����ͳ��֡�ʵı���
#define MAX_PATH 100
char info[MAX_PATH] = { 0 };
LARGE_INTEGER frequency;        // ticks per second
LARGE_INTEGER t1, t2;           // ticks
double frameTimeQP = 0;
float frameTime = 0;

//����ģ���֡��
float timeStep = 1 / 60.0f;
double accumulator = timeStep;

float startTime = 0, fps = 0;
int totalFrames = 0;
float currentTime = 0;

//TODO1:����һ������
//TODO2:����һ����
//TODO3���̶����ߵĹ�������
//TODO4��ģ������
//TODO5������Springs
//TODO6��ģ����Բ��ϵ���ײ

void init(GLvoid)
{
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);

	//���ù���
	glEnable(GL_LIGHTING);

	//����0
	glEnable(GL_LIGHT0);
	GLfloat lightPos[4] = { -1.0,1.0,0.5,0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat *)&lightPos);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	ball_collider = new SphereCollider(ball_pos, 2);

}

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


void RenderOneFrame(void) {
	//ͳ��֡�ʵ���Ϣ
	CalcFPS();

	//drawing
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();

	glDisable(GL_LIGHTING); // drawing some smooth shaded background - because I like it ;)
	glBegin(GL_POLYGON);
	glColor3f(0.8f, 0.8f, 1.0f);
	glVertex3f(-200.0f, -100.0f, -100.0f);
	glVertex3f(200.0f, -100.0f, -100.0f);
	glColor3f(0.4f, 0.4f, 0.8f);
	glVertex3f(200.0f, 100.0f, -100.0f);
	glVertex3f(-200.0f, 100.0f, -100.0f);
	glEnd();
	glEnable(GL_LIGHTING);

	glTranslatef(-6.5, 6, -9.0f); // move camera out and center on the cloth
	glRotatef(25, 0, 1, 0); // rotate a bit to see the cloth from the side

	cloth1.drawShaded(); // finally draw the cloth with smooth shading

	glPushMatrix(); // to draw the ball we use glutSolidSphere, and need to draw the sphere at the position of the ball
	glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]); // hence the translation of the sphere onto the ball position
	glColor3f(0.4f, 0.8f, 0.5f);
	glutSolidSphere(ball_radius - 0.1, 50, 50); // draw the ball, but with a slightly lower radius, otherwise we could get ugly visual artifacts of cloth penetrating the ball slightly
	glPopMatrix();

	// Perform the buffer swap to display back buffer
	glutSwapBuffers();
}

float ball_time = 0; // counter for used to calculate the z position of the ball below
//������
void StepPhysics(float dt) {

	//����ģ�ⲻӦ�÷�����Ⱦѭ������
	ball_time++;
	ball_pos.f[2] = cos(ball_time / 50.0) * 7;
	ball_collider->setPos(ball_pos);

	cloth1.addForce(Vec3(0, -10, 0)); // add gravity each frame, pointing down
	cloth1.timeStep(dt); // calculate the particle positions of the next frame
	cloth1.CollisionDetection(ball_collider);

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

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h == 0)
		gluPerspective(80, (float)w, 1.0, 5000.0);
	else
		gluPerspective(80, (float)w / (float)h, 1.0, 5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

void arrow_keys(int a_keys, int x, int y)
{
	switch (a_keys) {
	case GLUT_KEY_UP:
		glutFullScreen();
		break;
	case GLUT_KEY_DOWN:
		glutReshapeWindow(1280, 720);
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);


	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);

	glutCreateWindow("Cloth Simulation Tutorial");
	init();
	glutDisplayFunc(RenderOneFrame);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(arrow_keys);
	glutIdleFunc(OnIdle);

	glutMainLoop();
}