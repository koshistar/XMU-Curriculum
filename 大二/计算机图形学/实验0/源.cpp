#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>         /* glut.h includes gl.h and glu.h*/
#endif
#include<cmath>
void display(void)

{
	/* clear window */

	glClear(GL_COLOR_BUFFER_BIT);


	/* draw unit square polygon */

	//glbegin(gl_polygon);
	//glvertex2f(-0.5, -0.5);
	//glvertex2f(-0.5, 0.5);
	//glvertex2f(0.5, 0.5);
	//glvertex2f(0.5, -0.5);
	//GLfloat vertices[] = { 0,0.5,-0.5,-0.5,0.5,-0.5 };
	//GLfloat colors[] = { 0.5,0.5,1.0,0.0,1.0,0.0,0.5,0.5,0.5 };
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	//glVertexPointer(2, GL_FLOAT, 0, vertices);
	//glColorPointer(3, GL_FLOAT, 0, colors);
	//glBegin(GL_TRIANGLES);
	//glArrayElement(0);
	//glArrayElement(1);
	//glArrayElement(2);
	GLfloat radius = 0.2;//°ë¾¶
	GLfloat x = 0.25, y = 0.75;//Ô²ÐÄ
	glLineWidth(5.0f);
	glBegin(GL_LINE_LOOP);
	for (GLfloat fValue = 0; fValue <= 2 * std::acos(-1); fValue += 0.0001f)
	{
		glVertex2f(x + radius * std::cos(fValue), y + radius * sin(fValue));
	}
	glEnd();
	/* flush GL buffers */

	glFlush();

}


void init()
{

	/* set clear color to black */

	 	//glClearColor (0.0, 0.0, 0.0, 0.0); 
		/* set clear color to red */
	glClearColor(1.0, 0.0, 0.0, 0.0);
	/* set fill  color to white */

	 	//glColor3f(1.0, 1.0, 1.0); 
	glColor3f(0.0, 0.0, 1.0);
		/* set fill  color to blue */
	/* set up standard orthogonal view with clipping */
	/* box as cube of side 2 centered at origin */
	/* This is default view and these statement could be removed */

		 glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);  
}

int main(int argc, char** argv)
{

	/* Initialize mode and open a window in upper left corner of screen */
	/* Window title is name of program (arg[0]) */

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("simple");
	glutDisplayFunc(display);
	init();
	glutMainLoop();

}
