/*
 * Practice_Activity_2.cpp
 *
 *  Created on: Jul 12, 2020
 *      Author: Blake
 */

#include <GL/glut.h> //Include the freeglut header file


/*void display(){
	//specify the red, green, blue, and alpha values used when the color buffers are cleared. Color is set to Black with full Opacity
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); //Clears color
	glMatrixMode(GL_MODELVIEW); //Required for transforming
	glLoadIdentity();

	glTranslatef(-0.5f, 0.5f, 0.0f); //Translate next object drawn to the left .5 and up .5

	//Draw Star Triangles
	glBegin(GL_TRIANGLES);
		//12 oclock
		glColor3f(0.439216f, 0.858824f, 0.576471f); //Aquamarine
		glVertex3f( 0.0f,   0.3f, 0.0f);
		glVertex3f(-0.05f,  0.1f, 0.0f);
		glVertex3f( 0.05f,  0.1f, 0.0f);

		//9 oclock
		glVertex3f(-0.05f,  0.1f,   0.0f);
		glVertex3f(-0.175f, 0.1f,   0.0f);
		glVertex3f(-0.08f, -0.025f, 0.0f);

		//7 oclock
		glVertex3f(-0.075f,  0.0f,  0.0f);
		glVertex3f(-0.122f, -0.22f, 0.0f);
		glVertex3f(0.0f,    -0.09f, 0.0f);

		//5 oclock
		glVertex3f(0.075f,  0.0f,   0.0f);
		glVertex3f(0.122f, -0.22f,  0.0f);
		glVertex3f(0.0f,   -0.09f,  0.0f);

		//3 oclock
		glVertex3f(0.05f,  0.1f,   0.0f);
		glVertex3f(0.175f, 0.1f,   0.0f);
		glVertex3f(0.08f, -0.025f, 0.0f);
	glEnd();

	//Draw Star Body (Pentagon)
	glBegin(GL_POLYGON);
		glColor3f(0.439216f, 0.858824f, 0.576471f); //Aquamarine
		glVertex3f(-0.05f,  0.1f,   0.0f);
		glVertex3f(-0.08f, -0.025f, 0.0f);
		glVertex3f( 0.0f,  -0.09f,  0.0f);
		glVertex3f( 0.08f, -0.025f, 0.0f);
		glVertex3f( 0.05f,  0.1f,   0.0f);
	glEnd();

	glTranslatef(1.0f, 0.0f, 0.0f); //Translate next object drawn to the right 1

	//Draw Heptagon
	glBegin(GL_POLYGON);
		glColor3f(0.556863f, 0.137255f, 0.137255f); //Firebrick
		glVertex3f(0.0f, 0.2f, 0.0f);
		glVertex3f(-0.11f, 0.11f, 0.0f);
		glVertex3f(-.125f, -0.075f, 0.0f);
		glColor3f(0.556863f, 0.137255f, 0.419608f); //Maroon
		glVertex3f(-0.06f, -0.225f, 0.0f);
		glVertex3f(0.06f, -0.225f, 0.0f);
		glVertex3f(0.125f, -0.075f, 0.0f);
		glVertex3f(0.11f, 0.11f, 0.0f);
	glEnd();

	glTranslatef(-0.5f, -1.0f, 0.0f); //Translate next object drawn to the left .5 and down 1

	//Draw Pentagon
	glBegin(GL_POLYGON);
		glColor3f(0.419608f, 0.556863f, 0.137255f); //MediumForest Green
		glVertex3f(0.0f, 0.2f, 0.0f);
		glVertex3f(-0.15f, 0.05f, 0.0f);
		glVertex3f(-0.1f, -0.2f, 0.0f);
		glColor3f(0.137255f, 0.556863f, 0.419608f); //Sea Green
		glVertex3f(0.1f, -0.2f, 0.0f);
		glVertex3f(0.15f, 0.05f, 0.0f);
	glEnd();

	glFlush();
}*/

/*void reshape(GLsizei width, GLsizei height){
	if(height ==0) height =1;
		GLfloat aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);

	//Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION); //Create a projection matrix
	glLoadIdentity(); //Reset projection matrix
	gluPerspective(45.0f, aspect, 0.1f, 100.f); //Set perspective projection, fov, aspect, znear and zFar clipping
	}*/

	/*int main(int argc, char** argv){
		glutInit(&argc, argv); //Initializes the freeglut library
		glutInitWindowSize(1280, 720);
		glutInitWindowPosition(0,0);
		glutCreateWindow("Practice Activity 2");
		glutDisplayFunc(display);
		glutMainLoop();
		return 0;


	}*/
