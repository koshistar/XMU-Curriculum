#include<GL/glut.h>
#include<cstdio>
#include<cmath>
#include<fstream>
#include<cstring>
#include<vector>
using namespace std;
typedef struct VERTEX_3D
{
	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
}VERTEX3D;
typedef struct TRIANGLE
{
	int A;
	int B;
	int C;
}TRIANGLE;
int vertexNum;
int faceNum;
vector<VERTEX3D> vertexes;
vector<TRIANGLE> faces;
float centerX = 0;
float centerY = 0;
float centerZ = 0;

float theta = 0;
//GLUquadric* sphere;
void loadPLY(const char* fileName)
{
	ifstream file(fileName);
	//file.open(fileName, ios::in);
	char* str = new char[100];
	if (!file)
	{
		printf("Can not load file %s.", fileName);
	}
	for (int i = 0; i < 3; i++)
	{
		file.getline(str, 100);
	}
	file >> str;
	file >> str;
	file >> vertexNum;
	//file >> str;

	for (int i = 0; i < 7; i++)
	{
		file.getline(str, 100);
	}
	file >> str;
	file >> str;
	file >> faceNum;
	//file >> str;
	for (int i = 0; i < 3; i++)
	{
		file.getline(str, 100);
	}
	vertexes.resize(vertexNum);
	faces.resize(faceNum);
	//printf("%d %d", vertexNum, faceNum);
	for (int i = 0; i < vertexNum; i++)
	{
		file >> vertexes[i].x;
		file >> vertexes[i].y;
		file >> vertexes[i].z;
		file >> vertexes[i].nx;
		file >> vertexes[i].ny;
		file >> vertexes[i].nz;
		centerX += vertexes[i].x;
		centerY += vertexes[i].y;
		centerZ += vertexes[i].z;
	}
	int vertexesNumInFace;
	for (int i = 0; i < faceNum; i++)
	{
		file >> vertexesNumInFace;
		file >> faces[i].A;
		file >> faces[i].B;
		file >> faces[i].C;
	}
	centerX /= vertexNum;
	centerY /= vertexNum;
	centerZ /= vertexNum;
	//printf("%f %f %f\n", vertexes[0].x, vertexes[0].y, vertexes[0].z);
	file.close();
}
//void drawBackground()
//{
//	glDisable(GL_LIGHTING);
//	//½¥±ä±³¾°
//	glBegin(GL_QUADS);
//	glColor3f(0.91, 0.91, 0.91);
//	glVertex3f(-1, -1, -0.5);
//	glVertex3f(1, -1, -0.5);
//	glColor3f(0.87, 0.88, 0.89);
//	glVertex3f(1, 1, -0.5);
//	glVertex3f(-1, 1, -0.5);
//	glEnd();
//	glEnable(GL_LIGHTING);
//}
void init()
{
	loadPLY("lizhenxiout.ply");
	glClearColor(0.87, 0.88, 0.89, 1.0);

	/*sphere = gluNewQuadric();
	gluQuadricOrientation(sphere, GLU_INSIDE);
	gluQuadricNormals(sphere, GLU_SMOOTH);*/

	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[] = { 0.5,0.5,0.5,1.0 };
	GLfloat diffuse[] = { 0.9,0.9,0.9,1.0 };
	//GLfloat specular[] = { 1.0,1.0,1.0,1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}
void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//drawBackground();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//»æÖÆ±³¾°(Ìì¿ÕºÐ)
	//glDepthMask(GL_FALSE);
	//glPushMatrix();
	//glScalef(5000, 5000, 5000);
	//glColor3f(0.89, 0.90, 0.90);
	//gluSphere(sphere, 1.0, 32, 32);
	//glPopMatrix();
	//glDepthMask(GL_TRUE);

	gluLookAt(centerX, centerY, centerZ + 3000.0, centerX, centerY, centerZ, 0, 1.0, 0);

	GLfloat light_position[] = { sin(theta),1.0,cos(theta),0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	/*GLfloat mat_specular[] = { 1.0,1.0,1.0,1.0 };
	GLfloat mat_shininess[] = {50.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);*/

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glColor3f(0.32, 0.39, 0.38);
	glVertexPointer(3, GL_FLOAT, sizeof(VERTEX3D), &vertexes[0].x);
	glNormalPointer(GL_FLOAT, sizeof(VERTEX3D), &vertexes[0].nx);

	glBegin(GL_TRIANGLES);
	//for (int i = 0; i < faceNum; i++)
	//{
	//	glBegin(GL_TRIANGLES);

	//	glNormal3f(vertexes[faces[i].A].nx, vertexes[faces[i].A].ny, vertexes[faces[i].A].nz);
	//	glNormal3f(vertexes[faces[i].B].nx, vertexes[faces[i].B].ny, vertexes[faces[i].B].nz);
	//	glNormal3f(vertexes[faces[i].C].nx, vertexes[faces[i].C].ny, vertexes[faces[i].C].nz);

	//	glVertex3f(vertexes[faces[i].A].x, vertexes[faces[i].A].y, vertexes[faces[i].A].z);
	//	glVertex3f(vertexes[faces[i].B].x, vertexes[faces[i].B].y, vertexes[faces[i].B].z);
	//	glVertex3f(vertexes[faces[i].C].x, vertexes[faces[i].C].y, vertexes[faces[i].C].z);

	//	glEnd();
	//}
	for (size_t i = 0; i < faceNum; i++)
	{
		glArrayElement(faces[i].A);
		glArrayElement(faces[i].B);
		glArrayElement(faces[i].C);
	}
	glEnd();
	glutSwapBuffers();
}
void idleFunc()
{
	theta += 0.01f;
	if (theta > 360.0f)
	{
		theta = 0;
	}
	glutPostRedisplay();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 10000.0);

	glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char** argv)
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
	/*glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);*/
	glutMainLoop();

	return 0;
}
