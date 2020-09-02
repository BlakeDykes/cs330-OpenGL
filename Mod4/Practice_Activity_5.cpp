/*
 * Practice_Activity_5.cpp
 *
 *  Created on: Jul 22, 2020
 *      Author: Blake
 */

#include <time.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define WINDOW_TITLE "Practice Activity 5"

//Shader Program Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

GLint
	windowHeight = 600,
	windowWidth = 800;

GLuint
	VBO,
	VAO,
	EBO,
	shaderProgram,
	texture;

float rotation = 0;
clock_t lastTime = 0;

void ResizeWindow(int, int);
void Render(void);
void CreateShaders(void);
void CreateBuffers(void);
void ExitOnGLError(const char*);

//Vertex Shader Source Code
const GLchar * vertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position;	//xyz coordinates from VAO
	layout (location = 1) in vec3 color;	//RGB values from VBO

	out vec3 colorEx;	//RGB values exported to fragment shader source code

	uniform mat4 projection;
	uniform mat4 model;
	uniform mat4 view;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f);	//transform vertices to clip coordinates
		colorEx = color;
	}
);

const GLchar * fragmentShaderSource = GLSL(330,
		in vec3 colorEx;
		out vec4 gpuColor;

	void main()
		{
			gpuColor = vec4(colorEx, 1.0);	//Sends RGBA values to GPU
		}
);

int main(int argc, char* argv[])
{
	//Create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(windowWidth, windowHeight);
	int windowHandle = glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(ResizeWindow);

	if(windowHandle < 1)
	{
		fprintf(stderr, "ERROR: Could not create a new rendering window.\n");
		exit(EXIT_FAILURE);
	}

	GLenum glewInitResult;

	//Initialize glew
	glewExperimental = GL_TRUE;
	glewInitResult = glewInit();
	if(GLEW_OK != glewInitResult)
	{
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(glewInitResult));
		exit(EXIT_FAILURE);
	}

	//Only display depth values that are less than the stored depth value
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	CreateShaders();
	CreateBuffers();

	glUseProgram(shaderProgram);
	ExitOnGLError("ERROR: Could not use the program");

	//Set background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	glutDisplayFunc(Render);


	glutMainLoop();

	//Delete array/buffer objects
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return 0;
}

//Resize the window
void ResizeWindow(int w, int h)
{
	windowHeight = h;
	windowWidth = w;
	glViewport(0, 0, windowWidth, windowHeight);
}

//Renders graphics
void Render(void)
{
	//Clears the screeen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);

	//get radians for rotation about y-axis
	clock_t now = clock();

	if(lastTime == 0)
		lastTime = now;

	rotation += 45.0f * ((float)(now - lastTime) / CLOCKS_PER_SEC);
	float radians = glm::radians(rotation);
	lastTime = now;

	//Transform in model space
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, radians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	//transform the camera
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, -0.5f, -5.0f));

	//create a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);

	//Get uniform matrices from shader source code
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	ExitOnGLError("ERROR: Could not get shader uniform locations");

	//Passes matrix values into the source code
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//Redraw
	glutPostRedisplay();

	//Draw the triangles
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	//Deactivate the VAO
	glBindVertexArray(0);

	//Flips the back buffer with the front buffer
	glutSwapBuffers();

}

void CreateShaders(void)
{
	//Create shader program
	shaderProgram = glCreateProgram();
	ExitOnGLError("ERROR: Could not create the shader program");

	//Create and compile vertex shader from source
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//Create and compile fragment shader from source
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//Attach vertex and fragment shaders to shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//Link shader program
	glLinkProgram(shaderProgram);
	ExitOnGLError("ERROR: Could not link the shader program");

	//Delete the vertex and fragment shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void CreateBuffers(void)
{
	GLfloat vertices[] =
		{
				-0.5f, 0.0f,  0.5f,		0.98f, 0.4f, 0.7f,	//Back Left 0
				 0.5f, 0.0f,  0.5f, 	0.0f, 0.0f,  1.0f,	//Back Right 1
				-0.5f, 0.0f, -0.5f,		1.0f, 0.0f,  0.0f,	//Front Left 2
				 0.5,  0.0f, -0.5f,		0.0f, 1.0f,  0.0f,	//Front Right 3
				 0.0f, 1.0f,  0.0f,		1.0f, 1.0f,  0.0f	//Top 4
		};

		GLuint indices[]=
		{
				0, 2, 4,
				1, 0, 4,
				3, 1, 4,
				2, 3, 4,
				2, 3, 1,
				2, 1, 0
		};

	//Generate buffer Ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//Bind VAO
	glBindVertexArray(VAO);

	//Activate VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Activate Element Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Set Attribute pointer to 0 and specify how the gpu will iterate through the Buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	ExitOnGLError("ERROR: Could not enable vertex attributes");

	//Set Attribute pointer to 1 and specify how the gpu will iterate through Buffer
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,	6 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	ExitOnGLError("ERROR: Could not enable vertex attributes");

	glBindVertexArray(0);
}


//Function for getting a GL error
void ExitOnGLError(const char* error_message)
{
  const GLenum ErrorValue = glGetError();

  if (ErrorValue != GL_NO_ERROR)
  {
    fprintf(stderr, "%s: %s\n", error_message, gluErrorString(ErrorValue));
    exit(EXIT_FAILURE);
  }
}









