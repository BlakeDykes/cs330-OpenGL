/*
 * TriangleOGL.cpp
 *
 *  Created on: Jul 9, 2020
 *      Author: Blake
 */

#include <windows.h>
#include <GL/freeglut.h>

/*Implements Display Callback Handler*/
void displayTriangle(){

	/*specify the red, green, blue, and alpha values used when the color buffers are cleared. Color is set to Black with full Opacity */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); //Uses the color buffer and sets the background color

	glBegin(GL_TRIANGLES); //Starting delimiter for triangle primitive

		glColor3f(1.0f, 0.0f, 0.0f); //Sets the vertex color to red using floating point values
		glVertex2f(-1.0f, 0.0f); //Sets the Red vertex position on on x and y using floating point values

		glColor3f(0.0f, 1.0f, 0.0f); //Sets the vertex color to green using floating point values
		glVertex2f(0.0f, 1.0f); //Sets the green vertex position on x and y using floating point values

		glColor3f(0.0f, 0.0f, 1.0f); //Sets the vertex color to blue using floating point values
		glVertex2f(1.0f, 0.0f); //Sets the blue vertex position on x and y using floating point values

	glEnd(); //Ending delimiter for Triangle primitive

	glFlush(); //Empties all buffers, and executes all issued commands to be accepted by the rendering engine.
}

/*Implements Display Callback Handler*/
void displayQuad(){

	/*Specify the red, green, blue, and alpha values used when the color buffers are cleared. Color is set to black with full opacity */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); //Uses the color buffer and sets the background color

	glBegin(GL_QUADS); //Starting delimiter for quad primitive

		glColor3f(1.0f, 0.0f, 0.0f); //Sets the vertex Color to Red using floating point values

		/*Sets 4 vertices to form the quad */
		glVertex2f(-0.5f, 0.5f);
		glVertex2f(0.5f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glVertex2f(-0.5f, -0.5f);

		glEnd(); //Ending delimiter for quad primitive
		glFlush(); //Empties all buffers, and executes all issued commands to be accepted by the rendering engine.
}

/*Implements Display Callback Handler*/
void displayPolygon(){

	/*Specify the red, green, blue, and alpha values used when the color buffers are cleared. Color is set to black with full opacity */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); //Uses the color buffer and sets the background color

	glBegin(GL_POLYGON); //Starting delimiter for polygon primitive

		glColor3f(0.0f, 1.0f, 0.5f); //Sets the vertex Colors to lime green using floating point values

		/*Sets 4 vertices to form a polygon*/
		glVertex2f(-0.5f, 0.5f);
		glVertex2f(0.0f, 1.0f);
		glVertex2f(0.5f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glVertex2f(-0.5f, -0.5f);

	glEnd(); //Ending delimiter for polygon primitive
	glFlush(); //Empties all buffers, and executes all issued commands to be accepted by the rendering engine.
}

/*Implements Display Callback Handler*/
void displayTriangleAndSquare(){
	/*Specify the red, green, blue, and alpha values used when the color buffers are cleared. Color is set to black with full opacity */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); //Uses the color buffer and sets the background color

	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);

		/*Sets 4 vertices to form the quad*/
		glVertex2f(0.0f, 0.0f);
		glVertex2f(0.5f, 0.0f);
		glVertex2f(0.5f, -0.5f);
		glVertex2f(0.0f, -0.5f);
	glEnd(); //Ending delimiter for quad primitive

	glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 0.0f, 1.0f); //Sets the vertex colors to blue using floating point values

		/*Sets 3 vertices to form the triangle */
		glVertex2f(-1.0f, 0.0f);
		glVertex2f(-0.5f, 0.5f);
		glVertex2f(0.0f, 0.0f);
	glEnd(); //Ending delimiter for triangle primitive

	glFlush();
}

/*Implements Display Callback Handler*/
void displayTransforms(){

	/*Specify the red, green, blue and alpha values used when the color buffers are cleared. Color is set to Black with full opacity */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT); //Uses the color buffer and sets the background color

	glMatrixMode(GL_MODELVIEW); //Required for transforming (translating, rotating, and scaling) graphic objects
	glLoadIdentity(); //Replaces the current matrix with the identity matrix. i.e. resets or refreshes the matrix for the next frame when the window is resized
	glTranslatef(-0.5f, 0.0f, 0.0f); // Translates all graphic object -0.5 in the x axis
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f); //Rotates all graphic objects 180 degrees in the z axis

	glBegin(GL_TRIANGLES); //Starting delimiter for triangle primitive
		glColor3f(1.0f, 0.0f, 0.0f); //sets the vertex color to red using floating point values
		glVertex2f(-0.5f, 0.0f); //Sets the red vertex position on x and y using floating point values

		glColor3f(0.0f, 0.5f, 0.0f); //Sets the vertex color to green using floating point values
		glVertex2f(0.0f, 0.5f); //Sets the green vertex position on x and y using floating point values

		glColor3f(0.0f, 0.0f, 0.5f); //sets the vertex color to blue using floating point values
		glVertex2f(0.5f, 0.0f); //Sets the blue vertex position on x and y using floating point values

	glEnd(); //Ending delimiter for triangle primitive

	glFlush(); //Empties all buffers, and executes all issued commands to be accepted by the rendering engine.
}


/* Main function
int main(int argc, char** argv){
	glutInit(&argc, argv); //Initializes the freeglut library
	glutCreateWindow("OpenGl Triangle"); //Create a window and title
	glutInitWindowSize(1280, 720); //Specifies the window's width & height
	glutInitWindowPosition(0,0); //Specifies the position of the window's top-left corner
	glutDisplayFunc(displayTransforms); //Sets the display callback for the current window
	glutMainLoop();
	return 0;

}
*/
