#include <GL/glut.h>
#include <iostream>
#include<math.h>
#include<algorithm>

#define PI 3.1415926535

typedef struct materialStruct
{
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess[1];
}materialStruct;

materialStruct brassMaterials = {
	{0.33,0.22,0.03,1.0},
	{0.78,0.57,0.11,1.0},
	{0.99,0.91,0.81,1.0},
	{27.8}
};
materialStruct redPlasticMaterials = {
	{0.3,0.0,0.0,1.0},
	{0.6,0.0,0.0,1.0},
	{0.8,0.6,0.6,1.0},
	{32.0}
};

materialStruct whiteShinyMaterials = {
	{1.0,1.0,1.0,1.0},
	{1.0,1.0,1.0,1.0},
	{1.0,1.0,1.0,1.0},
	{100.0}
};

typedef struct lightingStruct
{
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
}lightingStruct;

lightingStruct whiteLighting = {
	{0.0,0.0,0.0,1.0},
	{1.0,1.0,1.0,1.0},
	{1.0,1.0,1.0,1.0}
};
lightingStruct coloredLighting =
{
	{0.2,0.0,0.0,1.0},
	{0.0,1.0,0.0,1.0},
	{0.0,0.0,1.0,1.0}
};

GLfloat light0_pos[4] = { 0.90,0.90,2.25,0.00 };

materialStruct* currentMaterials;
lightingStruct* currentLighting;

typedef struct sliderStruct
{
	float x, y, width, height;
	float minVal, maxVal;
	float* value;
	bool isDragging;
}sliderStruct;

float color1[] = { 1.0,1.0,1.0 };
float colorLighting[] = { 0.5f,1.0f,0.5f };
float colorMaterial[] = { 1.0f,0.5f,0.5f };
sliderStruct lightingSlider[12];
sliderStruct materialSlider[13];

/* 绘制正方体 */
GLfloat Vertices[8][3] = { {1,-1,1},{1,-1,-1},{-1,-1,-1},{-1,-1,1},{1,1,1},{1,1,-1},{-1,1,-1},{-1,1,1} }; //顶点坐标
GLint Faces[6][4] = { {0,3,7,4},{0,1,5,4},{1,2,6,5},{2,3,7,6},{4,5,6,7},{0,1,2,3} }; // 面顶点组索引
GLfloat Colors[6][3] = { {1, 0, 0},{1, 1, 0} ,{0, 1, 0},{0, 0, 1},{1, 0, 1},{0, 1, 1} }; // 颜色

/* 物体旋转 */
GLfloat beta = 0;
GLfloat delta = 1.0; // 旋转速度

/* 键盘交互 */
bool enableMove = true; //是否启用相机移动
GLfloat moveSpeed = 0.3;
GLfloat cameraPosX = 0.0f, cameraPosY = 0.0f, cameraPosZ = 10.0f;
GLfloat lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = -15.0f;

/* 鼠标交互 */
GLfloat latestX = 0, latestY = 0;
GLint w = 800, h = 800;

void material(materialStruct* materials)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materials->ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materials->diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materials->specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materials->shininess);
}
void light(lightingStruct* lighting)
{
	glLightfv(GL_LIGHT0, GL_AMBIENT, lighting->ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lighting->diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lighting->specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
}

float toGlX(int x) { return (x / (float)w) * 2 - 1; }
float toGlY(int y) { return 1 - (y / (float)h) * 2; }

void initialSlider(sliderStruct* slider, float x, float y, float width, float height, float minVal, float maxVal, float* targetValue)
{
	//slider = new sliderStruct{ x,y,width,height,minVal,maxVal,targetValue,false };
	slider->x = x;
	slider->y = y;
	slider->width = width;
	slider->height = height;
	slider->minVal = minVal; 
	slider->maxVal = maxVal;
	slider->value = targetValue;
	slider->isDragging = false;
	//std::cout << *targetValue << " " << *slider->value << " " << slider->x << " " << slider->y << std::endl;
}
void drawSlider(sliderStruct *slider,float color1[],float color2[])
{
	//绘制滑条轨道
	glColor3fv(color1);
	glBegin(GL_QUADS);
	glVertex2f(slider->x, slider->y);
	glVertex2f(slider->x + slider->width, slider->y);
	glVertex2f(slider->x + slider->width,  slider->y + slider->height);
	glVertex2f(slider->x, slider->y + slider->height);
	glEnd();

	//滑块位置计算
	float sliderPos = slider->x + (*slider->value - slider->minVal) / (slider->maxVal - slider->minVal) * slider->width;

	//绘制滑条
	glColor3fv(color2);
	glBegin(GL_QUADS);
	glVertex2f(sliderPos - 0.03f,  slider->y - 0.01f);
	glVertex2f( sliderPos + 0.03f,  slider->y - 0.01f);
	glVertex2f( sliderPos + 0.03f,  slider->y + slider->height + 0.01f);
	glVertex2f( sliderPos - 0.03f,  slider->y + slider->height + 0.01f);
	glEnd();
}

void initSlider()
{
	initialSlider(&lightingSlider[0], -1, -0.88f, 0.3f, 0.08f, 0, 1.0, &currentLighting->ambient[0]);
	initialSlider(&lightingSlider[1], -1, -0.72f, 0.3f, 0.08f, 0, 1.0, &currentLighting->ambient[1]);
	initialSlider(&lightingSlider[2], -1, -0.56f, 0.3f, 0.08f, 0, 1.0, &currentLighting->ambient[2]);
	initialSlider(&lightingSlider[3], -1, -0.40f, 0.3f, 0.08f, 0, 1.0, &currentLighting->ambient[3]);
	initialSlider(&lightingSlider[4], -1, -0.24f, 0.3f, 0.08f, 0, 1.0, &currentLighting->diffuse[0]);
	initialSlider(&lightingSlider[5], -1, -0.08f, 0.3f, 0.08f, 0, 1.0, &currentLighting->diffuse[1]);
	initialSlider(&lightingSlider[6], -1, 0.08f, 0.3f, 0.08f, 0, 1.0, &currentLighting->diffuse[2]);
	initialSlider(&lightingSlider[7], -1, 0.24f, 0.3f, 0.08f, 0, 1.0, &currentLighting->diffuse[3]);
	initialSlider(&lightingSlider[8], -1, 0.40f, 0.3f, 0.08f, 0, 1.0, &currentLighting->specular[0]);
	initialSlider(&lightingSlider[9], -1, 0.56f, 0.3f, 0.08f, 0, 1.0, &currentLighting->specular[1]);
	initialSlider(&lightingSlider[10], -1, 0.72f, 0.3f, 0.08f, 0, 1.0, &currentLighting->specular[2]);
	initialSlider(&lightingSlider[11], -1, 0.88f, 0.3f, 0.08f, 0, 1.0, &currentLighting->specular[3]);

	initialSlider(&materialSlider[0], 0.7f, 0.96, 0.3f, 0.08f, 0, 1.0, &currentMaterials->ambient[0]);
	initialSlider(&materialSlider[1], 0.7f, 0.80, 0.3f, 0.08f, 0, 1.0, &currentMaterials->ambient[1]);
	initialSlider(&materialSlider[2], 0.7f, 0.64, 0.3f, 0.08f, 0, 1.0, &currentMaterials->ambient[2]);
	initialSlider(&materialSlider[3], 0.7f, 0.48, 0.3f, 0.08f, 0, 1.0, &currentMaterials->ambient[3]);
	initialSlider(&materialSlider[4], 0.7f, 0.32, 0.3f, 0.08f, 0, 1.0, &currentMaterials->diffuse[0]);
	initialSlider(&materialSlider[5], 0.7f, 0.16, 0.3f, 0.08f, 0, 1.0, &currentMaterials->diffuse[1]);
	initialSlider(&materialSlider[6], 0.7f, 0, 0.3f, 0.08f, 0, 1.0, &currentMaterials->diffuse[2]);
	initialSlider(&materialSlider[7], 0.7f, -0.16, 0.3f, 0.08f, 0, 1.0, &currentMaterials->diffuse[3]);
	initialSlider(&materialSlider[8], 0.7f, -0.32, 0.3f, 0.08f, 0, 1.0, &currentMaterials->specular[0]);
	initialSlider(&materialSlider[9], 0.7f, -0.48, 0.3f, 0.08f, 0, 1.0, &currentMaterials->specular[1]);
	initialSlider(&materialSlider[10], 0.7f, -0.64, 0.3f, 0.08f, 0, 1.0, &currentMaterials->specular[2]);
	initialSlider(&materialSlider[11], 0.7f, -0.80, 0.3f, 0.08f, 0, 1.0, &currentMaterials->specular[3]);
	initialSlider(&materialSlider[12], 0.7f, -0.96, 0.3f, 0.08f, 0, 128.0, &currentMaterials->shininess[0]);

	for (int i = 0; i < 12; i++)
	{
		drawSlider(&lightingSlider[i], color1, colorLighting);
	}
	for (int i = 0; i < 13; i++)
	{
		drawSlider(&materialSlider[i], color1, colorMaterial);
	}
}
void init(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	currentMaterials = &redPlasticMaterials;
	material(currentMaterials);
	currentLighting = &whiteLighting;
	light(currentLighting);

	initSlider();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 0.0, 0.0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); //开启深度测试

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80, 1, 0.1, 60); // 选择透视视景体
	//glOrtho(10, -10, 10, -10, 0.1, 60);//选择正交视景体（相机前进后退物体大小不变）
	glMatrixMode(GL_MODELVIEW);

	latestX = w / 2;
	latestY = h / 2;
	glutWarpPointer(w / 2, h / 2);

	glEnable(GL_NORMALIZE);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
		lookAtX, lookAtY, lookAtZ,
		0, 1, 0);//设置相机坐标

	glRotatef(beta, 1.0, -1.0, -1.0); //旋转

	for (int i = 0; i < 6; i++)//绘制六面体
	{
		glColor3fv(Colors[i]);
		glBegin(GL_POLYGON);
		for (int j = 0; j < 4; j++)
		{
			glVertex3fv(Vertices[Faces[i][j]]);
			glNormal3fv(Vertices[Faces[i][j]]);
		}
		glEnd();
	}

	//绘制UI
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);

	for (int i = 0; i < 12; i++)
	{
		drawSlider(&lightingSlider[i], color1, colorLighting);
	}
	for (int i = 0; i < 13; i++)
	{
		drawSlider(&materialSlider[i], color1, colorMaterial);
	}

	glEnable(GL_LIGHTING);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	light(currentLighting);
	material(currentMaterials);
	glPopMatrix();
	glutSwapBuffers();

	glFinish();
}


void reshape(int ww, int hh)
{
	glViewport(0, 0, (GLsizei)ww, (GLsizei)hh);
	w = ww;
	h = hh;
}

void idlefunc()
{
	Sleep(3);
	//改变动画参数
	beta += delta;
	if (beta > 360) beta -= 360;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	GLfloat move = enableMove ? moveSpeed : 0;
	switch (key) {
	case 27: exit(0); break;//esc退出
	case 's':
		cameraPosZ += move;
		lookAtZ += move;;//注意，相机坐标和朝向坐标要同步增加或减少
		break;
	case 'w':
		cameraPosZ -= move;
		lookAtZ -= move;
		break;
	case 'd':
		cameraPosX += move;
		lookAtX += move;
		break;
	case 'a':
		cameraPosX -= move;
		lookAtX -= move;
		break;
	case 'q':
		cameraPosY += move;
		lookAtY += move;
		break;
	case 'e':
		cameraPosY -= move;
		lookAtY -= move;
		break;
	case 'l':
		enableMove = !enableMove; break; //禁用相机移动
	case 'b':
		currentMaterials = &brassMaterials;
		//material(currentMaterials);
		initSlider();
		break;
	case 'n':
		currentMaterials = &redPlasticMaterials;
		//material(currentMaterials);
		initSlider();
		break;
	case 'm':
		currentMaterials = &whiteShinyMaterials;
		//material(currentMaterials);
		initSlider();
		break;
	case 'o':
		currentLighting = &whiteLighting;
		//light(currentLighting);
		initSlider();
		break;
	case 'p':
		currentLighting = &coloredLighting;
		//light(currentLighting);
		initSlider();
		break;
	}
}

void mouse(int x, int y)
{
	if (enableMove)
	{
		lookAtX += (x - latestX) / 25;//根据鼠标指针坐标的位置，修改相机朝向坐标
		lookAtY += (latestY - y) / 25;
	}
	latestX = x;
	latestY = y;
}

void mouseClick(int button, int state, int x, int y)
{
	float mx = toGlX(x);
	float my = toGlY(y);

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			for (sliderStruct &slider:lightingSlider)
			{
				float sliderPos = slider.x + (*slider.value - slider.minVal) / (slider.maxVal - slider.minVal) * slider.width;
				if (mx >= sliderPos - 0.03f && mx <= sliderPos + 0.03f && my >= slider.y - 0.01f && my <= slider.y + slider.height + 0.01f)
				{
					slider.isDragging = true;
				}
				else
				{
					slider.isDragging = false;
				}
			}
			for (sliderStruct &slider : materialSlider)
			{
				float sliderPos = slider.x + (*slider.value - slider.minVal) / (slider.maxVal - slider.minVal) * slider.width;
				if (mx >= sliderPos - 0.03f && mx <= sliderPos + 0.03f && my >= slider.y - 0.01f && my <= slider.y + slider.height + 0.01f)
				{
					slider.isDragging = true;
				}
				else
				{
					slider.isDragging = false;
				}
			}
		}
	}
}

void motion(int x, int y)
{
	for (sliderStruct& slider : lightingSlider)
	{
		if (slider.isDragging)
		{
			float mx = toGlX(x);
			mx = std::max(slider.x, std::min(slider.x + slider.width, mx));

			*slider.value = slider.minVal + (mx - slider.x) / slider.width * (slider.maxVal - slider.minVal);
			//std::cout << *slider.value << " " << currentLighting->ambient[1] << std::endl;
			glutPostRedisplay();
			return;
		}
	}
	for (sliderStruct& slider : materialSlider)
	{
		if (slider.isDragging)
		{
			float mx = toGlX(x);
			mx = std::max(slider.x, std::min(slider.x + slider.width, mx));

			*slider.value = slider.minVal + (mx - slider.x) / slider.width * (slider.maxVal - slider.minVal);
			glutPostRedisplay();
			return;
		}
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idlefunc);
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutMouseFunc(mouseClick);
	glutMotionFunc(motion);
	glutMainLoop();
	return 0;
}
