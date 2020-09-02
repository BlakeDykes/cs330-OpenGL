/*
 * Practice_Activity_4.cpp
 *
 *  Created on: Jul 18, 2020
 *      Author: Blake
 */

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

#define WINDOW_TITLE "Practice Activity 4"


//Variables for window width and height
int WindowWidth = 800, WindowHeight = 600;

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

void UInitialize(int, char*[]);
void UInitWindow(int, char*[]);
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateVBO(void);
void UCreateShaders(void);

//Vertex Shader Program Source Code
const GLchar * VertexShader = GLSL(440,
		 in layout(location = 0) vec4 vertex_Position;
in layout(location = 1) vec4 colorFromVBO; //for attribute 1 expect vec4 floats passed into the Vertex Shader
out vec4 colorFromVShader; //variable that will reference vertex colors passed to the Vertex Shader
		void main(){
			gl_Position = vertex_Position;	//Sends vertex positions to gl_Postion vec4
			colorFromVShader = colorFromVBO; //References vertex colors sent from the buffer
		}
	);

//Fragment Shader Program Source Code
const GLchar * FragmentShader = GLSL(440,
		in vec4 colorFromVShader; //Vertex colors from the vertex shader
out vec4 vertex_Color; //vec4 variable that will reference the vertex colors passed into the fragment shader from the vertex shader
	void main(){
		vertex_Color = colorFromVShader; //Sends the vertex colors to the GPU
	}
);

int main(int argc, char* argv[])
{
	UInitialize(argc, argv);
	glutMainLoop();
	exit(EXIT_SUCCESS);
}

void UInitialize(int argc, char* argv[])
{
	GLenum GlewInitResult;

	UInitWindow(argc, argv); //Creates the window

	//Checks glew status
	GlewInitResult = glewInit();
	if(GLEW_OK != GlewInitResult){
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	//Displya GPU OpenGL version
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

	UCreateVBO();
	UCreateShaders();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void UInitWindow(int argc, char* argv[]){

	//Initializes freeglut
	glutInit(&argc, argv);

	//Sets the window size
	glutInitWindowSize(WindowWidth, WindowHeight);

	//Memory buffer setup for display
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	//Creates a window with the macro placeholder title
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow); //Called when the window is resized
	glutDisplayFunc(URenderGraphics); //Renders graphics on the screen
}

void UResizeWindow(int width, int height)
{
	glViewport(0,0,width, height);
}

void URenderGraphics(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears the screen

	GLuint totalVerticies = 6;

	//Draw the triangles using indices
	glDrawElements(GL_TRIANGLES, totalVerticies, GL_UNSIGNED_SHORT, NULL);
	glutSwapBuffers();	//Flip the back buffer with the front buffer every frame
}

void UCreateVBO(void)
{
	GLfloat verts[] =
	{
			//Index 0
			-1.0f, 0.0f, //Far Left
			0.0f, 0.0f, 1.0f, 1.0f, //Blue

			//Index 1
			-0.5f, 0.0f, //Center Left
			0.0f, 1.0f, 0.0f, 1.0f, //Green

			//Index 2
			-1.0f, 1.0f, //Top Left
			1.0f, 0.0f, 0.0f, 1.0f, //Red

			//Index 3
			0.0f, 0.0f, //Center
			1.0f, 0.0f, 0.0f, 1.0f, //Red

			//Index 4
			0.0f, -1.0f, //Center Bottom
			0.0f, 1.0f, 0.0f, 1.0f //Green
	};

	float numVertices = sizeof(verts);

	GLuint myBufferID;
	glGenBuffers(1, &myBufferID); //Creates a buffer
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID); //Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, numVertices, verts, GL_STATIC_DRAW); // Sends vertex or coordiantes to GPU

	GLuint floatsPerVertex = 2;	//Number of coordinates per vertex
	glEnableVertexAttribArray(0);	//Specifies the initial position of the coordinates in the buffer

	// 6 floats before a new vertex/color combination
	GLint stride = sizeof(float) * 6;

	//Iterates through the vertexes (xy)
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);

	glEnableVertexAttribArray(1); //Specifies position 1 for the color values in the buffer

	//Iterates through the colors (rgba)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * 2));

	//Creates a buffer object for the indicies
	GLushort indicies [] =
	{
		0, 1, 2, //First Triangle
		3, 1, 4	//Second Triangle
	};
	float numIndicies = sizeof(indicies);
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndicies, indicies, GL_STATIC_DRAW);
}

void UCreateShaders(void)
{
	//Create a shader program object
	GLuint ProgramId = glCreateProgram();

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER); //Create a Vertex Shader Object
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER); //Create a Fragment Shader Object

	glShaderSource(vertexShaderId, 1, &VertexShader, NULL); //Retrieves the vertex shader source code
	glShaderSource(fragmentShaderId, 1, &FragmentShader, NULL); //Retrieves the fragment shader source code

	glCompileShader(vertexShaderId);
	glCompileShader(fragmentShaderId);

	//Attaches the vertex and fragment shaders to the shader program
	glAttachShader(ProgramId, vertexShaderId);
	glAttachShader(ProgramId, fragmentShaderId);

	glLinkProgram(ProgramId);	//Links the shader program
	glUseProgram(ProgramId);	//Uses the shader program
}












