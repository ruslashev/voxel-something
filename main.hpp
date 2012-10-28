#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/noise.hpp>
#include <cstdio>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>

struct vertex
{
	glm::vec3 position;
	glm::vec3 color;
	vertex(glm::vec3 position, glm::vec3 color) : position(position), color(color) {}
};

struct voxel
{
	bool empty;
	glm::vec3 color;
};

void loadShader(GLenum type, GLuint& shader, const char* filename);

GLuint vao, vbo, vertexShader, fragmentShader, shaderProgram;
GLfloat rotx = 0.0f, roty = 0.0f, rotz = 0.0f;
int msx = 0, msy = 0;

void loadEverythingButOGL();
void loadGL();
bool loadModel(std::string filename, int& w, int& h, int& d);

void cleanup()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDetachShader(shaderProgram, vertexShader);	
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	glfwCloseWindow();
	glfwTerminate();
}
