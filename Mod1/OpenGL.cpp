/*
 * OpenGL.cpp
 *
 *  Created on: Jul 1, 2020
 *      Author: Blake
 */

/*
 * OGL02Animation.cpp: 3D Shapes with animation
 */
#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h

/* Implements Display Callback Handler*/
void displayGraphics() {

	/*Specify the red, green, blue, and alpha values used when the color buffers are cleared. Color is set to Black with full Opacity. */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); //Uses the color buffer and sets the background color of the window
	glFlush(); //Empties all buffers and executes all issued commands to be accepted by the rendering engine.
}

/* Main function required for Immediate mode */
int main(int argc, char** argv)
{
	glutInit(&argc, argv); //Initializes the freeGLUT library
	glutCreateWindow("Blake Dykes - CS 330"); //Create a window and title
	glutInitWindowPosition(0,0); //Specifies the window's width and height
	glutInitWindowPosition(0,0); //Specifies the position of the window's top left corner
	glutDisplayFunc(displayGraphics); //Sets the display callback for the current window
	glutMainLoop(); //Enters the GLUT event processing loop
	return 0; //Exits main function

}

