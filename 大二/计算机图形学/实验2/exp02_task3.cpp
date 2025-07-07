#include <GL/glut.h>
#include<math.h>

#define DEG_TO_RAD 0.017453
const GLfloat r = 1.0f;
GLfloat angle = 0;
GLfloat beta = 0;
GLfloat eyeX = 0.0f;
GLfloat eyeY = 0.0f;
GLfloat eyeZ = 5.0f;
GLfloat centerX = 0.0f;
GLfloat centerY = 0.0f;
GLfloat centerZ = -1.0f;
GLfloat upX = 0.0f;
GLfloat upY = 1.0f;
GLfloat upZ = 0.0f;

float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;

bool isCameraLocked = false;
int lastX = 0, lastY = 0;
bool mouseLeftDown = false;
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);

	glShadeModel(GL_FLAT);

	glEnable(GL_DEPTH_TEST);
}
void updateCamera()
{
	//前向向量
	centerX = sin(cameraAngleX) * cos(cameraAngleY);
	centerY = sin(cameraAngleY);
	centerZ = -cos(cameraAngleY) * cos(cameraAngleX);
	//标准化前向向量
	GLfloat frontLength = sqrt(centerX * centerX + centerY * centerY + centerZ * centerZ);
	centerX /= frontLength;
	centerY /= frontLength;
	centerZ /= frontLength;
	//计算右向量 （与上向量（0,1,0）进行叉积运算
	GLfloat rightX = centerY * 0.0f - centerZ * 1.0f;
	GLfloat rightY = centerZ * 0.0f - centerX * 0.0f;
	GLfloat rightZ = centerX * 1.0f - centerY * 0.0f;
	//标准化右向量
	GLfloat rightLength = sqrt(rightX * rightX + rightY * rightY + rightZ * rightZ);
	rightX /= rightLength;
	rightY /= rightLength;
	rightZ /= rightLength;
	//计算上向量
	upX = rightY * centerZ - rightZ * centerY;
	upY = rightZ * centerX - rightX * centerZ;
	upZ = rightX * centerY - rightY * centerX;
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, eyeX + centerX, eyeY + centerY, eyeZ + centerZ, upX, upY, upZ);

	glColor3f(1.0, 0.0, 0.0);
	//绘制球体
	/*for (GLfloat fai = 0; fai < 180; fai += 10.0f)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (GLfloat theta = 0; theta <= 360; theta += 10.0f)
		{
			glVertex3f(r * sin(fai * DEG_TO_RAD) * cos(theta * DEG_TO_RAD), r * sin(fai * DEG_TO_RAD) * sin(theta * DEG_TO_RAD), r * cos(fai * DEG_TO_RAD));
			glVertex3f(r * sin(fai * DEG_TO_RAD + 10.0 * DEG_TO_RAD) * cos(theta * DEG_TO_RAD), r * sin(fai * DEG_TO_RAD + 10.0 * DEG_TO_RAD) * sin(theta * DEG_TO_RAD), r * cos(fai * DEG_TO_RAD + 10.0 * DEG_TO_RAD));
		}
		glEnd();
	}*/
	glRotatef(angle, 1.0, 0.0, 0.0);
	glRotatef(angle / 2, 0.0, 1.0, 0.0);
	//绘制经线
	for (GLfloat theta = 0; theta <= 360; theta += 15.0f)
	{
		glBegin(GL_LINE_STRIP);
		for (GLfloat fai = 0; fai <= 180; fai += 5.0f)
		{
			glVertex3f(r * sin(fai * DEG_TO_RAD) * cos(theta * DEG_TO_RAD), r * sin(fai * DEG_TO_RAD) * sin(theta * DEG_TO_RAD), r * cos(fai * DEG_TO_RAD));
		}
		glEnd();
	}
	//绘制纬线
	for (GLfloat fai = 0; fai < 180; fai += 5.0f)
	{
		glBegin(GL_LINE_LOOP);
		for (GLfloat theta = 0; theta <= 360; theta += 15.0f)
		{
			glVertex3f(r * sin(fai * DEG_TO_RAD) * cos(theta * DEG_TO_RAD), r * sin(fai * DEG_TO_RAD) * sin(theta * DEG_TO_RAD), r * cos(fai * DEG_TO_RAD));
		}
		glEnd();
	}
	glEnd();
	glutSwapBuffers();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
void idleFunc(void)
{
	angle += 0.01f;
	if (angle > 720.0f)
	{
		angle -= 720.0f;
	}
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'l':
		isCameraLocked = !isCameraLocked;
		break;
	}
	if (isCameraLocked)
		return;
	//计算右向量 （与上向量进行叉积运算）
	GLfloat rightX = centerY * upZ - centerZ * upY;
	GLfloat rightY = centerZ * upX - centerX * upZ;
	GLfloat rightZ = centerX * upY - centerY * upX;
	switch (key) {
	case 'w':
		eyeX += centerX * 0.1f;
		eyeY += centerY * 0.1f;
		eyeZ += centerZ * 0.1f;
		break;
	case 's':
		eyeX -= centerX * 0.1f;
		eyeY -= centerY * 0.1f;
		eyeZ -= centerZ * 0.1f;
		break;
	case 'a':
		eyeX -= rightX * 0.1f;
		eyeY -= rightY * 0.1f;
		eyeZ -= rightZ * 0.1f;
		break;
	case 'd':
		eyeX += rightX * 0.1f;
		eyeY += rightY * 0.1f;
		eyeZ += rightZ * 0.1f;
		break;
	case 'q':
		eyeY += 0.1f;
		break;
	case 'e':
		eyeY -= 0.1f;
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	if (isCameraLocked)
		return;
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			lastX = x;
			lastY = y;
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
		{
			mouseLeftDown = false;
		}
	}
}
void motion(int x, int y)
{
	if (mouseLeftDown && !isCameraLocked)
	{
		int dx = x - lastX;
		int dy = lastY - y;

		cameraAngleX += dx * 0.005f;
		cameraAngleY += dy * 0.005f;

		lastX = x;
		lastY = y;

		updateCamera();
	}
}

int main(int argc,char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutIdleFunc(idleFunc);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();

	return 0;
}