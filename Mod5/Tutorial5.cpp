/*
 * Tutorial5.cpp
 *
 *  Created on: Jul 27, 2020
 *      Author: Blake
 */


/*
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define WINDOW_TITLE "Tutorial 5"

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

GLint
	windowWidth = 800,
	windowHeight = 600,
	shaderProgram;

GLuint
	VBO,
	VAO,
	EBO;

GLfloat
	cameraSpeed = 0.05f;

GLchar
	currentKey;

GLfloat
	lastMouseX = 400,
	lastMouseY = 300,
	mouseXOffset,
	mouseYOffset,
	yaw = 0.0f,
	pitch = 0.0f,
	sensitivity = 0.5f;	//Used for mouse /camera rotation sensitivity
bool mouseDetected = true;	//Initially true when mouse movement is detected

glm::vec3
	cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f),	//Initial camera position. Placed 5 units in Z
	cameraUpY = glm::vec3(0.0f, 1.0f, 0.0f),	//Temporary y unit vector
	cameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f),	//Temporary z unit vector
	front;

const GLchar * vertexShaderSource = GLSL(330,
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec3 color;

		out vec3 mobileColor;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

	void main(void){
		gl_Position = projection * view * model * vec4(position, 1.0f);
		mobileColor = color;
	}
);

const GLchar * fragmentShaderSource = GLSL(330,
		in vec3 mobileColor;

		out vec4 gpuColor;

	void main(void){
		gpuColor = vec4(mobileColor, 1.0f);
	}

);

void ResizeWindow(int, int);
void RenderGraphics(void);
void CreateShaders(void);
void CreateBuffers(void);
void Keyboard(unsigned char key, int x, int y);
void KeyReleased(unsigned char key, int x, int y);
void MouseClick(int button, int state, int x, int y);
void MouseMove(int x, int y);
void MousePressedMove(int x, int y);

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);	//Sets the initial display mode
	glutInitWindowSize(windowWidth, windowHeight);	//Sets the initial size of the window
	glutCreateWindow(WINDOW_TITLE);	//Creates a window with WINDOW_TITLE as title

	glutReshapeFunc(ResizeWindow);	//Sets the reshape callback for the current window

	glewExperimental = GL_TRUE;	//Must be set to true before calling glewInit()
	if(glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}

	CreateShaders();
	CreateBuffers();

	glUseProgram(shaderProgram); //Installs the shaderProgram object as part of current rendering state

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //specifies the RGBA values used by glClear to clear the color buffers

	glutDisplayFunc(RenderGraphics); //Sets the display callback for the current window
	glutKeyboardFunc(Keyboard);	//Sets the keyboard callback for the current window
	glutKeyboardUpFunc(KeyReleased);	//Sets the keyboard key release callback for the current window
	glutMouseFunc(MouseClick);	//Sets the mouse callback for the current window
	glutPassiveMotionFunc(MouseMove);	//Sets the passive motion callback for the current window
	glutMotionFunc(MousePressedMove);	//Sets the motion callback for the current window

	glutMainLoop();	//Enter the GLUT even processing loop

	glDeleteVertexArrays(1, &VAO); //Destroys vertex array object
	glDeleteBuffers(1, &VBO);	//Destroys vertex buffer object

	return 0;

}

void ResizeWindow(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
	glViewport(0, 0, windowWidth, windowHeight);
}

void RenderGraphics(void)
{
	glEnable(GL_DEPTH_TEST);	//Enables depth comparisons and updating of the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Sets the Color writing buffer and the depth buffer to RGBA values previously set by GLClearColor

	glBindVertexArray(VAO); //Activate the vertex array object before rendering and transforming them

	if(currentKey == 'w')
		cameraPosition += cameraSpeed * cameraForwardZ;

	if(currentKey == 's')
		cameraPosition -= cameraSpeed * cameraForwardZ;

	if(currentKey == 'a')
		cameraPosition -= glm::normalize(glm::cross(cameraForwardZ, cameraUpY)) * cameraSpeed;

	if(currentKey == 'd')
		cameraPosition += glm::normalize(glm::cross(cameraForwardZ, cameraUpY)) * cameraSpeed;

	cameraForwardZ = front;	//Replaces camera forward vector with Radians normalized as a unit vector

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); //Plcae the object at the center of the viewport
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f)); //Rotate the object 45 degrees on the y axis
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); //Double the objects xyz scale

	glm::mat4 view;
	view = glm::lookAt(cameraPosition - cameraForwardZ, cameraPosition, cameraUpY);

	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);

	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();	//Marks the current window as needing to be redisplayed

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	glutSwapBuffers();	//swaps the buffers of the current window if double buffered
}

void CreateShaders()
{
	shaderProgram = glCreateProgram();

	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);		//Replaces the source code in vertexShader with the code at vertexShaderSource
	glCompileShader(vertexShader);

	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//Delete the fragment and vertex shaders once attached and linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void CreateBuffers()
{
	GLfloat vertices [] = {
			//Position			 //Color
			 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,	// 0
			-0.5f,  0.5f,  0.5f, 0.5f, 1.0f, 0.0f,	// 1
			-0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.5f,	// 2
			-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,	// 3
			-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 0.0f,	// 4
			 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,	// 5
			 0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 0.5f,	// 6
			 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,	// 7
	};

	GLuint indices [] = {
			3, 5, 6,	// 0.1
			6, 4, 3,	// 0.2
			2, 7, 0,	// 1.1
			0, 1, 2,	// 1.2
			1, 4, 3,	// 2.1
			3, 2, 1,	// 2.2
			0, 6, 5,	// 3.1
			5, 7, 0,	// 3.2
			3, 5, 7,	// 4.1
			7, 2, 3,	// 4.2
			4, 6, 0,	// 5.1
			0, 1, 4,	// 5.2


	};

	//Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//Activate the vertex array object before binding and setting any VBOs and vertex attribute pointers
	glBindVertexArray(VAO);

	//Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Copy vertices to VBO

	//Acitvate the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Set attribute pointer 0 to hold positions data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);	//Enables vertex attribute

	//Set attribute pointer 1 to hold color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);	//Enables vertex attribute


	glBindVertexArray(0);	//Deactivates the VAO which is good practice
}

void Keyboard(unsigned char key, GLint x, GLint y)
{
	switch(key){

		case 'w':
			currentKey = key;
			cout << "You pressed the W!" << endl;
			break;

		case 's':
			currentKey = key;
			cout << "You pressed the S!" << endl;
			break;

		case 'a':
			currentKey = key;
			cout<<"You pressed the A!" << endl;
			break;

		case 'd':
			currentKey = key;
			cout<<"You pressed the D!" << endl;
			break;

		default:
			currentKey = '0';
			cout<<"Press a key!" << endl;
	}
}

void KeyReleased(unsigned char key, GLint x, GLint y)
{
	currentKey = '0';
	cout << "Key Released" << endl;
}

void MouseClick(int button, int state, int x, int y)
{
	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)){
		cout << "Left Mouse Button Clicked!" << endl;
	}
	if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)){
		cout << "Right Mouse Button Clicked!" << endl;
	}
}

void MouseMove(int x, int y)
{
	if(mouseDetected)
	{
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}

	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y;

	lastMouseX = x;
	lastMouseY = y;

	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	yaw += mouseXOffset;
	pitch += mouseYOffset;

	if (pitch > 89.0f)
		pitch = 89.0f;

	if(pitch < -89.0f)
		pitch = -89.0f;

	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
}

void MousePressedMove(int x, int y)
{

}*/













