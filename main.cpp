#include "main.h"

using namespace std;

int width = 16, height = 16, depth = 16;
voxel* voxels = NULL;
vector<vertex> vertices;

int main()
{
	loadEverythingButOGL();
	loadGL();

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection = glm::perspective(60.0f, 800.0f / 600.0f, 1.0f, 500.0f);
	glm::mat4 mvp;
	
	while (glfwGetWindowParam(GLFW_OPENED) && glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS)
	{
		int x, y;
		glfwGetMousePos(&x, &y);
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			rotx += y - msy;
			roty += x - msx;
			
		}
		msx = x; msy = y;
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);		
		
		model = glm::rotate(glm::mat4(1.), rotx, glm::vec3(1., 0., 0.));
		model = glm::rotate(model, roty, glm::vec3(0., 1., 0.));
		model = glm::translate(model, glm::vec3(-width/2.f, -height/2.f, -depth/2.f));
		
		view = glm::lookAt(glm::vec3(0., 20., 40.), glm::vec3(0., 0., 0.), glm::vec3(0., 1., 0.));
		
		mvp = projection * view * model;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
		
		glDrawArrays(GL_POINTS, 0, vertices.size());
	
		glfwSwapBuffers();
	}
	
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	
	glfwTerminate();
	
	return 0;
}

void loadEverythingButOGL()
{
	loadModel("model.vx", width, height, depth);
	
	vertices.reserve(width*height*depth);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int z = 0; z < depth; z++)
			{
				voxel v = voxels[x * height * depth + y * depth + z ];
				if (v.empty) continue;
				vertices.push_back(vertex(x, y, z, v.r, v.g, v.b));
				//pushVoxel(vertices, x, y, z, voxels[x * height * depth + y * width + z ]);
			}
		}
	}
}

void loadModel(string filename, int& w, int& h, int& d)
{
	ifstream fs(filename.c_str());
	if (!fs.is_open()) { cout << "'ders no file called \"" << filename << "\" ya dumass\n"; exit(1); }
		
	string line;
	
	while (getline(fs, line))
	{
		if (line.substr(0, 4) == "size")
		{
			string vals = line.substr(line.find("[")+1);
			istringstream s(vals.erase(vals.size()-1, vals.size()));
			s >> w;
			s >> h;
			s >> d;
			
			voxels = new voxel[w*h*d];
			for (int x = 0; x < w; x++)
			{
				for (int y = 0; y < h; y++)
				{
					for (int z = 0; z < d; z++)
					{
						int i = x * h * d + y * d + z;
						cout << i << " ";
						
						voxels[i].empty = true;
						voxels[i].r = 0;
						voxels[i].g = 0;
						voxels[i].b = 0;
					}
				}
			}
		} else if (line.substr(0, 3) == "vox")
		{
			string vals = line.substr(line.find("[")+1);
			istringstream s(vals.erase(vals.size()-1, vals.size()));
			int x, y, z, r, g, b;
			s >> x;
			s >> y;
			s >> z;
			s >> r;
			s >> g;
			s >> b;
			
			int i = x * h * d + y * d + z;
			voxels[i].empty = false;
			voxels[i].r = r;
			voxels[i].g = g;
			voxels[i].b = b;
		} else { /* everything else is not parsed */ }
	}
	fs.close();
}

/*
void pushVoxel(vector<vertex>& vertices, float x, float y, float z, voxel& v)
{
	if (v.empty) return;
	vertices.push_back(vertex(x, y, z, v.r, v.g, v.b));
}
*/
void loadGL()
{
	glfwInit();
	
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2); // not
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 1); // funny
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	glfwOpenWindow(800, 600, 0, 0, 0, 0, 16, 0, GLFW_WINDOW);
	glfwSetWindowTitle("Voxel something");
	
	glewInit();
	
	glViewport(0, 0, 800, 600);
	glPointSize(2.0f);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);
	
	// Load shaders
	loadShader(GL_VERTEX_SHADER, vertexShader, vertexSource);
	loadShader(GL_FRAGMENT_SHADER, fragmentShader, fragmentSource);
	
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	
	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(colAttrib);
	glEnableVertexAttribArray(posAttrib);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void loadShader(GLenum type, GLuint& shader, const char* source)
{
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
}