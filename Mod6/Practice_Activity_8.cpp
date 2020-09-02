/*
 * Practice_Activity_8.cpp
 *
 *  Created on: Aug 9, 2020
 *      Author: Blake
 */

#include <time.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL2/SOIL2.h"

using namespace std;

#define WINDOW_TITLE "Practice Activity 8"

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

//Camera Vectors
glm::vec3
	cameraPosition = glm::vec3(0.0f, 1.0f, 4.0f),
	cameraUpY = glm::vec3(0.0f, 1.0f, 0.0f),
	forWardZ = glm::vec3(0.0f, 0.0f, -1.0f),
	front = glm::vec3(0.0f, 0.0f, -1.0f);


//Light color
glm::vec3 lightColor(0.25f, 0.6f, 0.25f);

//Light position
glm::vec3 fillLightPosition(5.0f, 0.0f,  3.0f);
glm::vec3 keyLightPosition(-1.0f, 1.0f, -4.0f);

float rotation = 0;
float radians = 0;
clock_t lastTime = 0;
bool flip = false;

void ResizeWindow(int, int);
void Render(void);
void CreateShaders(void);
void CreateBuffers(void);
void ExitOnGLError(const char*);
void GenerateTexture(void);

const GLchar * pyrVertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position;
	layout (location = 1) in vec3 normals;
	layout (location = 2) in vec2 textureCoordinate;

	out vec3 mobileNormals;
	out vec2 mobileTextureCoordinate;	//RGB values exported to fragment shader source code
	out vec3 FragmentPos;	//For outgoing pixels to fragment shader

	uniform mat4 projection;
	uniform mat4 model;
	uniform mat4 view;

	void main()
	{
		//Transforms vertices into clip coordinates
		gl_Position = projection * view * model * vec4(position, 1.0f);

		//Assigns texture coordinates to outgoing variable
		mobileTextureCoordinate = vec2(textureCoordinate.x, 1.0f - textureCoordinate.y);

		//Gets fragment / pixel position in world space only (exclude view and projection)
		FragmentPos = vec3(model * vec4(position, 1.0f));

		//Get normal vectors in world space only and exclude normal translation properties
		mobileNormals = mat3(transpose(inverse(model))) * normals;
	}
);

const GLchar * pyrFragmentShaderSource = GLSL(330,

	in vec2 mobileTextureCoordinate;
	in vec3 mobileNormals;
	in vec3 FragmentPos;

	out vec4 FragColor;

	uniform sampler2D uTexture;

	uniform vec3 lightColor;
	uniform vec3 keyLightPos;
	uniform vec3 fillLightPos;
	uniform vec3 viewPosition;

	void main()
	{
		//Key light adjustments
		float keySpecularIntensity = 3.0f;
		float keyHighlightSize = 4.0f;
		float keyAmbientStrength = 0.0f;

		//Fill light adjustments
		float fillSpecularIntensity = 0.2f;
		float fillHighlightSize = 0.5f;
		float fillAmbientStrength = 0.0f;

		//Phong lighting model calculations to generate ambient, diffuse, and specular components

		vec3 norm = normalize(mobileNormals);
		vec3 viewDir = normalize(viewPosition - FragmentPos);	//Calculate view direction

		//Key Light
		//Calculate key ambient lighting
		vec3 keyAmbient = keyAmbientStrength * lightColor;

		//Calculate key diffuse lighting
		vec3 keyLightDirection = normalize(keyLightPos - FragmentPos);	//Calculate distance (light direction) between light source and fragments/pixels on object
		float keyImpact = max(dot(norm, keyLightDirection), 0.0);	//Calculate diffuse impact by generating dot product of normal and light
		vec3 keyDiffuse = keyImpact * lightColor; 	//Generate diffuse light color

		//Calculate key specular lighting
		vec3 keyReflectDir = reflect(-keyLightDirection, norm);	//Calculate reflection vector
		float keySpecularComponent = pow(max(dot(viewDir, keyReflectDir), 0.0), keyHighlightSize);
		vec3 keySpecular = keySpecularIntensity * keySpecularComponent * lightColor;

		vec3 keyResult = keyAmbient + keyDiffuse + keySpecular;

		//Fill Light
		//Calculate fill ambient lighting
		vec3 fillAmbient = fillAmbientStrength * lightColor;

		//Calculate fill diffuse lighting
		vec3 fillLightDirection = normalize(fillLightPos - FragmentPos);
		float fillImpact = max(dot(norm, fillLightDirection), 0.0);
		vec3 fillDiffuse = fillImpact * lightColor;

		//Calculate fill specular lighting
		vec3 fillReflectDir = reflect(-fillLightDirection, norm);
		float fillSpecularComponent = pow(max(dot(viewDir, fillReflectDir), 0.0), fillHighlightSize);
		vec3 fillSpecular = fillSpecularIntensity * fillSpecularComponent * lightColor;

		vec3 fillResult = fillAmbient + fillDiffuse + fillSpecular;


		vec3 objectTexture = texture(uTexture, mobileTextureCoordinate).xyz;
		vec3 result =  objectTexture * (keyResult + fillResult);

		//Sends light result to gpu
		FragColor = vec4(result, 1.0f);
//		FragColor = texture(uTexture, mobileTextureCoordinate);

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


	//get radians for rotation about y-axis
	clock_t now = clock();

	if(lastTime == 0)
		lastTime = now;

	//Reverse rotation after 360°
	if(radians >= 6.28319)
		flip = true;
	if(radians <= -6.28319)
		flip = false;

	if(!flip)
	{
		rotation += 90.0f * ((float)(now - lastTime) / CLOCKS_PER_SEC);
		radians = glm::radians(rotation);
		lastTime = now;
	}

	if(flip)
	{
		rotation -= 90.0f * ((float)(now - lastTime) / CLOCKS_PER_SEC);
		radians = glm::radians(rotation);
		lastTime = now;
	}

	glm::mat4 model;
	model = glm::translate(model, glm::vec3( 0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, radians, glm::vec3(1.0f, 1.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	//Calculate position change in view space
	float viewPos = radians/4;

	glm::mat4 view;
	view = glm::lookAt(cameraPosition - front, cameraPosition + front, cameraUpY);
	view = glm::translate(view, glm::vec3(viewPos, -viewPos, 0.0f));

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

//	GLint textureLoc = glGetUniformLocation(shaderProgram, "uTexture");
	GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	GLint keyLightPosLoc = glGetUniformLocation(shaderProgram, "keyLightPos");
	GLint fillLightPosLoc = glGetUniformLocation(shaderProgram, "fillLightPos");
	GLint viewPositionLoc = glGetUniformLocation(shaderProgram, "viewPosition");
	ExitOnGLError("ERROR: Could not get uniform locations");

//	glUniform1i(textureLoc, texture);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(keyLightPosLoc, keyLightPosition.x, keyLightPosition.y, keyLightPosition.z);
	glUniform3f(fillLightPosLoc, fillLightPosition.x, fillLightPosition.y, fillLightPosition.z);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	ExitOnGLError("ERROR: Could not pass uniform data");

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
	glShaderSource(vertexShader, 1, &pyrVertexShaderSource, NULL);
	glCompileShader(vertexShader);
	ExitOnGLError("ERROR: Could not compile Vertex Shader");

	//Create and compile fragment shader from source
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &pyrFragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	ExitOnGLError("ERROR: Could not compile Fragment Shader");

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
				//Positions					//Normals				//Texture Coordinates
				-0.5f, 0.0f,  0.5f,			-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		//Back Left 0
				-0.5f, 0.0f, -0.5f,			-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		//Front Left 2
				 0.0f, 1.0f,  0.0f,			-1.0f, 0.0f, 0.0f,		0.5f, 0.75f,	//Top 4

				  0.5f, 0.0f,  0.5f, 		 0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		//Back Right 1
				 -0.5f, 0.0f,  0.5f,		 0.0f, 0.0f, 1.0f,		1.0f, 0.0f,		//Back Left 0
				  0.0f, 1.0f,  0.0f,		 0.0f, 0.0f, 1.0f,		0.5f, 0.75f,	//Top 4

				  0.5,  0.0f, -0.5f,		 1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		//Front Right 3
				  0.5f, 0.0f,  0.5f, 		 1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		//Back Right 1
				  0.0f, 1.0f,  0.0f,		 1.0f, 0.0f, 0.0f,		0.5f, 0.75f,	//Top 4

				 -0.5f, 0.0f, -0.5f,		 0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		//Front Left 2
				  0.5,  0.0f, -0.5f,		 0.0f, 0.0f, -1.0f,		1.0f, 0.0f,		//Front Right 3
				  0.0f, 1.0f,  0.0f,		 0.0f, 0.0f, -1.0f,		0.5f, 0.75f,	//Top 4

				 -0.5f, 0.0f, -0.5f,		 0.0f, -1.0f, 0.0f,		0.0f, 0.0f,		//Front Left 2
				  0.5,  0.0f, -0.5f,		 0.0f, -1.0f, 0.0f,		1.0f, 0.0f,		//Front Right 3
				  0.5f, 0.0f,  0.5f, 		 0.0f, -1.0f, 0.0f,		1.0f, 1.0f,		//Back Right 1

				 -0.5f, 0.0f, -0.5f,		 0.0f, -1.0f, 0.0f,		0.0f, 0.0f,		//Front Left 2
				  0.5f, 0.0f,  0.5f, 		 0.0f, -1.0f, 0.0f,		1.0f, 1.0f,		//Back Right 1
				 -0.5f, 0.0f,  0.5f,		 0.0f, -1.0f, 0.0f,		0.0f, 1.0f		//Back Left 0
		};


	//Generate buffer Ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//Bind VAO
	glBindVertexArray(VAO);

	//Activate VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Vertex Coordinates Attribute Pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	ExitOnGLError("ERROR: Could not enable vertex attributes");

	//Normal Attribute Pointer
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	ExitOnGLError("ERROR: Could not enable normal attribute poitner");

	//Texture Coordinate Attribute Pointer
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,	8 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);
	ExitOnGLError("ERROR: Could not enable texture coordinates");

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


void ExitOnGLError(const char* error_message)
{
  const GLenum ErrorValue = glGetError();

  if (ErrorValue != GL_NO_ERROR)
  {
    fprintf(stderr, "%s: %s\n", error_message, gluErrorString(ErrorValue));
    exit(EXIT_FAILURE);
  }
}


