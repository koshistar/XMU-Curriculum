#include<GL/glut.h>
#include<cmath>
GLfloat timeValue = 0;
bool isReverse = false;
GLfloat x2 = 0.625f;
GLfloat y2 = 1.25f;
GLfloat x3 = 1.25f;
void myinit()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-0.5, 1.5, -0.5, 1.5);
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	y2 += (timeValue - 0.5f) * 0.001f;
	x3 += (timeValue - 0.5f) * 0.001f;
	x2 = x3 / 2;
	GLfloat vertices[3][2] = { {0.0,0.0},{x2,y2},{x3,0.0} };
	GLfloat p[2] = { 7.5,5.0 };
	GLfloat pColor[3] = { 0.5,0.5,0.5 };
	GLfloat vertexColors[3][3] = { {1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0} };
	int j, k;
	int rand();

	GLfloat rotationAngle = fmod(timeValue * 360.0, 360.0f);
	glTranslatef(0.625f, 0.417f, 0.0f);//大三角形中心
	glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);//绕z轴转
	glTranslatef(-0.625f, -0.417f, 0.0f);

	glBegin(GL_POINTS);
	
	glPointSize(5.0f);
	//glColor3ub(80, 130, 254);
	
	for (k = 0; k < 5000; k++)
	{
		j = rand() % 3;

		p[0] = (p[0] + vertices[j][0]) / 2;
		p[1] = (p[1] + vertices[j][1]) / 2;

		pColor[0] = (pColor[0] + vertexColors[j][0]) / 2;
		pColor[1] = (pColor[1] + vertexColors[j][1]) / 2;
		pColor[2] = (pColor[2] + vertexColors[j][2]) / 2;

		GLfloat r = (pColor[0] + timeValue) > 1.0f ? pColor[0] - 1.0f : pColor[0] + timeValue;
		GLfloat g = (pColor[1] + timeValue) > 1.0f ? pColor[1] - 1.0f : pColor[1] + timeValue;
		GLfloat b = (pColor[2] + timeValue) > 1.0f ? pColor[2] - 1.0f : pColor[2] + timeValue;

		//glColor3f(rand() % 256 / 255.0f, rand() % 256 / 255.0f, rand() % 256 / 255.0f);  // 随机颜色
		//glColor3fv(pColor);
		glColor3f(r, g, b);
		glVertex2fv(p);
	}

	glEnd();
	glutSwapBuffers();
}
void updateValue()
{
	if (isReverse)
		timeValue -= 0.0001f;
	else
		timeValue += 0.0001f;
	if (timeValue > 1.0f)
		isReverse = true;
	if (timeValue < 0)
		isReverse = false;
	display();
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	//Create a single buffered window
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	myinit();
	glutDisplayFunc(display);
	glutIdleFunc(updateValue);

	glutMainLoop();
}
