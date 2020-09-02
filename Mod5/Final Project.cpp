/*
 * Milestone_2.cpp
 *
 *  Created on: Jul 28, 2020
 *      Author: Blake
 */


#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

using namespace std;

#define WINDOW_TITLE "Milestone 2"

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

GLint
	windowWidth = 1600,
	windowHeight = 1200,
	shaderProgram,
	modelLoc,
	viewLoc,
	projLoc;

GLuint
	VBO,
	VAO,
	NBO,
	EBO;

glm::vec3
	cameraPosition = glm::vec3(0.0f, 0.0f, 10.0f),
	cameraUpY = glm::vec3(0.0f, 1.0f, 0.0f),
	forWardZ = glm::vec3(0.0f, 0.0f, -1.0f),
	front = glm::vec3(0.0f, 0.0f, -1.0f);

int
	indicesLength,
	altKeyDown,
	buttonState,
	buttonDown;

GLfloat
	lastMouseX,
	lastMouseY,
	mouseXOffset,
	mouseYOffset,
	yaw,
	pitch,
	rotateSensitivity = 0.005f,
	zoomSensitivity = 0.05f;

//Light color
glm::vec3 lightColor(0.25f, 0.6f, 0.25f);

//Light position
glm::vec3 fillLightPosition(5.0f, 0.0f,  3.0f);
glm::vec3 keyLightPosition(-1.0f, 1.0f, -4.0f);


void Initialize(int argc, char* argv[]);
void ResizeWindow(int, int);
void RenderGraphics(void);
void CreateShaders(void);
void CreateBuffers(void);
void IdleFunction(void);
void DestroyGraphics(void);
void MouseClick(int button, int state, int x, int y);
void MousePressedMove(int x, int y);
void ExitOnGLError(const char* error_message);

const GLchar * vertexShaderSource = GLSL(330,
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec3 color;
		layout (location = 2) in vec3 normals;
//		layout (location = 3) in vec2 textureCoordinates;

		out vec3 mobileColor;
		out vec3 mobileNormals;
//		out vec2 mobileTextureCoordinates
		out vec3 FragmentPos;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

	void main(void){
		gl_Position = projection * view * model * vec4(position, 1.0f);

		mobileColor = color;

		mobileNormals = mat3(transpose(inverse(model))) * normals;

		FragmentPos = vec3(model * vec4(position, 1.0f));
	}
);

const GLchar * fragmentShaderSource = GLSL(330,
		in vec3 mobileColor;
		in vec3 mobileNormals;
//		in vec2 mobileTextureCoordinates;
		in vec3 FragmentPos;

		out vec4 FragColor;

		uniform sampler2D uTexture;

		uniform vec3 lightColor;
		uniform vec3 keyLightPos;
		uniform vec3 fillLightPos;
		uniform vec3 viewPosition;

	void main(void){
		//Key light adjustments
				float keySpecularIntensity = 3.0f;
				float keyHighlightSize = 4.0f;
				float keyAmbientStrength = 0.0f;

				//Fill light adjustments
				float fillSpecularIntensity = 0.2f;
				float fillHighlightSize = 0.5f;
				float fillAmbientStrength = 0.0f;

				//Phong lighting model calculations to generate ambient, diffuse, and specular components

				vec3 viewDir = normalize(viewPosition - FragmentPos);	//Calculate view direction

				//Key Light
				//Calculate key ambient lighting
				vec3 keyAmbient = keyAmbientStrength * lightColor;

				//Calculate key diffuse lighting
				vec3 keyLightDirection = normalize(keyLightPos - FragmentPos);	//Calculate distance (light direction) between light source and fragments/pixels on object
				float keyImpact = max(dot(mobileNormals, keyLightDirection), 0.0);	//Calculate diffuse impact by generating dot product of normal and light
				vec3 keyDiffuse = keyImpact * lightColor; 	//Generate diffuse light color

				//Calculate key specular lighting
				vec3 keyReflectDir = reflect(-keyLightDirection, mobileNormals);	//Calculate reflection vector
				float keySpecularComponent = pow(max(dot(viewDir, keyReflectDir), 0.0), keyHighlightSize);
				vec3 keySpecular = keySpecularIntensity * keySpecularComponent * lightColor;

				vec3 keyResult = keyAmbient + keyDiffuse + keySpecular;

				//Fill Light
				//Calculate fill ambient lighting
				vec3 fillAmbient = fillAmbientStrength * lightColor;

				//Calculate fill diffuse lighting
				vec3 fillLightDirection = normalize(fillLightPos - FragmentPos);
				float fillImpact = max(dot(mobileNormals, fillLightDirection), 0.0);
				vec3 fillDiffuse = fillImpact * lightColor;

				//Calculate fill specular lighting
				vec3 fillReflectDir = reflect(-fillLightDirection, mobileNormals);
				float fillSpecularComponent = pow(max(dot(viewDir, fillReflectDir), 0.0), fillHighlightSize);
				vec3 fillSpecular = fillSpecularIntensity * fillSpecularComponent * lightColor;

				vec3 fillResult = fillAmbient + fillDiffuse + fillSpecular;


				vec3 objectTexture = texture(uTexture, mobileTextureCoordinate).xyz;
				vec3 result =  objectTexture * (keyResult + fillResult);

				//Sends light result to gpu
				FragColor = vec4(result, 1.0f) * mobileColor;
	}

);

int main(int argc, char* argv[])
{
	Initialize(argc, argv);
	glutMainLoop();			//Enter the GLUT even processing loop
	exit(EXIT_SUCCESS);
	return 0;
}

void Initialize(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(windowWidth, windowHeight);

	int windowHandle = glutCreateWindow(WINDOW_TITLE);

	if(windowHandle < 1){
		fprintf(stderr, "ERROR: Could not create a new window.\n");
		exit(EXIT_FAILURE);
	}

	glewExperimental = GL_TRUE;
	GLenum glewInitResult = glewInit();
	if(glewInitResult != GLEW_OK)
	{
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(glewInitResult));
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	CreateShaders();
	CreateBuffers();

	glUseProgram(shaderProgram);
	ExitOnGLError("ERROR: Could not use shader program");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glutReshapeFunc(ResizeWindow);
	glutDisplayFunc(RenderGraphics);	//Sets the display callback for the current window
	glutIdleFunc(IdleFunction);
	glutCloseFunc(DestroyGraphics);
	glutMotionFunc(MousePressedMove);	//Sets the motion callback for the current window
	glutMouseFunc(MouseClick);			//Sets the mouse callback for the current window
}

void ResizeWindow(int x, int y)
{
	windowWidth = x;
	windowHeight = y;
	glViewport(0, 0, windowWidth, windowHeight);
}

void CreateShaders()
{
	shaderProgram = glCreateProgram();

	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);	//Replaces the source code in vertexShader with the code at vertexShaderSource
	glCompileShader(vertexShader);
	ExitOnGLError("ERROR: Could not link and compile vertex shader");

	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	ExitOnGLError("ERROR: Could not link and compile fragment shader");

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

			-5.0f, -13.0f, -4.0f, 	0.706f, 0.788f, 0.780f, //0
			-9.0f, -3.0f,  -7.5f, 	0.706f, 0.788f, 0.780f,	//1
			-9.5f, -5.0f,  -13.0f,	0.706f, 0.788f, 0.780f,	//2
			-7.0f,  0.0f,  -13.0f,	0.706f, 0.788f, 0.780f,	//3
			-8.5f,  0.0f,  -10.0f,	0.020f, 0.051f, 0.110f,	//4
			-1.5f,  3.5f,  -13.0f,	0.020f, 0.051f, 0.110f,	//5
			-9.0f,  3.0f,  -8.0f,	0.020f, 0.051f, 0.110f, //6
			-9.0f,  3.5f,  -5.0f,	0.020f, 0.051f, 0.110f,	//7
			-10.0f, 9.0f,  -9.0f,	0.020f, 0.051f, 0.110f,	//8
			-10.5f, 7.0f,  -6.0f,	0.741f, 0.569f, 0.380f,	//9
			-8.0f,  13.0f, -8.0f,	0.020f, 0.051f, 0.110f,	//10
			-11.0f, 13.0f, -6.0f,	0.020f, 0.051f, 0.110f,	//11
			-8.5f,  19.5f, -6.5f, 	0.020f, 0.051f, 0.110f,	//12
			-3.5f,  13.0f, -6.0f,	0.020f, 0.051f, 0.110f,	//13
			-2.0f,  10.0f, -5.0f,	0.020f, 0.051f, 0.110f, //14
			-11.0f, 13.0f, -5.75f,	0.741f, 0.569f, 0.380f,	//15
			-8.5f,  19.5f, -6.25f, 	0.741f, 0.569f, 0.380f,	//16
			-3.5f,  13.0f, -5.75f,	0.741f, 0.569f, 0.380f,	//17
			-2.0f,  10.0f, -4.75f,	0.020f, 0.051f, 0.110f,	//18
			-6.0f,  10.0f, -7.0f,	0.020f, 0.051f, 0.110f,	//19
			-8.0f,  9.0f,  -7.5f,	0.020f, 0.051f, 0.110f,	//20
			-7.0f,  11.0f, -7.0f,	0.169f, 0.114f, 0.114f,	//21
			-10.5f, 8.0f,  -5.5f,	0.020f, 0.051f, 0.110f,	//22
			 0.0f,  10.0f, -5.5f,	0.020f, 0.051f, 0.110f,	//23
			-2.0f,  10.0f, -9.0f,	0.020f, 0.051f, 0.110f,	//24
			-6.0f,  10.0f, -5.5f,	0.020f, 0.051f, 0.110f,	//25
			 5.0f, -13.0f, -4.0f, 	0.706f, 0.788f, 0.780f, //26 - 0
			 9.0f, -3.0f,  -7.5f, 	0.706f, 0.788f, 0.780f,	//27 - 1
			 9.5f, -5.0f,  -13.0f,	0.706f, 0.788f, 0.780f,	//28 - 2
			 7.0f,  0.0f,  -13.0f,	0.706f, 0.788f, 0.780f,	//29 - 3
			 8.5f,  0.0f,  -10.0f,	0.020f, 0.051f, 0.110f,	//30 - 4
			 1.5f,  3.5f,  -13.0f,	0.020f, 0.051f, 0.110f,	//31 - 5
			 9.0f,  3.0f,  -8.0f,	0.020f, 0.051f, 0.110f, //32 - 6
			 9.0f,  3.5f,  -5.0f,	0.020f, 0.051f, 0.110f,	//33 - 7
			 10.0f, 9.0f,  -9.0f,	0.020f, 0.051f, 0.110f,	//34 - 8
			 10.5f, 7.0f,  -6.0f,	0.741f, 0.569f, 0.380f,	//35 - 9
			 8.0f,  13.0f, -8.0f,	0.020f, 0.051f, 0.110f,	//36 - 10
			 11.0f, 13.0f, -6.0f,	0.020f, 0.051f, 0.110f,	//37 - 11
			 8.5f,  19.5f, -6.5f, 	0.020f, 0.051f, 0.110f,	//38 - 12
			 3.5f,  13.0f, -6.0f,	0.020f, 0.051f, 0.110f,	//39 - 13
			 2.0f,  10.0f, -5.0f,	0.020f, 0.051f, 0.110f, //40 - 14
			 11.0f, 13.0f, -5.75f,	0.741f, 0.569f, 0.380f,	//41 - 15
			 8.5f,  19.5f, -6.25f, 	0.741f, 0.569f, 0.380f,	//42 - 16
			 3.5f,  13.0f, -5.75f,	0.741f, 0.569f, 0.380f,	//43 - 17
			 2.0f,  10.0f, -4.75f,	0.020f, 0.051f, 0.110f,	//44 - 18
			 6.0f,  10.0f, -7.0f,	0.020f, 0.051f, 0.110f,	//45 - 19
			 8.0f,  9.0f,  -7.5f,	0.020f, 0.051f, 0.110f,	//46 - 20
			 7.0f,  11.0f, -7.0f,	0.169f, 0.114f, 0.114f,	//47 - 21
			 10.5f, 8.0f,  -5.5f,	0.020f, 0.051f, 0.110f,	//48 - 22
			 0.0f,  10.0f, -5.5f,	0.020f, 0.051f, 0.110f,	//49 - 23
			 2.0f,  10.0f, -9.0f,	0.020f, 0.051f, 0.110f,	//50 - 24
			 6.0f,  10.0f, -5.5f,	0.020f, 0.051f, 0.110f,	//51 - 25
			-8.0f,  7.75f, -0.0f,	0.196f, 0.294f, 0.384f,	//52
			-2.0f,  9.0f,  -3.0f,	0.196f, 0.294f, 0.384f,	//53
			 0.0f,  10.0f, -4.0f, 	0.706f, 0.788f, 0.780f,	//54
			 0.0f,  8.0f,   0.0f,	0.706f, 0.788f, 0.780f, //55
			-0.5f,  8.0f,   0.0f,	0.020f, 0.051f, 0.110f, //56
			-2.0f,  6.0f,   4.0f,	0.741f, 0.569f, 0.380f, //57
			-2.0f,  3.5f,   5.0f,	0.196f, 0.294f, 0.384f,	//58
			-5.5f,  4.0f,   3.5f,	0.020f, 0.051f, 0.110f,	//59
			-6.0f,  2.5f,   3.0f,	0.020f, 0.051f, 0.110f,	//60
			-8.0f,  3.0f,  -1.0f,	0.020f, 0.051f, 0.110f,	//61
			-8.5f, -2.5f,  -3.0f, 	0.243f, 0.333f, 0.384f,	//62
			-5.5f, -9.0f,  -4.0f, 	0.243f, 0.333f, 0.384f,	//63
			-7.0f, -5.75f, -2.0f,	0.741f, 0.569f, 0.380f,	//64
			-4.0f, -6.75f,  1.0f, 	0.741f, 0.569f, 0.380f, //65
			-7.0f,  1.5f,   1.0f,	0.741f, 0.569f, 0.380f,	//66
			-5.5f, -3.9f,   1.5f,	0.741f, 0.569f, 0.380f, //67
			-4.07f,-2.5f,   4.5f,	0.020f, 0.051f, 0.110f,	//68
			-6.85f, 1.75f,  1.25f,	0.020f, 0.051f, 0.110f,	//69
			-3.0f,  0.5f,   6.0f, 	0.243f, 0.333f, 0.384f,	//70
			-5.45f,-3.85f,  1.75f,	0.020f, 0.051f, 0.110f, //71
			-4.1f, -2.53f,  4.4f,	0.020f, 0.051f, 0.110f,	//72
			-2.97f, 0.5f,   6.03f, 	0.020f, 0.051f, 0.110f,	//73
			-2.0f,  1.47f,  6.03f, 	0.020f, 0.051f, 0.110f,	//74
			-2.0f,  1.5f,   6.0f, 	0.020f, 0.051f, 0.110f,	//75
			-2.0f,  3.47f,  5.2f,	0.020f, 0.051f, 0.110f,	//76
			 0.0f,  1.47f,  6.2f,	0.243f, 0.333f, 0.384f, //77
			 0.0f,  4.0f,   5.2f,	0.243f, 0.333f, 0.384f, //78
			-1.5f,  6.0f,   4.2f,	0.243f, 0.333f, 0.384f, //79
			 8.0f,  7.75f, -0.0f,	0.196f, 0.294f, 0.384f,	//80 - 52
			 2.0f,  9.0f,  -3.0f,	0.243f, 0.333f, 0.384f,	//81 - 53
			 0.0f,  10.0f, -4.0f, 	0.706f, 0.788f, 0.780f,	//82 - 54
			 0.0f,  8.0f,   0.0f,	0.706f, 0.788f, 0.780f, //83 - 55
			 0.5f,  8.0f,   0.0f,	0.020f, 0.051f, 0.110f, //84 - 56
			 2.0f,  6.0f,   4.0f,	0.741f, 0.569f, 0.380f, //85 - 57
			 2.0f,  3.5f,   5.0f,	0.196f, 0.294f, 0.384f,	//86 - 58
			 5.5f,  4.0f,   3.5f,	0.020f, 0.051f, 0.110f,	//87 - 59
			 6.0f,  2.5f,   3.0f,	0.020f, 0.051f, 0.110f,	//88 - 60
			 8.0f,  3.0f,  -1.0f,	0.020f, 0.051f, 0.110f,	//89 - 61
			 8.5f, -2.5f,  -3.0f, 	0.243f, 0.333f, 0.384f,	//90 - 62
			 5.5f, -9.0f,  -4.0f, 	0.243f, 0.333f, 0.384f,	//91 - 63
			 7.0f, -5.75f, -2.0f,	0.741f, 0.569f, 0.380f,	//92 - 64
			 4.0f, -6.75f,  1.0f, 	0.741f, 0.569f, 0.380f, //93 - 65
			 7.0f,  1.5f,   1.0f,	0.741f, 0.569f, 0.380f,	//94 - 66
			 5.5f, -3.9f,   1.5f,	0.741f, 0.569f, 0.380f, //95 - 67
			 4.07f,-2.5f,   4.5f,	0.741f, 0.569f, 0.380f,	//96 - 68
			 6.85f, 1.75f,  1.25f,	0.020f, 0.051f, 0.110f,	//97 - 69
			 3.0f,  0.5f,   6.0f, 	0.243f, 0.333f, 0.384f,	//98 - 70
			 5.45f,-3.85f,  1.75f,	0.020f, 0.051f, 0.110f, //99 - 71
			 4.1f, -2.53f,  4.4f,	0.020f, 0.051f, 0.110f,	//100 - 72
			 2.97f, 0.5f,   6.03f, 	0.020f, 0.051f, 0.110f,	//101 - 73
			 2.0f,  1.47f,  6.03f, 	0.020f, 0.051f, 0.110f,	//102 - 74
			 2.0f,  1.5f,   6.0f, 	0.020f, 0.051f, 0.110f,	//103 - 75
			 2.0f,  3.47f,  5.2f,	0.020f, 0.051f, 0.110f,	//104 - 76
			 0.0f,  1.47f,  6.2f,	0.243f, 0.333f, 0.384f, //105 - 77
			 0.0f,  4.0f,   5.2f,	0.243f, 0.333f, 0.384f, //106 - 78
			 1.5f,  6.0f,   4.2f,	0.243f, 0.333f, 0.384f, //107 - 79
			-4.0f, -4.0f,   9.5f,   0.706f, 0.788f, 0.780f,	//108
			-3.0f, -2.0f,  12.0f,	0.706f, 0.788f, 0.780f,	//109
			-2.5f, -1.0f, 11.0f, 	0.706f, 0.788f, 0.780f,	//110
			-0.5f,  1.25f, 9.0f,	0.706f, 0.788f, 0.780f,	//111
			-1.5f,  0.25f, 10.0f,	0.706f, 0.788f, 0.780f,	//112
			-4.5f, -8.0f, 6.5f,		0.169f, 0.114f, 0.114f,	//113
			-3.75f,-5.0f, 12.5f, 	0.169f, 0.114f, 0.114f,	//114
			-3.0f, -3.0f, 14.0f,	0.169f, 0.114f, 0.114f,	//115
			-1.0f,  0.5f, 12.0f,	0.169f, 0.114f, 0.114f,	//116
			-1.5f, -2.0f, 14.0f,	0.020f, 0.051f, 0.110f,	//117
			-3.0f, -1.5f, 14.5f,	0.020f, 0.051f, 0.110f,	//118
			-1.0f, -1.0f, 14.5f,	0.020f, 0.051f, 0.110f,	//119
			-1.0f, -3.0f, 14.0f,	0.020f, 0.051f, 0.110f,	//120
			-2.0f, -3.5f, 14.0f,	0.169f, 0.114f, 0.114f, //121
			-0.75f,-4.0f, 13.5f,	0.169f, 0.114f, 0.114f,	//122
			-0.5f, -8.0f, 6.5f,		0.169f, 0.114f, 0.114f,	//123
			-0.5f, -5.0f, 12.5f, 	0.169f, 0.114f, 0.114f,	//124
			-5.0f, -8.5f, 2.0f, 	0.741f, 0.569f, 0.380f, //125
			-0.5f, -8.5f, 2.0f, 	0.169f, 0.114f, 0.114f, //126
			4.0f,  -4.0f, 9.5f,   	0.706f, 0.788f, 0.780f,	//127 - 108
			3.0f,  -2.0f, 12.0f,	0.706f, 0.788f, 0.780f,	//128 - 109
			2.5f,  -1.0f, 11.0f, 	0.706f, 0.788f, 0.780f,	//129 - 110
			0.5f,   1.25f,9.0f,		0.706f, 0.788f, 0.780f,	//130 - 111
			1.5f,   0.25f,10.0f,	0.706f, 0.788f, 0.780f,	//140 - 112
			4.5f,  -8.0f, 6.5f,		0.169f, 0.114f, 0.114f,	//141 - 113
			3.75f, -5.0f, 12.5f, 	0.169f, 0.114f, 0.114f,	//142 - 114
			3.0f,  -3.0f, 14.0f,	0.169f, 0.114f, 0.114f,	//143 - 115
			1.0f,   0.5f, 12.0f,	0.169f, 0.114f, 0.114f,	//144 - 116
			1.5f,  -2.0f, 14.0f,	0.020f, 0.051f, 0.110f,	//145 - 117
			3.0f,  -1.5f, 14.5f,	0.020f, 0.051f, 0.110f,	//146 - 118
			1.0f,  -1.0f, 14.5f,	0.020f, 0.051f, 0.110f,	//147 - 119
			1.0f,  -3.0f, 14.0f,	0.020f, 0.051f, 0.110f,	//148 - 120
			2.0f,  -3.5f, 14.0f,	0.169f, 0.114f, 0.114f, //149 - 121
			0.75f, -4.0f, 13.5f,	0.169f, 0.114f, 0.114f,	//150 - 122
			0.5f,  -8.0f, 6.5f,		0.169f, 0.114f, 0.114f,	//151 - 123
			0.5f,  -5.0f, 12.5f, 	0.169f, 0.114f, 0.114f,	//152 - 124
			5.0f,  -8.5f, 2.0f, 	0.741f, 0.569f, 0.380f, //153 - 125
			0.5f,  -8.5f, 2.0f, 	0.169f, 0.114f, 0.114f,  //154 - 126
	};

	GLuint indices [] = {
			0,1,2,		//1
			2,1,3,		//2
			1,4,3,		//3
			3,4,5,		//4
			4,6,5,		//5
			1,6,4,		//6
			1,7,6,		//7
			6,8,5,		//8
			7,9,6,		//9
			6,9,8,		//10
			5,8,10,		//11
			9,11,8,		//12
			11,10,8,	//13
			11,12,10,	//14
			10,13,12,	//15
			14,13,10,	//16
			10,13,14,	//17
			14,12,13,	//18
			18,17,19,	//19
			17,21,19,	//20
			19,21,20,	//21
			20,21,15,	//22
			17,16,21,	//23
			21,16,15,	//24
			18,14,13,	//25
			13,17,18,	//26
			17,13,12,	//27
			12,16,17,	//28
			18,13,16,	//29
			18,17,16,	//30
			20,15,22,	//31
			22,15,11,	//32
			22,15,9,	//33
			9,15,11,	//34
			11,15,16,	//35
			16,12,11,	//36
			14,24,10,	//37
			5,8,24,		//38
			8,10,24,	//39
			18,23,24,	//40
			25,18,19,	//41
			25,19,20,	//42
			25,20,22,	//43
			18,25,22,	//44
			26,27,28,	//45
			28,27,29,	//46
			27,30,29,	//47
			29,30,31,	//48
			30,32,31,	//49
			27,32,30,	//50
			27,33,32,	//51
			32,34,31,	//52
			33,35,32,	//53
			32,35,34,	//54
			31,34,36,	//55
			35,37,34,	//56
			37,36,34,	//57
			37,38,36,	//58
			36,39,38,	//59
			40,39,36,	//60
			36,39,40,	//61
			40,38,39,	//62
			44,43,45,	//63
			43,47,45,	//64
			45,47,46,	//65
			46,47,41,	//66
			43,42,47,	//67
			47,42,41,	//68
			44,40,39,	//69
			39,43,44,	//70
			43,39,38,	//71
			38,42,43,	//72
			44,39,42,	//73
			44,43,42,	//74
			46,41,48,	//75
			48,41,37,	//76
			48,41,35,	//77
			35,41,37,	//78
			48,41,35,	//79
			35,41,37,	//80
			37,41,42,	//81
			42,38,37,	//82
			40,50,36,	//83
			31,34,50,	//84
			34,36,50,	//85
			44,49,50,	//86
			51,44,45,	//87
			51,45,46,	//88
			51,46,48,	//89
			44,51,48,	//90
			23,24,50,	//91
			24,5,31,	//92
			31,50,24,	//93
			22,52,18,	//94
			7,52,9,		//95
			52,22,9,	//96
			52,53,18,	//97
			53,55,54,	//98
			18,54,53,	//99
			18,54,23,	//100
			52,53,56,	//101
			56,55,53,	//102
			58,57,59,	//103
			57,56,52,	//104
			60,59,61,	//105
			61,59,7,	//106
			57,52,59,	//107
			59,7,52,	//108
			61,52,7,	//109
			58,59,60,	//110
			62,63,1,	//111
			62,7,1,		//112
			69,60,61,	//113
			69,66,61,	//114
			66,61,62,	//115
			62,61,7,	//116
			64,67,62,	//117
			63,65,64,	//118
			63,64,62,	//119
			68,71,67,	//120
			68,72,71,	//121
			71,70,69,	//122
			67,71,69,	//123
			69,70,60,	//124
			65,68,67,	//125
			67,66,62,	//126
			65,67,64,	//127
			67,66,69,	//128
			71,72,70,	//129
			68,72,70,	//130
			68,73,70,	//131
			70,74,73,	//132
			73,77,74,	//133
			77,78,76,	//134
			74,77,75,	//135
			75,77,76,	//136
			57,79,55,	//137
			57,79,76,	//138
			76,58,57,	//139
			76,78,79,	//140
			70,74,60,	//141
			74,75,60,	//142
			75,76,58,	//143
			75,58,60,	//144
			0,63,1,		//145
			48,80,44,	//146
			33,80,35,	//147
			80,48,35,	//148
			80,81,44,	//149
			81,83,82,	//150
			44,82,81,	//151
			44,82,49,	//152
			80,81,84,	//153
			84,83,81,	//154
			86,85,87,	//155
			85,84,80,	//156
			88,87,89,	//157
			89,87,33,	//158
			85,80,87,	//159
			87,33,80,	//160
			89,80,33,	//161
			86,87,88,	//162
			90,91,27,	//163
			90,33,27,	//164
			97,88,89,	//165
			97,94,89,	//166
			94,89,90,	//167
			90,89,33,	//168
			92,95,90,	//169
			91,93,92,	//170
			91,92,90,	//171
			96,99,95,	//172
			96,100,99,	//173
			99,98,97,	//174
			95,99,97,	//175
			97,98,88,	//176
			93,96,95,	//177
			95,94,90,	//178
			93,95,92,	//179
			95,94,97,	//180
			99,100,98,	//181
			96,100,98,	//182
			96,101,98,	//183
			98,102,101,	//184
			101,105,102,//185
			105,106,104,//186
			102,105,103,//187
			103,105,104,//188
			85,107,83,	//189
			85,107,104,	//190
			104,106,107,//191
			98,102,88,	//192
			102,103,88,	//193
			103,104,86,	//194
			103,86,88,	//195
			26,91,27,	//196
			104,86,85,	//197
			57,56,55,	//198
			85,84,83,	//199
			79,107,78,	//200
			79,107,55,	//201
			108,109,72,	//202
			109,110,72,	//203
			110,112,73,	//204
			110,73,72,	//205
			112,111,73,	//206
			73,111,74,	//207
			108,72,65,	//208
			111,77,74,	//209
			113,108,65,	//210
			113,114,108,//211
			114,115,108,//212
			108,115,109,//213
			115,116,109,//214
			110,116,112,//215
			112,116,111,//216
			109,116,110,//217
			113,125,65,	//218
			125,65,63,	//219
			117,118,119,//220
			115,117,118,//221
			115,118,116,//222
			118,119,116,//223
			115,120,118,//224
			118,120,119,//225
			121,122,120,//226
			121,120,115,//227
			121,115,114,//228
			121,122,114,//229
			114,124,123,//230
			114,123,113,//231
			113,123,126,//232
			113,125,126,//233
			114,124,122,//234
			126,125,63,	//235
			127,128,100,//236
			128,129,100,//237
			129,131,101,//238
			129,101,100,//239
			131,130,101,//240
			101,130,102,//241
			127,100,93,	//242
			130,105,102,//243
			132,127,93,	//244
			132,133,127,//245
			133,134,127,//246
			127,134,128,//247
			134,135,128,//248
			129,135,131,//249
			131,135,130,//250
			128,135,129,//251
			132,144,93,	//252
			144,93,91,	//253
			136,137,138,//254
			134,136,137,//255
			134,137,135,//256
			137,138,135,//257
			134,139,137,//258
			137,139,138,//259
			140,141,139,//260
			140,139,134,//261
			140,134,133,//262
			140,141,133,//263
			133,143,142,//264
			133,142,132,//265
			132,142,145,//266
			132,144,145,//267
			133,143,141,//268
			145,144,91,	//269
			111,130,77,	//270
			116,130,111,//271
			116,135,130,//272
			116,119,138,//273
			116,138,135,//274
			119,120,139,//275
			119,139,138,//276
			120,122,141,//277
			120,141,139,//278
			122,114,133,//279
			122,133,141,//280
			114,113,132,//281
			114,132,133,//282
			113,126,145,//283
			113,145,132	//284
	};


	vector <glm::vec3> normals;
	glm::vec3
		edge1,
		edge2,
		crossResult,
		normalizedResult;

	int numOfTriangles = (sizeof(indices)/sizeof(indices[0])/3);
	cout << numOfTriangles << endl;

	//Calculate Normals
	for(int i = 0, z = 0; i < numOfTriangles; i+=3, z++)
	{
		edge1 = glm::vec3(vertices[indices[i]*6], vertices[(indices[i]*6) + 1], vertices[(indices[i]*6) + 2])
		- glm::vec3(vertices[indices[i+1]*6], vertices[(indices[i+1]*6) + 1], vertices[(indices[i+1]*6) + 2]);

		edge2 = glm::vec3(vertices[indices[i]*6], vertices[(indices[i]*6) + 1], vertices[(indices[i]*6) + 2])
				- glm::vec3(vertices[indices[i+2]*6], vertices[(indices[i+2]*6) + 1], vertices[(indices[i+2]*6) + 2]);

		crossResult = glm::cross(edge1, edge2);
		normalizedResult = glm::normalize(crossResult);


		normals.push_back(normalizedResult);
	}

	cout << normals.size() << endl;
	int normalContainerSize = normals.size() * 3;
	GLfloat normalArray [normalContainerSize];
	for(int i = 0; i< normalContainerSize; i+=3)
	{
		normalArray[i] = normals[i].x;
		normalArray[i+1] = normals[i].y;
		normalArray[i+2] = normals[i].z;
	}

//	for (int i = 0; i < normals.size(); i ++)
//	{
//		cout <<normals[i].x << ", " << normals[i].y << ", " << normals[i].z << endl;
//	}

	indicesLength = sizeof(indices)/sizeof(indices[0]);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	ExitOnGLError("ERROR: Could not enable color data");

	glBindBuffer(GL_ARRAY_BUFFER,NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalArray), normalArray, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void RenderGraphics(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));	//Place the object at the center of the viewport
	model = glm::rotate(model, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));	//Rotate the object 45 degrees about the y-axis
	model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));	//Scales down the object by 25% in the xyz scale

	glm::mat4 view;
	view = glm::lookAt(cameraPosition - front, cameraPosition + front, cameraUpY);

	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc = glGetUniformLocation(shaderProgram, "view");
	projLoc = glGetUniformLocation(shaderProgram, "projection");

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

	glDrawElements(GL_TRIANGLES, indicesLength, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);	//Deactivates the VAO
	glutSwapBuffers();		//Swaps the buffers of the current window if double buffered
}

void IdleFunction(void)
{
	glutPostRedisplay();	//Marks the current window as needing to be redisplayed
}

void DestroyGraphics(void)
{
	glDeleteProgram(shaderProgram);
	ExitOnGLError("ERROR: Could not destroy the shaders");

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");
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
}














