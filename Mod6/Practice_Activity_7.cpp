/*
 * Practice_Activity_7.cpp
 *
 *  Created on: Aug 8, 2020
 *      Author: Blake
 */

/*
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL2/SOIL2.h"

using namespace std;

#define WINDOW_TITLE "Practice Activity 7"

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

GLint
	shaderProgram,
	windowHeight = 600,
	windowWidth = 800;

GLuint
	VBO,
	VAO,
	texture;

GLfloat
	lastMouseX,
	lastMouseY,
	mouseXOffset,
	mouseYOffset,
	yaw,
	pitch,
	rotateSensitivity = 0.005f,
	zoomSensitivity = 0.05f;

int
	indicesLength,
	altKeyDown,
	buttonState,
	buttonDown;

glm::vec3
	cameraPosition = glm::vec3(0.0f, 1.0f, 4.0f),
	cameraUpY = glm::vec3(0.0f, 1.0f, 0.0f),
	forWardZ = glm::vec3(0.0f, 0.0f, -1.0f),
	front = glm::vec3(0.0f, 0.0f, -1.0f);

void ResizeWindow(int, int);
void Render(void);
void CreateShaders(void);
void CreateBuffers(void);
void ExitOnGLError(const char*);
void GenerateTexture(void);
void MouseClick(int button, int state, int x, int y);
void MousePressedMove(int x, int y);

const GLchar * vertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position;
	layout (location = 1) in vec2 textureCoordinate;


	out vec2 mobileTextureCoordinate;	//RGB values exported to fragment shader source code

	uniform mat4 projection;
	uniform mat4 model;
	uniform mat4 view;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f);	//transform vertices to clip coordinates
		mobileTextureCoordinate = vec2(textureCoordinate.x, 1.0f - textureCoordinate.y);
	}
);

const GLchar * fragmentShaderSource = GLSL(330,

		in vec2 mobileTextureCoordinate;

		out vec4 gpuTexture;

		uniform sampler2D uTexture;

	void main()
		{
		gpuTexture = texture(uTexture, mobileTextureCoordinate);	//Sends RGBA values to GPU
		}
);

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK){
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	CreateShaders();
	ExitOnGLError("ERROR: Could not Create Shaders");
	CreateBuffers();
	ExitOnGLError("ERROR: Could not Create Bufers");
	GenerateTexture();
	ExitOnGLError("ERROR: Could not Generate Textures");

	glUseProgram(shaderProgram);
	ExitOnGLError("ERROR: Could not use the program");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glutReshapeFunc(ResizeWindow);
	glutDisplayFunc(Render);
	glutMotionFunc(MousePressedMove);	//Sets the motion callback for the current window
	glutMouseFunc(MouseClick);			//Sets the mouse callback for the current window

	glutMainLoop();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;
}

void ResizeWindow(int x, int y)
{
	windowWidth = x;
	windowHeight = y;
	glViewport(0, 0, windowWidth, windowHeight);
}



void Render ()
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	glm::mat4 view;
	view = glm::lookAt(cameraPosition - front, cameraPosition + front, cameraUpY);

	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);

	//Reference matrix unifroms from the cube shader program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	//Passes matrix values into the source code
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_TRIANGLES, 0, 18);

	glBindVertexArray(0);

	glutSwapBuffers();

}

void CreateShaders()
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

void CreateBuffers()
{
	GLfloat vertices[] =
		{
				-0.5f, 0.0f,  0.5f,			0.0f, 0.0f,//Back Left 0
				-0.5f, 0.0f, -0.5f,			1.0f, 0.0f,//Front Left 2
				0.0f, 1.0f,  0.0f,			0.5f, 0.75f,//Top 4

				 0.5f, 0.0f,  0.5f, 		0.0f, 0.0f,//Back Right 1
				 -0.5f, 0.0f,  0.5f,		1.0f, 0.0f,//Back Left 0
				 0.0f, 1.0f,  0.0f,			0.5f, 0.75f,//Top 4

				 0.5,  0.0f, -0.5f,			0.0f, 0.0f,//Front Right 3
				 0.5f, 0.0f,  0.5f, 		1.0f, 0.0f,//Back Right 1
				 0.0f, 1.0f,  0.0f,			0.5f, 0.75f,//Top 4

				 -0.5f, 0.0f, -0.5f,		0.0f, 0.0f,//Front Left 2
				 0.5,  0.0f, -0.5f,			1.0f, 0.0f,//Front Right 3
				 0.0f, 1.0f,  0.0f,			0.5f, 0.75f,//Top 4

				 -0.5f, 0.0f, -0.5f,		0.0f, 0.0f,//Front Left 2
				 0.5,  0.0f, -0.5f,			1.0f, 0.0f,//Front Right 3
				 0.5f, 0.0f,  0.5f, 		1.0f, 1.0f,//Back Right 1

				 -0.5f, 0.0f, -0.5f,		0.0f, 0.0f,//Front Left 2
				 0.5f, 0.0f,  0.5f, 		1.0f, 1.0f,//Back Right 1
				 -0.5f, 0.0f,  0.5f,		0.0f, 1.0f//Back Left 0
		};


	//Generate buffer Ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//Bind VAO
	glBindVertexArray(VAO);

	//Activate VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Set Attribute pointer to 0 and specify how the gpu will iterate through the Buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	ExitOnGLError("ERROR: Could not enable vertex attributes");

	//Set Attribute pointer to 1 and specify how the gpu will iterate through Buffer
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,	5 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	ExitOnGLError("ERROR: Could not enable vertex attributes");

	glBindVertexArray(0);
}

void GenerateTexture()
{

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	ExitOnGLError("ERROR: Could not generate texture");

	int width, height;

	unsigned char* image = SOIL_load_image("brick015.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
	ExitOnGLError("ERROR: Could not load the image");

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MouseClick(int button, int state, int x, int y)
{
	lastMouseX = x;
	lastMouseY = y;
	buttonDown = button;
	buttonState = state;
}

void MousePressedMove(int x, int y)
{
	altKeyDown = glutGetModifiers();
	if(altKeyDown == GLUT_ACTIVE_ALT)
	{
		//Gets the direction the mouse was moved in x and y
		mouseXOffset = x - lastMouseX;
		mouseYOffset = lastMouseY - y;

		lastMouseX = x;
		lastMouseY = y;

		//Updates with new mouse coordinates
		mouseXOffset *= rotateSensitivity;
		mouseYOffset *= rotateSensitivity;

		//Accumulates the yaw and pitch variables
		yaw += mouseXOffset;
		pitch += mouseYOffset;

		//Handles left button rotation
		if((buttonDown == GLUT_LEFT_BUTTON) && (buttonState == GLUT_DOWN)){
			{
				front.x = 10.0f * cos(yaw);
				front.y = 10.0f * sin(pitch);
				front.z = sin(yaw) * cos(pitch) * 10.0f;
			}
		}
		//Handles Right button zoom
		if((buttonDown == GLUT_RIGHT_BUTTON) && (buttonState == GLUT_DOWN)){
			if(mouseYOffset < 0){
				cameraPosition += zoomSensitivity * front;
			}
			if(mouseYOffset > 0){
				cameraPosition -= zoomSensitivity * front;
			}
		}

	}
}

void ExitOnGLError(const char* error_message)
{
  const GLenum ErrorValue = glGetError();

  if (ErrorValue != GL_NO_ERROR)
  {
    fprintf(stderr, "%s: %s\n", error_message, gluErrorString(ErrorValue));
    exit(EXIT_FAILURE);
  }
} */














