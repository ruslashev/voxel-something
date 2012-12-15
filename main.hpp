#ifndef main_hpp
#define main_hpp

const int wind_width = 800;
const int wind_height = 600;

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
#include <math.h>

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;

void loadShader(GLenum type, GLuint& shader, std::string filename);

GLuint vertexShader, fragmentShader, shaderProgram;
GLint posAttrib, normAttrib;

GLfloat rotx = 0.0f, roty = 0.0f, rotz = 0.0f;
int msx = 0, msy = 0;

struct voxel
{
	bool empty;
	glm::vec3 color;
};

namespace SHTVXL
{
	struct header
	{
		char magic[6];
		byte version; 
	};
}

class mesh
{
private:
	GLuint vbo_vertices, vbo_normals, ibo_indices;
public:
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<ushort> indices;
	std::vector<glm::vec2> texCoords;

	mesh(std::string filename) : vbo_vertices(0), vbo_normals(0), ibo_indices(0) {
		if (!loadVoxels(filename))
		{
			std::cerr << "Failed to load model \"" << filename << "\"\n";
			exit(1);
		}
		processVoxels();
		
		upload();
	}
	~mesh()
	{
		if (vbo_vertices != 0) { glDeleteBuffers(1, &vbo_vertices); }
		if (vbo_normals != 0) { glDeleteBuffers(1, &vbo_normals); }
		if (ibo_indices != 0) { glDeleteBuffers(1, &ibo_indices); }
	}

	bool loadVoxels(std::string filename);

	void processVoxels();

	void upload()
	{
		if (this->vertices.size() > 0)
		{
			glGenBuffers(1, &this->vbo_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
			glBufferData(GL_ARRAY_BUFFER, this->vertices.size()*sizeof(this->vertices[0]), this->vertices.data(), GL_STATIC_DRAW);
		}
		
		if (this->normals.size() > 0)
		{
			glGenBuffers(1, &this->vbo_normals);
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
			glBufferData(GL_ARRAY_BUFFER, this->normals.size()*sizeof(this->normals[0]), this->normals.data(), GL_STATIC_DRAW);
		}
		
		if (this->indices.size() > 0)
		{
			glGenBuffers(1, &this->ibo_indices);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_indices);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(this->indices[0]), this->indices.data(), GL_STATIC_DRAW);
		}
	}

	void draw(GLenum drawMode)
	{
		if (this->vbo_vertices != 0)
		{
			glEnableVertexAttribArray(posAttrib);
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
			glVertexAttribPointer(posAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);
		}

		if (this->vbo_normals != 0)
		{
			glEnableVertexAttribArray(normAttrib);
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
			glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		
		if (this->ibo_indices != 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_indices);
			int size;
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			glDrawElements(drawMode, size/sizeof(ushort), GL_UNSIGNED_SHORT, 0);
		} else {
			glDrawArrays(drawMode, 0, this->vertices.size());
		}

		if (this->vbo_normals != 0) { glDisableVertexAttribArray(normAttrib); }
		if (this->vbo_vertices != 0) { glDisableVertexAttribArray(posAttrib); }
	}
};

void loadGL();

void cleanup()
{
	glDetachShader(shaderProgram, vertexShader);	
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	glfwCloseWindow();
	glfwTerminate();
}

#endif