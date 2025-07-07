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
void loadPLY(const char* fileName)
{
	ifstream file(fileName);
	//file.open(fileName, ios::in);
	char* str=new char[100];
	if (!file)
	{
		printf("Can not load file %s.", fileName);
	}
	for (int i = 0; i < 3; i++)
	{
		file.getline(str,100);
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
	printf("%d %d", vertexNum, faceNum);
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
void init()
{
	loadPLY("lizhenxiout.ply");
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(centerX, centerY, centerZ + 3000.0, centerX, centerY, centerZ, 0, 1.0, 0);

	//GLfloat light_position[] = { centerX,centerY,centerZ + 1000.0,1.0 };
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glColor3f(1.0, 0.0, 0.0);
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
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.1, 10000.0);

	glMatrixMode(GL_MODELVIEW);
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
	//glutIdleFunc(idleFunc);
	glutReshapeFunc(reshape);
	/*glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);*/
	glutMainLoop();

	return 0;
}
