/*
 * Final.cpp
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
#include <glm/gtc/type_ptr.hpp>

#include "SOIL2/SOIL2.h"

using namespace std;


#define WINDOW_TITLE "Macho"
const float PI = 3.1415926;

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

GLint
	windowWidth = 1600,
	windowHeight = 1200,
	shaderProgram,
	modelLoc,
	viewLoc,
	projLoc,
	lightColorLoc,
	keyLightPosLoc,
	fillLightPosLoc,
	viewPositionLoc;

GLuint
	VBO,
	VAO,
	NBO,
	EBO,
	TBO,
	texture;

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

//const char* fur = "Fur1.jpg";

//Light color
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

//Light position
glm::vec3 keyLightPosition(-5.0f, 8.0f, 12.0f);


void Initialize(int argc, char* argv[]);
void ResizeWindow(int, int);
void RenderGraphics(void);
void CreateShaders(void);
void CreateBuffers(void);
void GenerateTexture(const char* filename);
void IdleFunction(void);
void DestroyGraphics(void);
void MouseClick(int button, int state, int x, int y);
void MousePressedMove(int x, int y);
void ExitOnGLError(const char* error_message);
GLfloat * CalculateNormals(GLfloat vertices [], GLuint indices []);

const GLchar * vertexShaderSource = GLSL(330,
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec3 color;
		layout (location = 2) in vec3 normals;
//		layout (location = 3) in vec2 textureCoordinates;

		out vec3 mobileColor;
		out vec3 mobileNormals;
//		out vec2 mobileTextureCoordinates;
		out vec3 FragmentPos;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

	void main(void){
		gl_Position = projection * view * model * vec4(position, 1.0f);

		mobileColor = color;

		mobileNormals = mat3(transpose(inverse(model))) * normals;
//		mobileTextureCoordinates = vec2(textureCoordinates.x, 1.0f - textureCoordinates.y);

		FragmentPos = vec3(model * vec4(position, 1.0f));
	}
);

const GLchar * fragmentShaderSource = GLSL(330,
		in vec3 mobileColor;
		in vec3 mobileNormals;
//		in vec2 mobileTextureCoordinates;
		in vec3 FragmentPos;

		out vec4 FragColor;

//		uniform sampler2D uTexture;

		uniform vec3 lightColor;
		uniform vec3 keyLightPos;
		uniform vec3 viewPosition;

	void main(void){
				float ambientStrength = 0.3f;

				//Key light adjustments
				float keySpecularIntensity = 1.25f;
				float keyHighlightSize = 0.75f;

				//Phong lighting model calculations to generate ambient, diffuse, and specular components

				vec3 viewDir = normalize(-FragmentPos);
				vec3 ambient = ambientStrength * lightColor;
				vec3 norm = mobileNormals;

				//Key Light
				//Diffuse
				vec3 keyLightDir = normalize(keyLightPos - FragmentPos); //Calculate the distance from light source to object
				float keyImpact = max(dot(norm, keyLightDir), 0.0);	//Calculate diffuse impact by generating dot product of normal and light
				vec3 keyDiffuse = keyImpact * lightColor; 	//Generate diffuse light

				//Specular
				vec3 keyReflectDir = reflect(-keyLightDir, norm);	//Calculate reflection vector
				float keySpecularComponent = pow(max(dot(viewDir, keyReflectDir), 0.0), keyHighlightSize);
				vec3 keySpecular = keySpecularIntensity * keySpecularComponent * lightColor;


//				vec3 objectTexture = texture(uTexture, mobileTextureCoordinates).xyz;
				vec3 result = (ambient + keyDiffuse + keySpecular) * mobileColor;


				//Sends light result to gpu
				FragColor = vec4(result, 1.0f);
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

	glEnable(GL_TEXTURE_2D);

	CreateShaders();
	CreateBuffers();
//	GenerateTexture(fur);

	glUseProgram(shaderProgram);
	ExitOnGLError("ERROR: Could not use shader program");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glutReshapeFunc(ResizeWindow);
	glutDisplayFunc(RenderGraphics);	//Sets the display callback for the current window
	glutIdleFunc(IdleFunction);
	glutCloseFunc(DestroyGraphics);
	glutMotionFunc(MousePressedMove);	//Sets the motion callback for the current window
	glutMouseFunc(MouseClick);			//Sets the mouse callback for the current window

	cout << "*****WELCOME TO MACHO'S HEAD*****" <<  endl <<
			"*********************************" <<  endl <<
			"*woof***** ^..^      / **********" <<  endl <<
			"*****woof> /_/\\_____/ ***********" <<  endl <<
			"**********    /\\  /\\ ***********" << endl <<
			"**********   /  \\/  \\ **********" << endl;

	cout << endl << "CONTROLS: Hold the alt key to access mouse controls" << endl <<
			"Left click to rotate" << endl << "Right click to zoom" << endl;
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
			1.5f,   0.25f,10.0f,	0.706f, 0.788f, 0.780f,	//131 - 112
			4.5f,  -8.0f, 6.5f,		0.169f, 0.114f, 0.114f,	//132 - 113
			3.75f, -5.0f, 12.5f, 	0.169f, 0.114f, 0.114f,	//133 - 114
			3.0f,  -3.0f, 14.0f,	0.169f, 0.114f, 0.114f,	//134 - 115
			1.0f,   0.5f, 12.0f,	0.169f, 0.114f, 0.114f,	//135 - 116
			1.5f,  -2.0f, 14.0f,	0.020f, 0.051f, 0.110f,	//136 - 117
			3.0f,  -1.5f, 14.5f,	0.020f, 0.051f, 0.110f,	//137 - 118
			1.0f,  -1.0f, 14.5f,	0.020f, 0.051f, 0.110f,	//138 - 119
			1.0f,  -3.0f, 14.0f,	0.020f, 0.051f, 0.110f,	//139 - 120
			2.0f,  -3.5f, 14.0f,	0.169f, 0.114f, 0.114f, //140 - 121
			0.75f, -4.0f, 13.5f,	0.169f, 0.114f, 0.114f,	//141 - 122
			0.5f,  -8.0f, 6.5f,		0.169f, 0.114f, 0.114f,	//142 - 123
			0.5f,  -5.0f, 12.5f, 	0.169f, 0.114f, 0.114f,	//143 - 124
			5.0f,  -8.5f, 2.0f, 	0.741f, 0.569f, 0.380f, //144 - 125
			0.5f,  -8.5f, 2.0f, 	0.169f, 0.114f, 0.114f  //145 - 126
	};

		GLuint indices [] = {
			0,   1,		2,	//1
			2,   1,		3,	//2
			1,   4,		3,	//3
			3,   4,		5,	//4
			4,   6,		5,	//5
			1,   6,		4,	//6
			1,   7,		6,	//7
			6,   8,		5,	//8
			7,   9,		6,	//9
			6,   9,		8,	//10
			5,   8,		10,	//11
			9,   11,	8,	//12
			11,  10,	8,	//13
			11,  12,	10,	//14
			10,  12,	13,	//15
			14,  13,	10,	//16
			10,  13,	14,	//17
			14,  12,	13,	//18
			18,  17,	19,	//19
			17,  21,	19,	//20
			19,  21,	20,	//21
			20,  21,	15,	//22
			17,  16,	21,	//23
			21,  16,	15,	//24
			18,  14,	13,	//25
			13,  17,	18,	//26
			17,  13,	12,	//27
			12,  16,	17,	//28
			18,  13,	16,	//29
			18,  17,	16,	//30
			20,  15,	22,	//31
			22,  15,	11,	//32
			22,  15,	9,	//33
			9,   15,	11,	//34
			11,  15,	16,	//35
			16,  12,	11,	//36
			14,  24,	10,	//37
			5,   8,		24,	//38
			8,   10,	24,	//39
			18,  23,	24,	//40
			25,  18,	19,	//41
			25,  19,	20,	//42
			25,  20,	22,	//43
			18,  25,	22,	//44
			26,  28,	27,	//45
			28,  29,	27,	//46
			27,  29,	30,	//47
			29,  31,	30,	//48
			30,  31,	32,	//49
			27,  30,	32,	//50
			27,  32,	33,	//51
			32,  31,	34,	//52
			33,  32,	35,	//53
			32,  34,	35,	//54
			31,  36,	34,	//55
			37,  34,	36,	//57
			37,  36,	38,	//58
			36,  39,	38,	//59
			40,  36,	39,	//60
			36,  40,	39,	//61
			40,  38,	39,	//62
			44,  45,	43,	//63
			43,  45,	47,	//64
			45,  46,	47,	//65
			46,  41,	47,	//66
			35,  34,	37,	//56
			43,  47,	42,	//67
			47,  41,	42,	//68
			44,  39,	40,	//69
			39,  44,	43,	//70
			43,  38,	39,	//71
			38,  43,	42,	//72
			44,  42,	39,	//73
			44,  42,	43,	//74
			46,  48,	41,	//75
			48,  37,	41,	//76
			48,  35,	41,	//77
			35,  37,	41,	//78
			48,  35,	41,	//79
			35,  37,	41,	//80
			37,  42,	41,	//81
			42,  37,	38,	//82
			40,  36,	50,	//83
			31,  50,	34,	//84
			34,  50,	36,	//85
			44,  50,	49,	//86
			51,  45,	44,	//87
			51,  46,	45,	//88
			51,  48,	46,	//89
			44,  48,	51,	//90
			23,  50,	24,	//91
			24,  31,	5,	//92
			31,  24,	50,	//93
			22,  52,	18,	//94
			7,   52,	9,	//95
			52,  22,	9,	//96
			52,  53,	18,	//97
			53,  55,	54,	//98
			18,  53,	54,	//99
			18,  54,	23,	//100
			52,  56,	53,	//101
			56,  53,	55,	//102
			58,  57,	59,	//103
			57,  56,	52,	//104
			60,  59,	61,	//105
			61,  59,	7,	//106
			57,  52,	59,	//107
			59,  52,	7,	//108
			61,  7,		52,	//109
			58,  59,	60,	//110
			62,  1,		63,	//111
			62,  7,		1,	//112
			69,  60,	61,	//113
			69,  61,	66,	//114
			66,  61,	62,	//115
			62,  61,	7,	//116
			64,  67,	62,	//117
			63,  65,	64,	//118
			63,  64,	62,	//119
			68,  71,	67,	//120
			68,  72,	71,	//121
			71,  70,	69,	//122
			67,  71,	69,	//123
			69,  70,	60,	//124
			65,  68,	67,	//125
			67,  66,	62,	//126
			65,  67,	64,	//127
			67,  69,	66,	//128
			71,  72,	70,	//129
			68,  72,	70,	//130
			68,  73,	70,	//131
			70,  74,	73,	//132
			73,  77,	74,	//133
			77,  78,	76,	//134
			74,  77,	75,	//135
			75,  77,	76,	//136
			57,  79,	55,	//137
			57,  76,	79,	//138
			76,  57,	58,	//139
			76,  78,	79,	//140
			70,  74,	60,	//141
			74,  75,	60,	//142
			75,  76,	58,	//143
			75,  58,	60,	//144
			0,   63,	1,	//145
			48,  44,	80,	//146
			33,  35,	80,	//147
			80,  35,	48,	//148
			80,  44,	81,	//149
			81,  82,	83,	//150
			44,  82,	81,	//151
			44,  49,	82,	//152
			80,  81,	84,	//153
			84,  81,	83,	//154
			86,  87,	85,	//155
			85,  80,	84,	//156
			88,  89,	87,	//157
			89,  33,	87,	//158
			85,  87,	80,	//159
			87,  33,	80,	//160
			89,  80,	33,	//161
			86,  88,	87,	//162
			90,  91,	27,	//163
			90,  27,	33,	//164
			97,  89,	88,	//165
			97,  94,	89,	//166
			94,  90,	89,	//167
			90,  33,	89,	//168
			92,  90,	95,	//169
			91,  92,	93,	//170
			91,  90,	92,	//171
			96,  99,	95,	//172
			96,  100,	99,	//173
			99,  97,	98,	//174
			95,  97,	99,	//175
			97,  88,	98,	//176
			93,  95,	96,	//177
			95,  90,	94,	//178
			93,  92,	95,	//179
			95,  94,	97,	//180
			99,  98,	100,//181
			96,  98,	100,//182
			96,  98,	101,//183
			98,  102,	101,//184
			101, 102,	105,//185
			105, 104,	106,//186
			102, 103,	105,//187
			103, 104,	105,//188
			85,  83,	107,//189
			85,  107,	104,//190
			104, 107,	106,//191
			98,  88,	102,//192
			102, 88,	103,//193
			103, 86,	104,//194
			103, 88,	86,	//195
			26,  27,	91,	//196
			104, 86,	85,	//197
			57,  55,	56,	//198
			85,  84,	83,	//199
			79,  78,	107,//200
			79,  107,	55,	//201
			108, 109,	72,	//202-
			109, 110,	72,	//203
			110, 112,	73,	//204
			110, 73,	72,	//205
			112, 111,	73,	//206
			73,  111,	74,	//207
			108, 72,	65,	//208
			111, 77,	74,	//209
			113, 108,	65,	//210
			113, 114,	108,//211
			114, 115,	108,//212
			108, 115,	109,//213
			115, 116,	109,//214
			110, 116,	112,//215
			112, 116,	111,//216
			109, 116,	110,//217
			113, 65,	125,//218
			125, 65,	63,	//219
			117, 118,	119,//220
			115, 117,	118,//221
			115, 118,	116,//222
			118, 119,	116,//223
			115, 120,	118,//224
			118, 120,	119,//225
			121, 122,	120,//226
			121, 120,	115,//227
			121, 115,	114,//228
			121, 114,	122,//229
			114, 124,	123,//230
			114, 123,	113,//231
			113, 123,	126,//232
			113, 125,	126,//233
			114, 124,	122,//234
			126, 125,	63,	//235
			127, 100,	128,//236
			128, 100,	129,//237
			129, 101,	131,//238
			129, 100,	101,//239
			131, 101,	130,//240
			101, 102,	130,//241
			127, 93,	100,//242
			130, 102,	105,//243
			132, 93,	127,//244
			132, 127,	133,//245
			133, 127,	134,//246
			127, 128,	134,//247
			134, 128,	135,//248
			129, 131,	135,//249
			131, 130,	135,//250
			128, 129,	135,//251
			132, 144,	93,	//252
			144, 91,	93,	//253
			136, 137,	138,//254
			134, 137,	136,//255
			134, 135,	137,//256
			137, 135,	138,//257
			134, 137,	139,//258
			137, 138,	139,//259
			140, 139,	141,//260
			140, 134,	139,//261
			140, 133,	134,//262
			140, 141,	133,//263
			133, 142,	143,//264
			133, 132,	142,//265
			132, 142,	145,//266
			132, 145,	144,//267
			133, 141,	143,//268
			145, 91,	144,//269
			111, 130,	77,	//270
			116, 130,	111,//271
			116, 135,	130,//272
			116, 119,	138,//273
			116, 138,	135,//274
			119, 120,	139,//275
			119, 139,	138,//276
			120, 122,	141,//277
			120, 141,	139,//278
			122, 114,	133,//279
			122, 133,	141,//280
			114, 113,	132,//281
			114, 132,	133,//282
			113, 126,	145,//283
			113, 145,	132,//284
	};

	GLfloat texCoords [] = {
	};

	int numOfTriangles = (sizeof(indices)/sizeof(indices[0])/3);


	GLfloat * normalArray = CalculateNormals(vertices, indices);

	indicesLength = sizeof(indices)/sizeof(indices[0]);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &TBO);

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

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(&normalArray), &normalArray, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);
	ExitOnGLError("ERROR: Could not enable normal data");

	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(&texCoords), texCoords, GL_STATIC_DRAW);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

void GenerateTexture(const char* filename)
{
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	ExitOnGLError("ERROR: Could not generate texture");

	int width, height;

	unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);
//	if(!image)
//		cout << "image failed to load";
//	else
//		cout << "texture loaded just fine";
//	ExitOnGLError("ERROR: Could not load the image");

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	keyLightPosLoc = glGetUniformLocation(shaderProgram, "keyLightPos");
	fillLightPosLoc = glGetUniformLocation(shaderProgram, "fillLightPos");
	viewPositionLoc = glGetUniformLocation(shaderProgram, "viewPosition");
	ExitOnGLError("ERROR: Could not get uniform locations");

	//	glUniform1i(textureLoc, texture);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(keyLightPosLoc, keyLightPosition.x, keyLightPosition.y, keyLightPosition.z);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	ExitOnGLError("ERROR: Could not pass uniform data");

//	glBindTexture(GL_TEXTURE_2D, texture);
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

GLfloat * CalculateNormals(GLfloat vertices [], GLuint indices []) {

	vector <glm::vec3> normals;
	GLuint numOfTriangles = (sizeof(indices)/sizeof(indices[0])/3);

	//Calculate Normals
	for(GLuint i = 0; i < numOfTriangles*3; i+=3)
	{
		glm::vec3 edge1 = glm::vec3(vertices[indices[i+1]*6], vertices[(indices[i+1]*6) + 1], vertices[(indices[i+1]*6) + 2])
		- glm::vec3(vertices[indices[i]*6], vertices[(indices[i]*6) + 1], vertices[(indices[i]*6) + 2]);

		glm::vec3 edge2 = glm::vec3(vertices[indices[i+2]*6], vertices[(indices[i+2]*6) + 1], vertices[(indices[i+2]*6) + 2])
				- glm::vec3(vertices[indices[i]*6], vertices[(indices[i]*6) + 1], vertices[(indices[i]*6) + 2]);

		//List vectors accessed by the algorithm
//		cout << "normal- " << i << endl;
//		cout << "vec1- [" <<  vertices[indices[i]*6] << ", " << vertices[(indices[i]*6) + 1] << ", " << vertices[(indices[i]*6) + 2] << "]" << endl;
//		cout << "vec2- [" <<  vertices[indices[i+1]*6] << ", " << vertices[(indices[i+1]*6) + 1] << ", " << vertices[(indices[i+1]*6) + 2] << "]" <<  endl;
//		cout << "vec3- [" <<  vertices[indices[i+2]*6] << ", " << vertices[(indices[i+2]*6) + 1] << ", " << vertices[(indices[i+2]*6) + 2] << "]" <<  endl << endl;

		glm::vec3 crossProduct = glm::cross(edge1, edge2);
		glm::vec3 normalizedResult = glm::normalize(crossProduct);

		//List edges accessed by the algorithm and the cross product calculated
//		cout << i/3 << ": " << endl;
//		cout << "edge1: " << edge1.x << ", " << edge1.y << ", " << edge1.z << endl;
//		cout << "edge2: " << edge2.x << ", " << edge2.y << ", " << edge2.z << endl;
//		cout << "Cross Product: " << crossProduct.x << ", " << crossProduct.y << ", " << crossProduct.z << endl;

		//List normalized results
//		cout << "NormalizedResult " << i/3 << ": " << normalizedResult.x << ", " << normalizedResult.y << ", " << normalizedResult.z << endl;

		normals.push_back(normalizedResult);
	}

	int normalContainerSize = normals.size();
	GLfloat normArray [normalContainerSize];
	for(int i = 0, z = 0; i < normalContainerSize; i+=3, z++)
	{
		normArray[i] = normals[z].x;
		normArray[i+1] = normals[z].y;
		normArray[i+2] = normals[z].z;
	}

//	for (int i = 0; i < normalContainerSize; i ++)
//	{
//		cout << i << ": "<<normArray[i] << ", " << normArray[i+1] << ", " << normArray[i+2] << endl;
//	}

	return normArray;
}














