/*
 * practice.cpp
 *
 *  Created on: Jul 14, 2020
 *      Author: Blake
 */
/*
#include <iostream> // Includes C++ i/o stream
#include <GL/glew.h> // Includes glew header
#include <GL/freeglut.h> // Includes freeglut header

using namespace std; // Uses the standard namespace

#define WINDOW_TITLE "Modern OpenGL" // Macro for window title

//Vertex and Fragment Shader Source Macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

///Variables for window width and height
int WindowWidth = 800, WindowHeight = 600;

// User-defined Function prototypes to:
  //initialize the program, set the window size,
  //redraw graphics on the window when resized,
 // and render graphics on the screen


void UInitialize(int, char*[]);
void UInitWindow(int, char*[]);
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateVBO(void);
void UCreateShaders(void);


//Vertex Shader Program Source Code
const GLchar * VertexShader = GLSL(440,
		in layout(location=0) vec4 vertex_Position;

//Get the vertex colors from the Vertex Buffer Object
in layout(location=1) vec4 colorFromVBO; //for attribute 1 expect vec(4) floats passed into the Vertex shader.
out vec4 colorFromVShader; //declare a vec 4 variable that will reference the vertex colors passed into the Vertex shader from the buffer.
	void main(){
		gl_Position = vertex_Position; // Sends vertex positions to gl_position vec 4
		colorFromVShader = colorFromVBO; //references vertex colors sent from the buffer
	}
);

//Fragment Shader Program Source Code
const GLchar * FragmentShader = GLSL(440,
		in vec4 colorFromVShader; //Vertex colors from the vertex shader
out vec4 vertex_Color; // vec4 variable that will reference the vertex colors passed into the fragment shader rom the vertex shader
		void main(){
			//gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Sets the pixels / fragments of the tirangle to green
			vertex_Color = colorFromVShader; //Send the vertex colors to the GPU
	}
);

// main function. Entry point to the OpenGL program
int main(int argc, char* argv[])
{
	UInitialize(argc, argv); //Initialize the OpenGL program
	glutMainLoop(); //Starts the OpenGl loop in the background
	exit(EXIT_SUCCESS); //Terminates the program succesfully
}

// implements the UInitialize function
void UInitialize(int argc, char* argv[])
{
	//glew status variable
	GLenum GlewInitResult;

	UInitWindow(argc, argv); //Creates the window

	//Checks glew status
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult){
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	//Display GPU OpenGL version
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

	UCreateVBO(); //Calls the function to create the Vertex Buffer Object

	UCreateShaders(); //Calls the function to create the Shader Program

	//Sets the background color of the window to black. Optional
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Implements the UInitWindow function
void UInitWindow(int argc, char* argv[]){

	//Initializes freeglut
	glutInit(&argc, argv);

	//Sets the window size
	glutInitWindowSize(WindowWidth, WindowHeight);

	//Memory buffer setup for display
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	//Creates a window with the macro placeholder title
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow); // Called when the window is resized
	glutDisplayFunc(URenderGraphics); //Renders graphics on the screen

}

//Implements the UResizeWindow function
void UResizeWindow(int width, int height)
{
	glViewport(0,0, width, height);
}

// Implements the URenderGraphics function
void URenderGraphics(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clears the screen

	//Creates the triangle

	GLuint totalVerticies = 6; //Specifies the number of verticies for the triangle
	//glDrawArrays(GL_TRIANGLES, 0, totalVerticies); //Draws the Triangle

	//Draw the triangle using the indices
	glDrawElements(GL_TRIANGLES, totalVerticies, GL_UNSIGNED_SHORT, NULL);

	glutSwapBuffers(); //Flip the back buffer with the front buffer every frame. Similar to GL Flush

}


//Implements the CreateVBO function
void UCreateVBO(void){

	//Specifies coordinates for triangle verticies on x and y
	GLfloat verts[]=
	{
			//Indices will be 0, 1, 2, 3, 4, 1

			//Index 0
			-0.5f, 0.0f, //Top-center
			1.0f, 0.0f, 0.0f, 1.0f, //Red vertex

			//Index 1
			0.0f, -1.0f, //Bottom-left
			0.0f, 1.0f, 0.0f, 1.0f, //Green vertex

			//index 2
			-1.0f, -1.0f, //Bottom-right
			0.0f, 0.0f, 1.0f, 1.0f, //Blue vertex

			//Index 3
			0.5f, 0.0f, //top-center of the screen
			1.0f, 0.0f, 0.0f, 1.0f, //Red vertex

			//Index 4
			1.0f, -1.0f, //bottom-left of the screen
			0.0f, 1.0f, 0.0f, 1.0f, //Green vertex

			//0.0f, -1.0f, //bottom-right of the screen
			//0.0f, 0.0f, 1.0f, 1.0f //Blue vertex
	};

	//Stores the size of the verts array / number of coordinates needed for the triangle i.e. 6
	float numVertices = sizeof(verts);

	GLuint myBufferID; //Variable for vertex buffer object id
	glGenBuffers(1, &myBufferID); //Creates 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID); //Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, numVertices, verts, GL_STATIC_DRAW); //Sends vertex or coordinate data to the GPU

	//Creates the Vertex Attribute Pointer

	GLuint floatsPerVertex = 2; //Number of coordinates per vertex
	glEnableVertexAttribArray(0); //Specifies the initial position of the coordinates in the buffer

	//Strides between vertex coordinates is 6(x, y, r, g, b, a) A tightly packed stride is 0
	GLint vertexStride = sizeof(float) * 6; //The number of floats before each vertex position is 6 i.e. xy rgba

	//Instructs the GPU on how to handle the vertex buffer object data
  	//Paramters: attribPointerPosition | coordinates per vertex | data type | deactivate normalization | 6 strides | 0 offset


	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, vertexStride, 0); //Change the stride from 0 to 6

	//Sets an attribute pointer position for the vertex colors i.e. Attribute 1 for rgba floats. Attribute 0 was position x,y
	glEnableVertexAttribArray(1); //Specifies position 1 for the color values in the buffer

	GLint colorStride = sizeof(float) * 6; //The number of floats before each color is 6 i.e. rgba xy

	//Parameters: attribPosition 1 | floats per color is 4 i.e rgba | data type | deactivate normalization |
	//6 strides until the next color i.e. rgbaxy | 2 floats until the beginning of each color i.e xy
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, colorStride, (char*)(sizeof(float) * 2));

	//Creates a bufer object for the indices
	GLushort indicies[] = {0,1,2, 3,4,1};
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

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER); //Create a Vertex Shader object
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER); //Create a vertex shader object

	glShaderSource(vertexShaderId, 1, &VertexShader, NULL); //retrieves the vertex shader source code
	glShaderSource(fragmentShaderId, 1, &FragmentShader, NULL); //retrieves the fragment shader source code

	glCompileShader(vertexShaderId); //Compile the vertex shader
	glCompileShader(fragmentShaderId); //Compile the fragment shader

	//Attaches the vertex and fragment shaders to the shader program
	glAttachShader(ProgramId, vertexShaderId);
	glAttachShader(ProgramId, fragmentShaderId);

	glLinkProgram(ProgramId); //Links the shader program
	glUseProgram(ProgramId); //Uses the shader program
}*/


























