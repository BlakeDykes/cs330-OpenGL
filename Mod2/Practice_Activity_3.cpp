/*
 * Practice_Activity_3.cpp
 *
 *  Created on: Jul 12, 2020
 *      Author: Blake
 */
/*
#include <GL/freeglut.h> //Include the freeglut header file

//depth and tessellation settings
void applyDepthSettings(){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background color to black and opaque
	glClearDepth(1.0f); //Specify the clear value for the depth buffer
	glEnable(GL_DEPTH_TEST); //Enable depth test - assists in updating the depth buffer
	glDepthFunc(GL_LEQUAL); //Specifies the depth comparison function used to compare each incoming pixel depth value with the depth value present in the depth buffer (LEQUAL - passes if the incoming depth value is less than or equal to the stored depth value)
	glShadeModel(GL_SMOOTH); //Specify smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //Indicates the quality of color, texture coordinate, and fog coordinate interpolation, and that the highest quality option should be chosen
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears color and dpeth buffers
	glMatrixMode(GL_MODELVIEW); //Activates the Model-View matrix

	glLoadIdentity(); //Rest the model-view matrix
	glTranslatef(0.0f, 0.0f, -8.0f); //Move model to the center of the world with a z depth of -8
	glRotatef(45.0f, 0.4f, -1.0f, -0.3f);

	//delimiter beginning for 3D primitive
	glBegin(GL_QUADS);

	//Set front-left face color (Red) and vertex Coordinates
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f( 1.0f, -1.0f, 1.0f);
	glVertex3f( 0.5f,  1.0f, 1.0f);
	glVertex3f(-0.5f,  1.0f, 1.0f);

	//Set front-right face color (Green) and vertex coordinates
	glColor3f(0.0f,  1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(0.5f,  1.0f, -1.0f);
	glVertex3f(0.5f,  1.0f,  1.0f);

	//Set top face color (Blue) and vertex coordinates
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.5f, 1.0f,  1.0f);
	glVertex3f( 0.5f, 1.0f,  1.0f);
	glVertex3f( 0.5f, 1.0f, -1.0f);
	glVertex3f(-0.5f, 1.0f, -1.0f);

	//Set back-left face color (Yellow) and vertex coordinates
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f,  1.0f);
	glVertex3f(-0.5f,  1.0f,  1.0f);
	glVertex3f(-0.5f,  1.0f, -1.0f);

	//Set back-right face color (Dull Green) and vertex coordinates
	glColor3f(0.0f, 0.5f, 0.4f);
	glVertex3f( 1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-0.5f,  1.0f, -1.0f);
	glVertex3f( 0.5f,  1.0f, -1.0f);

	//Set bottom face color (Grey) and vertex coordinates
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex3f( 1.0f, -1.0f,  1.0f);
	glVertex3f( 1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f,  1.0f);

	glEnd();

	glutSwapBuffers();

}

void reshape(GLsizei width, GLsizei height){
	if(height ==0) height =1;
		GLfloat aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);

	//Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION); //Create a projection matrix
	glLoadIdentity(); //Reset projection matrix
	//Set Perspective projection, fov, aspect, zNear and zFar clipping
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);

}

int main (int argc, char** argv){
	glutInit(&argc, argv); //Initializes the freeglut library
	glutInitDisplayMode(GLUT_DOUBLE); //Enable double buffered mode
	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Practice Activity 3");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	applyDepthSettings();
	glutMainLoop();
	return 0;
}*/


