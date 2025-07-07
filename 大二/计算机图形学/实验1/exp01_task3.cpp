#include <GL/glut.h>
#include<cmath>
#include<ctime>
const GLfloat my_PI = 4 * atan(1);
const GLfloat colors[5][3] = {
	{0.0f, 0.505f, 0.784f},
	{1.0f, 0.780f, 0.173f},
	{0.0f, 0.0f, 0.0f},
	{0.0f, 0.620f, 0.376f},
	{0.875f, 0.0f, 0.141f}
};
void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glShadeModel(GL_FLAT); //使用单一颜色
}

void drawTorus(int n, GLfloat x, GLfloat y, GLfloat inr, GLfloat outr, GLfloat from, GLfloat to)
{
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= n; i++)
	{
		GLfloat angle = from + (to - from) / n * i;
		GLfloat cosAngle = std::cos(angle);
		GLfloat sinAngle = std::sin(angle);
		glVertex3f(inr * cosAngle + x, inr * sinAngle + y, 0.0f);
		glVertex3f(outr * cosAngle + x, outr * sinAngle + y, 0.0f);
	}
	glEnd();
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); 
	//glTranslatef(1.0f, -1.0f, 0.0f);						// Move 1.5 Units Left And 6.0 Units Into The Screen.
	//glColor3f(0.0f, 0.0f, 0.0f);
	//drawTorus(80, 0.0f, 0.0f, 0.6f, 0.7f, 0.0f, 2 * my_PI);
	//glColor3f(colors[0][0], colors[0][1], colors[0][2]);
	//drawTorus(80, -2.0f, 1.0f, 0.6f, 0.7f, 0, my_PI * 2);
	//glColor3f(colors[1][0], colors[1][1], colors[1][2]);
	//drawTorus(80, -1.3f, 0.3f, 0.6f, 0.7f, 0, my_PI * 2);
	//glColor3f(colors[2][0], colors[2][1], colors[2][2]);
	//drawTorus(80, -0.6f, 1.0f, 0.6f, 0.7f, 0, my_PI * 2);
	//glColor3f(colors[3][0], colors[3][1], colors[3][2]);
	//drawTorus(80, 0.1f, 0.3f, 0.6f, 0.7f, 0, my_PI * 2);
	//glColor3f(colors[4][0], colors[4][1], colors[4][2]);
	//drawTorus(80, 0.8f, 1.0f, 0.6f, 0.7f, 0, my_PI * 2);
	
	glColor3f(colors[0][0], colors[0][1], colors[0][2]);
	drawTorus(10, -2.0f, 1.0f, 0.6f, 0.7f, 0, my_PI / 4);
	drawTorus(10, -2.0f, 1.0f, 0.6f, 0.7f, my_PI * 7 / 4, my_PI * 2);
	glColor3f(colors[1][0], colors[1][1], colors[1][2]);
	drawTorus(60, -1.3f, 0.3f, 0.6f, 0.7f, my_PI / 4, my_PI * 7 / 4);
	glColor3f(colors[0][0], colors[0][1], colors[0][2]);
	drawTorus(60, -2.0f, 1.0f, 0.6f, 0.7f, my_PI / 4, my_PI * 7 / 4);

	glColor3f(colors[2][0], colors[2][1], colors[2][2]);
	drawTorus(10, -0.6f, 1.0f, 0.6f, 0.7f, my_PI * 7 / 4, my_PI * 2);
	drawTorus(60, -0.6f, 1.0f, 0.6f, 0.7f, 0, my_PI * 3 / 2);
	glColor3f(colors[1][0], colors[1][1], colors[1][2]);
	drawTorus(10, -1.3f, 0.3f, 0.6f, 0.7f, my_PI * 7 / 4, my_PI * 2);
	drawTorus(10, -1.3f, 0.3f, 0.6f, 0.7f, 0, my_PI / 4);

	glColor3f(colors[3][0], colors[3][1], colors[3][2]);
	drawTorus(60, 0.1f, 0.3f, 0.6f, 0.7f, my_PI / 4, my_PI * 7 / 4);
	glColor3f(colors[2][0], colors[2][1], colors[2][2]);
	drawTorus(10, -0.6f, 1.0f, 0.6f, 0.7f, my_PI * 3 / 2, my_PI * 7 / 4);

	glColor3f(colors[4][0], colors[4][1], colors[4][2]);
	drawTorus(30, 0.8f, 1.0f, 0.6f, 0.7f, 0, my_PI * 3 / 4);
	drawTorus(30, 0.8f, 1.0f, 0.6f, 0.7f, my_PI * 5 / 4, my_PI * 2);
	glColor3f(colors[3][0], colors[3][1], colors[3][2]);
	drawTorus(10, 0.1f, 0.3f, 0.6f, 0.7f, my_PI * 7 / 4, my_PI * 2);
	drawTorus(10, 0.1f, 0.3f, 0.6f, 0.7f, 0, my_PI / 4);
	glColor3f(colors[4][0], colors[4][1], colors[4][2]);
	drawTorus(20, 0.8f, 1.0f, 0.6f, 0.7f, my_PI * 3 / 4, my_PI * 5 / 4);
	//glBegin(GL_TRIANGLES);								// Drawing Using Triangles
	//glVertex3f(0.0f, 1.0f, 0.0f);					// Top
	//glVertex3f(-1.0f, -1.0f, 0.0f);					// Bottom Left
	//glVertex3f(1.0f, -1.0f, 0.0f);					// Bottom Right
	//glEnd();											// Finished Drawing The Triangle
	//glTranslatef(3.0f, 0.0f, 0.0f);						// Move Right 3 Units
	//glBegin(GL_QUADS);									// Draw A Quad
	//glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
	//glVertex3f(1.0f, 1.0f, 0.0f);					// Top Right
	//glVertex3f(1.0f, -1.0f, 0.0f);					// Bottom Right
	//glVertex3f(-1.0f, -1.0f, 0.0f);					// Bottom Left
	//glEnd();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	gluPerspective(45.0, (double)w / (double)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}

int main(int argc, char** argv)
{
	srand((unsigned)time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100); //窗口处于屏幕左上角的位置
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0;
}