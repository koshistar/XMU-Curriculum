#include<GL/glut.h>
#include<vector>
#include<iostream>
#include<cmath>
#include<random>
#include<time.h>
#define PI 3.1415926
struct Point
{
	float x;
	float y;
	float z;
	Point() = default;
	Point(float x, float y,float z) :x(x), y(y), z(z) {}
	bool isNear(float mx, float my, float mz, float threhold = 0.03f)
	{
		return std::sqrt((x - mx) * (x - mx) + (y - my) * (y - my) + (z - mz) * (z - mz)) < threhold;
	}
};
const int n = 5;
std::vector<std::vector<Point>> points;
std::vector<std::vector<Point>> bPoints;
std::vector<std::vector<Point>> surface;
std::vector<Point> temp;
float t = 0;
int T = 0;
int selectedPointI = -1, selectedPointJ = -1;
bool isDragging = false;
float theta = 0, fai = 0, beta = 0;
void drawPoint(const Point& p, float size = 5.0f)
{
	glPointSize(size);
	glBegin(GL_POINTS);
	glVertex3f(p.x, p.y, p.z);
	glEnd();
}
void drawLine(const Point& p1, const Point& p2)
{
	glBegin(GL_LINES);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glEnd();
}
void drawAxis()
{
	glEnable(GL_LINE_WIDTH);
	glLineWidth(3.0);
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidCube(0.05);
	glBegin(GL_LINES);
	glVertex3f(-1.8f, 0.0f, 0.0f);
	glVertex3f(1.8f, 0.0f, 0.0f);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);
	glutSolidCube(0.05);
	glBegin(GL_LINES);
	glVertex3f(0.0f, -1.8f, 0.0f);
	glVertex3f(0.0f, 1.8f, 0.0f);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	glutSolidCube(0.05);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, -1.8f);
	glVertex3f(0.0f, 0.0f, 1.8f);
	glEnd();
}
float* setColor(float z)
{
	float normalized = (z + 1.0f) / (1.0f + 1.0f);
	normalized = std::max(0.0f, std::min(1.0f, normalized));

	if (normalized < 0.25f)
	{
		float color[] = { 0.0f,normalized * 4.0f,1.0f };
		return color;
	}
	else if (normalized < 0.5f)
	{
		float color[] = { 0.0f,1.0f,1.0f - (normalized - 0.25f) * 4.0f };
		return color;
	}
	else if (normalized < 0.75f)
	{
		float color[] = { (normalized - 0.5f) * 4.0f,1.0f,0.0f };
		return color;
	}
	else
	{
		float color[] = { 1.0f, 1.0f - (normalized - 0.75f) * 4.0f,0.0f };
		return color;
	}
}

void recusive_besier(const std::vector<Point>& control_points, float t)
{
	int n = control_points.size();
	if (n == 1)
	{
		//bPoints[cnt].push_back(control_points[0]);
		temp.push_back(control_points[0]);
		//drawPoint(control_points[0], 4.0f);
		return;
	}
	//for (size_t i = 0; i < n - 1; i++)
	//{
	//	glColor3f(colors[level].r / 255.0, colors[level].g / 255.0, colors[level].b / 255.0);
	//	drawLine(control_points[i], control_points[i + 1]);
	//}
	std::vector<Point> new_points;
	for (int i = 0; i < n - 1; i++)
	{
		Point temp_point(control_points[i].x * (1 - t) + control_points[i + 1].x * t, control_points[i].y * (1 - t) + control_points[i + 1].y * t, control_points[i].z * (1 - t) + control_points[i + 1].z * t);
		new_points.push_back(temp_point);
		//glColor3f(0.0, 1.0, 0.0);
		//drawPoint(temp_point, 3.0f);
	}
	recusive_besier(new_points, t);
}
void drawSurface()
{
	for (int i = 0; i < points.size(); i++)
	{
		temp.clear();
		for(float j=0;j<=1;j+=0.02f)
			recusive_besier(points[i], j);
		bPoints.push_back(temp);
		//for (int j = 0; j < bPoints[i].size(); j++)
		//{
		//	std::cout << bPoints[i][j].x << " ";
		//}
		//std::cout << std::endl;
	}
	for (int i = 0; i < bPoints[0].size(); i++)
	{
		std::vector<Point> s;
		for (int j = 0; j < bPoints.size(); j++)
		{
			s.push_back(bPoints[j][i]);
		}

		temp.clear();
		for (float j = 0; j <= 1; j += 0.02f)
		{
			recusive_besier(s, j);
		}
		surface.push_back(temp);
	}
}


void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	srand(time(0));

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	for (int i = 0; i < n; i++)
	{
		std::vector<Point> tempPoint;
		for (int j = 0; j < n; j++)
		{
			tempPoint.push_back(Point(2 * rand() / float(RAND_MAX) - 1, 2 * rand() / float(RAND_MAX) - 1, 2 * rand() / float(RAND_MAX) - 1));
		}
		points.push_back(tempPoint);
	}
	drawSurface();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(2, 2, 2, 0, 0, 0, 0, 1, 0);
	glRotatef(theta / PI * 180, 0, 1, 0);
	glRotatef(fai / PI * 180, 1, 0, 0);
	glRotatef(beta / PI * 180, 0, 0, 1);
	drawAxis();
	glColor3f(0.8, 0.8, 0.8);
	for (std::vector<Point> ps : points)
	{
		for(Point p:ps)
			drawPoint(p);
	}
	for (int i = 0; i < bPoints.size(); i++)
	{
		glColor3f(0.0f, 0.0f, 0.0f);
		glLineWidth(5);
		for (int j = 0; j < bPoints[i].size()-1; j++)
		{
			drawLine(bPoints[i][j], bPoints[i][j + 1]);
		}
		glColor3f(0.5f, 0.5f, 0.5f);
		glLineWidth(3);
		for (int j = 0; j < points[i].size() - 1; j++)
		{
			drawLine(points[i][j], points[i][j + 1]);
		}
	}

	for (int i = 0; i < t; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < surface[i].size(); j++)
		{
			glColor3fv(setColor(surface[i][j].z));
			glVertex3f(surface[i][j].x, surface[i][j].y, surface[i][j].z);
			if (i > 0)
			{
				glColor3fv(setColor(surface[i - 1][j].z));
				glVertex3f(surface[i-1][j].x, surface[i-1][j].y, surface[i-1][j].z);
			}
		}
		glEnd();
	}

	glPointSize(5.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < bPoints.size(); i++)
	{
		drawPoint(bPoints[i][T]);
	}
	glColor3f(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < bPoints.size() - 1; i++)
	{
		drawLine(bPoints[i][T], bPoints[i + 1][T]);
	}

	glEnable(GL_LINE_WIDTH);
	glLineWidth(2.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < surface[T].size() - 1; i++)
	{
		drawLine(surface[T][i], surface[T][i + 1]);
	}

	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			//将屏幕坐标转化为OpenGL坐标
			GLint viewport[4];
			GLdouble modelview[16], projection[16];
			GLfloat winX, winY, winZ;
			GLdouble posX, posY, posZ;

			glGetIntegerv(GL_VIEWPORT, viewport);
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
			glGetDoublev(GL_PROJECTION_MATRIX, projection);

			winX = (float)x;
			winY = viewport[3] - (float)y;
			glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
			gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

			selectedPointI = -1;
			selectedPointJ = -1;
			for (size_t i = 0; i < points.size(); i++)
			{
				for (size_t j = 0; j < points[i].size(); j++)
				{
					if (points[i][j].isNear(posX, posY,posZ))
					{
						selectedPointI = i;
						selectedPointJ = j;
						isDragging = true;
						break;
					}
				}
			}
		}
		else if (state == GLUT_UP)
		{
			isDragging = false;
		}
	}
}

void motion(int x, int y)
{
	if (isDragging && selectedPointI >= 0 && selectedPointJ >= 0)
	{
		//将屏幕坐标转化为OpenGL坐标
		GLint viewport[4];
		GLdouble modelview[16], projection[16];
		GLfloat winX, winY, winZ;
		GLdouble posX, posY, posZ;

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);

		winX = (float)x;
		winY = viewport[3] - (float)y;
		glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

		points[selectedPointI][selectedPointJ].x = posX;
		points[selectedPointI][selectedPointJ].y = posY;
		points[selectedPointI][selectedPointJ].z = posZ;
		
		bPoints.clear();
		surface.clear();
		drawSurface();
		glutPostRedisplay();
	}
}
void idleFunc()
{
	t += 0.001f;
	T = (int)t;
	if (T >= surface.size() - 1)
		t = 0;
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (double)w / (double)h, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':fai -= 0.1f; break;
	case 's':fai += 0.1f; break;
	case 'a':theta -= 0.1f; break;
	case 'd':theta += 0.1f; break;
	case 'q':beta -= 0.1f; break;
	case 'e':beta += 0.1f; break;
	case 27:
		exit(0);
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	init();
	glutIdleFunc(idleFunc);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();
	return 0;
}