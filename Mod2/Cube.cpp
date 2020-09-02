/*
 * Cube.cpp
 *
 *  Created on: Jul 9, 2020
 *      Author: Blake
 */

#include <GL/glut.h> //Include the freeglut header file

/*Depth and tessellation settings
void applyDepthSettings(){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background color to black and opaque
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}*/

/*Implements Display Callback Handler */
/*void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears color and depth buffers
	glMatrixMode(GL_MODELVIEW); //Activates the Model-View matrix

	glLoadIdentity(); //Reset the model-view matrix
	glTranslatef(-0.0f, 0.0f, -8.0f); //Move model to the center of the world with a z depth of -7
	glRotatef(150.0f, 0.0f, 1.0f, -0.4f); //Rotate the model on x, y, and z

	//delimiter beginning for cube
	glBegin(GL_QUADS);

		//Set front face color (Red) and vertex Coordinates
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f( 1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f,  1.0f);

		//Set right face color (Green) and vertex coordinates
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f( 1.0f,  1.0f, -1.0f);
		glVertex3f( 1.0f,  1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f);

		//Set top face color (Blue) and vertex coordinates
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f( 1.0f,  1.0f,  1.0f);
		glVertex3f( 1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f,  1.0f);

		//Set left face color (Yellow) and vertex coordinates
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f);

		//Set back face color (Dull Green) and vertex coordinates
		glColor3f(0.0f, 0.5f, 0.4f);
		glVertex3f( 1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f);

		//Set bottom face color (Grey) and vertex coordinates
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f( 1.0f, -1.0f,  1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f);

	glEnd(); //Ending delimiter for Cube primitive

	glutSwapBuffers(); //Swap front and back frame bufers (similar to glFlush)
}*/

/*void reshape (GLsizei width, GLsizei height){
	if (height == 0) height = 1;
		GLfloat aspect = (GLfloat)width /(GLfloat)height;

	glViewport(0, 0, width, height);

	//Set the aspect ration of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION); //Create a projection matrix
	glLoadIdentity(); //Reset projection matrix
	//Set perspective projection, fov, aspect, zNear and zFar clipping
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}*/

/*int main (int argc, char** argv){
	glutInit(&argc, argv); // Initializes the freeglut library
	glutInitDisplayMode(GLUT_DOUBLE); //Enable double buffered mode
	glutInitWindowSize(640, 480); //Specifies the window's width & height
	glutInitWindowPosition(0,0); //Specifies the position of the window's top-left corner
	glutCreateWindow("3D Cube"); //Create a window and title
	glutDisplayFunc(display); //Sets the display callback for the screen re-paint event
	glutReshapeFunc(reshape); //Sets the display callback for the window re-size event
	applyDepthSettings(); //Apply depth settings
	glutMainLoop(); //Enter the infinite event-processing loop
	return 0;

}*/


