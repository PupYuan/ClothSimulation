#ifdef _WIN32
#include <windows.h> 
#endif

#include "util.h"
#include <GL/glut.h>
#include "Cloth.h"


Vec3 ball_pos(7, -5, 0); // the center of our one ball
float ball_radius = 2; // the radius of our one ball
Cloth cloth1(14, 10, 55, 45); // one Cloth object of the Cloth class
//TODO1:画出一个球来
//TODO2:画出一条布
void init(GLvoid)
{
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

}

float ball_time = 0; // counter for used to calculate the z position of the ball below
void display(void) {
	ball_time++;
	ball_pos.f[2] = cos(ball_time / 50.0) * 7;

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(-6.5, 6, -9.0f); // move camera out and center on the cloth
	glRotatef(25, 0, 1, 0); // rotate a bit to see the cloth from the side

	cloth1.drawShaded(); // finally draw the cloth with smooth shading

	glPushMatrix(); // to draw the ball we use glutSolidSphere, and need to draw the sphere at the position of the ball
	glTranslatef(ball_pos.f[0], ball_pos.f[1], ball_pos.f[2]); // hence the translation of the sphere onto the ball position
	glColor3f(0.4f, 0.8f, 0.5f);
	glutSolidSphere(ball_radius - 0.1, 50, 50); // draw the ball, but with a slightly lower radius, otherwise we could get ugly visual artifacts of cloth penetrating the ball slightly
	glPopMatrix();

	// Perform the buffer swap to display back buffer
	glutSwapBuffers();
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
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(arrow_keys);

	glutMainLoop();
}