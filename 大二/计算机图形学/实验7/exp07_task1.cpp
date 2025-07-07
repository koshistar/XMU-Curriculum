#include<GL/glut.h>
#include<vector>
#include<iostream>
#include<cmath>
#include<random>
#include<time.h>

struct Point
{
	float x;
	float y;
	Point(float x, float y) :x(x), y(y) {}
	bool isNear(float mx, float my, float threhold = 0.03f)
	{
		return std::sqrt((x - mx) * (x - mx) + (y - my) * (y - my)) < threhold;
	}
};
struct Color
{
	int r, g, b;
	Color(int r, int g, int b) :r(r), g(g), b(b) {}
};
std::vector<Point> points;
std::vector<Point> bPoints;
std::vector<Color> colors;
float t = 0;
int selectedPoint = -1;
bool isDragging = false;

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	srand(time(0));
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluOrtho2D(0.0, 1.0, 0.0, 1.0);
	//points.push_back(Point(0.1f, 0.1f));
	//points.push_back(Point(0.5f, 0.9f));
	//points.push_back(Point(0.9f, 0.1f));
}

void drawPoint(const Point& p, float size = 5.0f)
{
	glPointSize(size);
	glBegin(GL_POINTS);
	glVertex2f(p.x, p.y);
	glEnd();
}
void drawLine(const Point& p1, const Point& p2)
{
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glEnd();
}

void recusive_besier(const std::vector<Point>& control_points, float t, int level)
{
	int n = control_points.size();
	if (n == 1)
	{
		bPoints.push_back(control_points[0]);
		drawPoint(control_points[0], 4.0f);
		return;
	}
	for (size_t i = 0; i < n - 1; i++)
	{
		glColor3f(colors[level].r / 255.0, colors[level].g / 255.0, colors[level].b / 255.0);
		drawLine(control_points[i], control_points[i + 1]);
	}
	std::vector<Point> new_points;
	for (int i = 0; i < n - 1; i++)
	{
		Point temp_point(control_points[i].x * (1 - t) + control_points[i + 1].x * t, control_points[i].y * (1 - t) + control_points[i + 1].y * t);
		new_points.push_back(temp_point);
		glColor3f(0.0, 1.0, 0.0);
		drawPoint(temp_point, 3.0f);
	}
	recusive_besier(new_points, t, level + 1);
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0.8, 0.8, 0.8);
	for (Point p : points)
	{
		drawPoint(p);
	}
	//drawPoint(Point(0.5f, 0.5f), 10.0f);
	//drawLine(Point(0.4f, 0.4f), Point(0.4f, 0.6f));
	if (bPoints.size() > 1)
	{
		glColor3f(1.0, 0, 0);
		glLineWidth(5);
		for (size_t i = 0; i < bPoints.size() - 1; i++)
		{
			drawLine(bPoints[i], bPoints[i + 1]);
		}
	}
	if (points.size() > 1)
	{
		glColor3f(0.8, 0.8, 0.8);
		glLineWidth(3);
		for (size_t i = 0; i < points.size() - 1; i++)
		{
			drawLine(points[i], points[i + 1]);
		}
	}
	if (points.size() > 1)
	{
		recusive_besier(points, t, 0);
	}
	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	float mx = (float)x / glutGet(GLUT_WINDOW_WIDTH);
	float my = 1.0 - (float)y / glutGet(GLUT_WINDOW_HEIGHT);
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			selectedPoint = -1;
			for (size_t i = 0; i < points.size(); i++)
			{
				if (points[i].isNear(mx, my))
				{
					selectedPoint = i;
					isDragging = true;
					break;
				}
			}
			if (selectedPoint == -1)
			{
				points.emplace_back(mx, my);
				if (points.size() > 1)
				{
					colors.emplace_back(rand() % 256, rand() % 256, rand() % 256);
				}
				t = 0;
				bPoints.clear();
				glutPostRedisplay();
			}
		}
		else if (state == GLUT_UP)
		{
			isDragging = false;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			if (!points.empty())
			{
				points.pop_back();
				selectedPoint = -1;
				if(!colors.empty())
					colors.pop_back();
				bPoints.clear();
				t = 0;
			}
		}
	}
}

void motion(int x, int y)
{
	if (isDragging && selectedPoint >= 0)
	{
		points[selectedPoint].x = (float)x / glutGet(GLUT_WINDOW_WIDTH);
		points[selectedPoint].y = 1.0f - (float)y / glutGet(GLUT_WINDOW_HEIGHT);
		glutPostRedisplay();
	}
}
void idleFunc()
{
	if (t >= 1.0f)
	{
		bPoints.clear();
		t = 0;
	}
	else
	{
		t += 0.0001f;
	}
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float aspect = (float)w / (float)h;
	if (w <= h)
	{
		gluOrtho2D(0.0, 1.0, 0.0, 1.0 / aspect);
	}
	else
	{
		gluOrtho2D(0.0, 1.0 * aspect, 0.0, 1.0);
	}

	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'r':points.clear();
		bPoints.clear();
		colors.clear();
		selectedPoint = -1;
		break;
	case 'd':if (selectedPoint >= 0)
	{
		points.erase(points.begin() + selectedPoint);
		selectedPoint = -1;
		if (!colors.empty())
		{
			colors.pop_back();
		}
		bPoints.clear();
		t = 0;
	}
	else if (!points.empty())
	{
		points.pop_back();
		selectedPoint = -1;
		if (!colors.empty())
		{
			colors.pop_back();
		}
		bPoints.clear();
		t = 0;
	}
			break;
	case 27:
		exit(0);
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
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