#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

struct vertex
{
	float x, y, z;
	float r, g, b;
	vertex(float x, float y, float z, float r, float g, float b) : x(x), y(y), z(z), r(r), g(g), b(b) {}
};

struct voxel
{
	bool empty;
	float r;
	float g;
	float b;
};

void pushVoxel(std::vector<vertex>& vertices, float x, float y, float z, float r, float g, float b);
void loadShader(GLenum type, GLuint& shader, const char* source);

const char* vertexSource = "#version 120\n\nuniform mat4 mvp;\n\nattribute vec3 position;\nattribute vec3 color;\n\nvarying vec3 fcolor;\n\nvoid main()\n{\nfcolor = color;\ngl_Position = mvp * vec4(position, 1.0);\n}";
const char* fragmentSource = "#version 120\n\nvarying vec3 fcolor;\n;\n\nvoid main()\n{\ngl_FragColor = vec4(fcolor, 1.0);\n}";

GLuint vao, vbo, vertexShader, fragmentShader, shaderProgram;
GLfloat rotx = 0.0f, roty = 0.0f, rotz = 0.0f;
int msx = 0, msy = 0;

void loadEverythingButOGL();
void loadGL();
void loadModel(std::string filename, int& w, int& h, int& d);

void cleanup()
{
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	
	glfwTerminate();
}