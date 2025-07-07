#include <GL/glut.h>
#include<math.h>

#define DEG_TO_RAD 0.017453
const GLfloat RR = 6.0; //camera rad
GLfloat beta = 0;
GLfloat eyeX = 0.0f;
GLfloat eyeY = 0.0f;
GLfloat eyeZ = RR;
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
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);

	//以下语句二选一，看看效果
	//glShadeModel(GL_SMOOTH);
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
	// glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(RR * sin(beta * DEG_TO_RAD), RR * cos(beta * DEG_TO_RAD) * cos(45.0), RR * cos(beta * DEG_TO_RAD) * sin(45),
	//	0.0, 0.0, 0.0,
	//	0.0, 1.0, 0.0);
	
	//centerX +=sin(cameraAngleY) * cos(cameraAngleX);
	//centerY +=sin(cameraAngleX);
	//centerZ +=cos(cameraAngleY) * sin(cameraAngleX);
	gluLookAt(eyeX, eyeY, eyeZ,
		eyeX+centerX, eyeY+centerY, eyeZ+centerZ,
		upX, upY, upZ);
	////glRotatef(sin(beta * DEG_TO_RAD), 1.0, 0.0, 0.0);
	////glRotatef(cos(beta * DEG_TO_RAD) * cos(45.0), 0.0, 1.0, 0.0);
	////glRotatef(cos(beta * DEG_TO_RAD) * sin(45.0), 0.0, 0.0, 1.0);
	glRotatef(-beta, 0.0, 1.0, 0.0);
	glRotatef(-beta * 0.5, 1.0, 0.0, 0.0);
	//glRotatef(360 - beta, 0.0, 0.0, 1.0);
	//以下语句绘制三角形，请自行改成绘制立方体
	glBegin(GL_TRIANGLES);
	// face A, part 1
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, -1);
	glEnd();

	// face A, part 2
	glBegin(GL_TRIANGLES);
	glColor3f(0.5, 0, 0);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, 1, 1);
	glEnd();
	// face B, part 1
	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 0);
	glVertex3f(1, 1, -1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, -1);
	glEnd();
	// face B, part 2
	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 0.5);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, -1, 1);
	glEnd();
	// face C
	glBegin(GL_TRIANGLES);
	glColor3f(0, 1, 0);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(-1, -1, 1);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0, 0.5, 0);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, 1);
	glEnd();
	// face D
	glBegin(GL_TRIANGLES);
	glColor3f(0, 0, 1);
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, 1, -1);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0, 0, 0.5);
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, -1, 1);
	glEnd();
	// face E
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 1);
	glVertex3f(-1, 1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(-1, -1, -1);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.5, 0, 0.5);
	glVertex3f(-1, 1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, 1, -1);
	glEnd();
	//face F
	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, -1, -1);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.5, 0.5, 0.5);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glEnd();
	//glFlush(); /* clear buffers */
	glutSwapBuffers();
}


void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	//以下语句设置相机投影参数
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//以下两句二选一，看看效果
	//glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	//glOrtho(-4.0, 4.0, -4.0, 4.0, -4.0, 4.0);
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
	//glFrustum(-1.0, 1.0, -1.0, 1.0, 0.3, 10.0);


	//以下语句设置相机和物体的相对方位
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


void idlefunc()
{
	//改变动画参数
	beta += 0.05;
	if (beta > 720) beta -= 720;
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
		/*eyeZ += 0.1f;
		centerZ += 0.1f;*/
		eyeX += centerX * 0.1f;
		eyeY += centerY * 0.1f;
		eyeZ += centerZ * 0.1f;
		break;
	case 's':
		/*eyeZ -= 0.1f;
		centerZ -= 0.1f;*/
		eyeX -= centerX * 0.1f;
		eyeY -= centerY * 0.1f;
		eyeZ -= centerZ * 0.1f;
		break;
	case 'a':
		/*eyeX -= 0.1f;
		centerX -= 0.1f;*/
		eyeX -= rightX * 0.1f;
		eyeY -= rightY * 0.1f;
		eyeZ -= rightZ * 0.1f;
		break;
	case 'd':
		/*eyeX += 0.1f;
		centerX += 0.1f;*/
		eyeX += rightX * 0.1f;
		eyeY += rightY * 0.1f;
		eyeZ += rightZ * 0.1f;
		break;
	case 'q':
		eyeY += 0.1f;
		//centerY += 0.1f;
		break;
	case 'e':
		eyeY -= 0.1f;
		//centerY -= 0.1f;
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

int main(int argc, char** argv)
{
	/*
		1. draw cube:
			draw each face with 2 triangles
		2. adjust view point:
			modify params in gluLookAt
		3. try to move camera:
			3.1 open depth
				glEnable(GL_DEPTH_TEST);
				glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			3.2 change viewpoint dynamicly
				3.2.1 load camera in display
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
					gluLookAt();
				3.2.2 change angle in idlefunc
					note pre-define constant and angle2rad
			3.3 use double buffer to smooth
				glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
				glutSwapBuffers();
	*/
	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idlefunc);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();
	return 0;
}
