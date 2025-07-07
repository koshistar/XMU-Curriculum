#include <GL/glut.h>
#include<cmath>
#include<ctime>
const GLfloat my_PI = 4 * atan(1);
GLfloat theta = 0.0f;
GLfloat red[7];
GLfloat green[7];
GLfloat blue[7];
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT); //使用单一颜色
}

void drawCircle(int n, GLfloat x, GLfloat y, GLfloat r,int slice = 0,GLfloat theta=0)
{
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x, y, 0.0f);
	int perSlice;
	int t = 0;
	if (slice != 0)
	{
		perSlice = n / slice;
	}
	int curSlice = perSlice;
	for (int i = 0; i <= n; i++)
	{
		glVertex3f(r * std::cos(2 * my_PI / n * i + theta), r * std::sin(2 * my_PI / n * i + theta), 0.0f);
		if (slice != 0 && i>=curSlice)
		{
			curSlice += perSlice;
			glColor3f(red[t], green[t], blue[t]);
			t++;
		}
	}
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix

	glTranslatef(-1.5f, 0.0f, -6.0f);						// Move 1.5 Units Left And 6.0 Units Into The Screen.
	drawCircle(500, 0.0f, 0.0f, 1.0f, 7, theta);
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
	glEnd();

	glFlush();
}

void rotate(void)
{
	theta += 0.001f;
	if (theta > 2 * my_PI)
		theta -= 2 * my_PI;
	display();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
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
	for (int i = 0; i < 7; i++)
	{
		red[i] = rand() / (double)(RAND_MAX);
		green[i] = rand() / (double)(RAND_MAX);
		blue[i] = rand() / (double)(RAND_MAX);
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100); //窗口处于屏幕左上角的位置
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(rotate);
	glutMainLoop();
	return 0;
}
