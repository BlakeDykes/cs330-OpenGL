/*
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define WINDOW_TITLE "Modern OpenGL"

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

GLint
	cubeShaderProgram,
	windowWidth = 800,
	windowHeight = 600;

GLuint
	VBO,
	cubeVAO,
	lightVAO,
	texture;

//Subject position and scale
glm::vec3 cubePosition(0.0f, 0.0f, 0.0f);
glm::vec3 cubeScale(2.0f);

//Cube and light color
glm::vec3 objectColor(0.6f, 0.5f, 0.75f);
glm::vec3 lightColor(1.0f, 0.0f, 0.0f);

//Light position and scale
glm::vec3 lightPosition(0.5f, 0.5f, -3.0f);
glm::vec3 lightScale(0.3f);

//Camera position
glm::vec3 cameraPosition(0.0f, 0.0f, -6.0f);

//Camera rotation
float cameraRotation = glm::radians(-25.0f);

void ResizeWindow(int, int);
void RenderGraphics(void);
void CreateShaders(void);
void CreateBuffers(void);
//void GenerateTexture(void);
void ExitOnGLError(const char*);

const GLchar * cubeVertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position;	//VAP position 0 for vertex position data
	layout (location = 1) in vec3 normal; //VAP position 1 for normals

	out vec3 Normal;	//For outgoing normals to fragment shader
	out vec3 FragmentPos;	//For outgoing color/ pixels to fragment shader

	//Uniform / Global variables for the transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main(){
		gl_Position = projection * view * model * vec4(position, 1.0f);	//Transforms vertices into clip coordinates

		FragmentPos = vec3(model * vec4(position, 1.0f));	//Gets fragment / pixel position in world space only (exclude view and projection)

		Normal = mat3(transpose(inverse(model))) * normal;	//Get normal vectors in world space only and exclude normal translation properties
	}
);

// Cube Fragment Shader Source Code
const GLchar * cubeFragmentShaderSource = GLSL(330,

	in vec3 Normal; //For incoming normals
	in vec3 FragmentPos;	//For incoming fragment position

	out vec4 cubeColor; //For outgoing cube color to the GPU

	//Uniform / Global variables for object color, light color, light position, and camera/view position
	uniform vec3 objectColor;
	uniform vec3 lightColor;
	uniform vec3 lightPos;
	uniform vec3 viewPosition;

	void main(){
	//Phong lighting model calculations to generate ambient, diffuse, and specular components

		//Calculate ambient lighting
		float ambientStrength = 0.1f;	//Set ambient or global lighting strength
		vec3 ambient = ambientStrength * lightColor;

		//Calculate diffuse lighting
		vec3 norm = normalize(Normal);	//Normalize vectors to 1 unit
		vec3 lightDirection = normalize(lightPos - FragmentPos);	//Calculate distance(light direction) between light source and fragments/pixels on
		float impact = max(dot(norm, lightDirection), 0.0);	//Calculate diffuse impact by generating dot product of normal and light
		vec3 diffuse = impact * lightColor;	//Generate diffuse light color

		//Calculate specular lighting
		float specularIntensity = 0.8f;	//Set specular light strength
		float highlightSize = 16.0f;	//Set specular highlight size
		vec3 viewDir = normalize(viewPosition - FragmentPos);	//Calculate view direction
		vec3 reflectDir = reflect(-lightDirection, norm);	//Calculate reflection vector
		//Calculate specular component
		float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
		vec3 specular = specularIntensity * specularComponent * lightColor;

		//Calculate phong result
		vec3 phong = (ambient + diffuse + specular) * objectColor;

		cubeColor = vec4(phong, 1.0f);
	}
);

const GLchar * lampVertexShaderSource = GLSL(330,

	layout (location = 0) in vec3 position;	//VAP position 0 for vertex position data

	//Uniform / Global variables for the transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f);	//Transforms vertices into clip coordinates
	}

);

const GLchar * lampFragmentShaderSource = GLSL(330,

	out vec4 color;	//For outgoing lamp color ( smaller cube) to the GPU

	void main()
	{
		color = vec4(1.0f);	//Set color to white ( 1.0f, 1.0f, 1.0f) with alpha 1.0
	}

);

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(ResizeWindow);

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK){
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}

	CreateShaders();
	CreateBuffers();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glutDisplayFunc(RenderGraphics);

	glutMainLoop();

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	return 0;
}

void ResizeWindow(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
	glViewport(0, 0, windowWidth, windowHeight);
}

void RenderGraphics (void)
{
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the cube shader and activate the cube vertex array object for rendering and transforming
	glUseProgram(cubeShaderProgram);
	ExitOnGLError("ERROR: Could not use cubeShaderProgram");
	glBindVertexArray(cubeVAO);

	glm::mat4 model;
	model = glm::translate(model, cubePosition);
	model = glm::scale(model, cubeScale);

	glm::mat4 view;
	view = glm::translate(view, cameraPosition);
	view = glm::rotate(view, cameraRotation, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);

	//Reference matrix unifroms from the cube shader program
	GLint modelLoc = glGetUniformLocation(cubeShaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(cubeShaderProgram, "view");
	GLint projLoc = glGetUniformLocation(cubeShaderProgram, "projection");
	ExitOnGLError("Getting cube uniforms");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//Reference matrix uniforms from the cube shader program for the cube color, light color, light position, and camera position
	GLint objectColorLoc = glGetUniformLocation(cubeShaderProgram, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(cubeShaderProgram, "lightColor");
	GLint lightPositionLoc = glGetUniformLocation(cubeShaderProgram, "lightPos");
	GLint viewPositionLoc = glGetUniformLocation(cubeShaderProgram, "viewPosition");
	ExitOnGLError("Getting color uniforms");

	//Pass color, light, and camera data to the cube shader program's corresponding uniforms
	glUniform3f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b);
	glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
	glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	glDrawArrays(GL_TRIANGLES, 0, 36);	//Draw the primitives /cube
	ExitOnGLError("ERROR: 1st draw");

	glBindVertexArray(0);

	//Use the lamp shader and activate the lamp vertex array object for rendering and transforming
	glUseProgram(lampShaderProgram);
	ExitOnGLError("ERROR: Could not use lampShaderProgram");
	glBindVertexArray(lightVAO);

	//Transform the smaller cube used as a visual que for the light source
	model = glm::translate(model, lightPosition);
	model = glm::scale(model, lightScale);

	//Reference matrix uniforms from the lamp shader program
	modelLoc = glGetUniformLocation(lampShaderProgram, "model");
	viewLoc = glGetUniformLocation(lampShaderProgram, "view");
	projLoc = glGetUniformLocation(lampShaderProgram, "projection");
	ExitOnGLError("Getting lamp uniforms");

	//Pass matrix data to the lamp shader programs matrix uniforms
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	ExitOnGLError("Passing lamp uniforms");

	glDrawArrays(GL_TRIANGLES, 0, 36);	//Draw the primitives / small cube(lamp)
	ExitOnGLError("ERROR: 2nd draw");

	glBindVertexArray(0);	//Deactivate the lamp vertex array object

	glutPostRedisplay();	//marks the current window to be redisplayed
	glutSwapBuffers();	//Flips the back buffer with the front buffer every frame
}

void CreateShaders()
{
	GLint cubeVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(cubeVertexShader, 1, &cubeVertexShaderSource, NULL);
	glCompileShader(cubeVertexShader);
	ExitOnGLError("Compiling cubeVertexShader");

	GLint cubeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(cubeFragmentShader, 1, &cubeFragmentShaderSource, NULL);
	glCompileShader(cubeFragmentShader);
	ExitOnGLError("Compiling cubeFragmentShader");

	cubeShaderProgram = glCreateProgram();
	glAttachShader(cubeShaderProgram, cubeVertexShader);
	glAttachShader(cubeShaderProgram, cubeFragmentShader);
	glLinkProgram(cubeShaderProgram);
	ExitOnGLError("Linking cubeShaderProgram");

	glDeleteShader(cubeVertexShader);
	glDeleteShader(cubeFragmentShader);

	GLint lampVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(lampVertexShader, 1, &lampVertexShaderSource, NULL);
	glCompileShader(lampVertexShader);
	ExitOnGLError("Compiling lampVertexShader");

	GLint lampFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(lampFragmentShader, 1, &lampFragmentShaderSource, NULL);
	glCompileShader(lampFragmentShader);
	ExitOnGLError("Compiling lampFragmentShader");

	lampShaderProgram = glCreateProgram();
	glAttachShader(lampShaderProgram, lampVertexShader);
	glAttachShader(lampShaderProgram, lampFragmentShader);
	glLinkProgram(lampShaderProgram);
	ExitOnGLError("compiling lampShaderProgram");

	glDeleteShader(lampVertexShader);
	glDeleteShader(lampFragmentShader);

}


void CreateBuffers()
{
	GLfloat vertices[] = {
			//Positions				//Normals

			//Back Face				//-z normals
			-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f, 	0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f, 	0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f, 	0.0f, 0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f, 	0.0f, 0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f, 	0.0f, 0.0f, -1.0f,

			//Front Face			//+z normals
			-0.5f, -0.5f, 0.5f, 	0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.5f, 	0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, 0.5f, 	0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, 0.5f, 	0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f, 0.5f, 	0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 	0.0f, 0.0f, 1.0f,

			//Left Face				//-x normals
			-0.5f,  0.5f,  0.5f, 	-1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, 	-1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 	-1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 	-1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, 	-1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, 	-1.0f, 0.0f, 0.0f,

			//Right Face			//+x normals
			 0.5f,  0.5f,  0.5f, 	1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, 	1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, 	1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, 	1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, 	1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 	1.0f, 0.0f, 0.0f,

			 //Bottom Face			//-y normals
			-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,

			//Top Face				//+y normals
			-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
			 0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
			 0.5f, 0.5f,  0.5f,		0.0f, 1.0f, 0.0f,
			 0.5f, 0.5f,  0.5f,		0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f,  0.5f,		0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f
	};


	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);


	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	ExitOnGLError("ERROR: 1");

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) (sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	ExitOnGLError("ERROR: 2");

	glBindVertexArray(0);

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	ExitOnGLError("ERROR: 3");

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

//void GenerateTexture(){
//
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//
//	int width, height;
//
//	unsigned char* image = SOIL_load_image("SNHU.JPG", &width, &height, 0, SOIL_LOAD_RGB);	//Loads texture file
//
//	if(!image)
//	{
//		cout << "Failed to load texture" << endl;
//	}
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//	glGenerateMipmap(GL_TEXTURE_2D);
//	SOIL_free_image_data(image);
//	glBindTexture(GL_TEXTURE_2D, 0);	//Unbind the texture
//}



void ExitOnGLError(const char* error_message)
{
  const GLenum ErrorValue = glGetError();

  if (ErrorValue != GL_NO_ERROR)
  {
    fprintf(stderr, "%s: %s\n", error_message, gluErrorString(ErrorValue));
    exit(EXIT_FAILURE);
  }
}

*/





















