#include "main.hpp"

using namespace std;

int main()
{
	loadGL();

	mesh testMesh("model.svx");
	
	atexit(cleanup);

	glm::mat4 model, view, mvp;
	glm::mat4 projection = glm::perspective(60.0f, (float)wind_width/(float)wind_height, 0.1f, 100.0f);
	GLint mvpUniform = glGetUniformLocation(shaderProgram, "mvp");
	
	double time = 0.0;
	const double constdt = 0.01;
	double oldTime = glfwGetTime();
	double dt = 0.0;
	
	while (glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC) && !((glfwGetKey(GLFW_KEY_LCTRL) || glfwGetKey(GLFW_KEY_RCTRL)) && (glfwGetKey('C') || glfwGetKey('W') || glfwGetKey('D'))))
	{
		double newTime = glfwGetTime();
		double frameTime = newTime - oldTime;
		frameTime = min(frameTime, 0.25); // max frame time to avoid spiral of death
		oldTime = newTime;
		dt += frameTime;
		
		while (dt >= constdt)
		{
			// update (time, dt)
			{
				int x, y;
				glfwGetMousePos(&x, &y);
				if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)  { rotx += y - msy; roty += x - msx; }
				if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { rotz += x - msx; rotz += y - msy; }
				msx = x; msy = y;
				
				model = glm::rotate(glm::mat4(1), rotx, glm::vec3(1, 0, 0));
				model = glm::rotate(model,        roty, glm::vec3(0, 1, 0));
				model = glm::rotate(model,        rotz, glm::vec3(0, 0, 1));
				view = glm::lookAt(glm::vec3(0, 0, 4-glfwGetMouseWheel()), glm::vec3(0), glm::vec3(0, 1, 0));

				mvp = projection * view * model;
				
				glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));
			}
			
			time += constdt;
			dt -= constdt;
		}
		
		// draw
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			testMesh.draw(GL_POINTS);

			glfwSwapBuffers();
		}
	}
	
	return 0;
}

bool mesh::loadVoxels(string filename)
{
	ifstream filestream(filename.c_str(), ios::binary);
	if (!filestream.good()) { return false; }
	
	SHTVXL::header header;
	filestream.read((char*)&header, sizeof(header));
	
	if (strncmp(header.magic, "SHTVXL", 6) || header.version != 1)
	{
		cout << "File \"" << filename << "\" is not a valid version 1 SHTVXL model" << endl;
		filestream.close(); return false;
	}
	
	filestream.close();
	return true;
}

void mesh::processVoxels()
{
	vertices.push_back(glm::vec4(1, 0, 0, 1));
	vertices.push_back(glm::vec4(0, 1, 0, 1));
	vertices.push_back(glm::vec4(0, 0, 1, 1));
}

void loadGL()
{
	if (!glfwInit()) { cerr << "Failed to initialize GLFW\n"; exit(1); }
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2); glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1); glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // in case of bragging on the Internet uncomment this line
	if (!glfwOpenWindow(wind_width, wind_height, 0, 0, 0, 0, 24, 8, GLFW_WINDOW)) { cerr << "Failed to open window\n"; exit(1); }
	glfwSetWindowTitle("voxel something");
	glfwSwapInterval(1);
	
	GLenum glewInitStatus = glewInit();
	if (glewInitStatus != GLEW_OK) { cerr << "GLEW failed to initialize. Error string:\n" << glewGetErrorString(glewInitStatus) << endl; exit(1); }
	
	glViewport(0, 0, wind_width, wind_height);
	glEnable(GL_DEPTH_TEST);
	glPointSize(5.0f);
	
	loadShader(GL_VERTEX_SHADER, vertexShader, "shaders/vert.glsl");
	loadShader(GL_FRAGMENT_SHADER, fragmentShader, "shaders/frag.glsl");
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	posAttrib = glGetAttribLocation(shaderProgram, "vposition");
	normAttrib = glGetAttribLocation(shaderProgram, "vnormal");
}

void loadShader(GLenum type, GLuint& shader, string filename)
{
	char compileLog[513];
	ifstream filestream (filename.c_str());
	if (!filestream.good()) { cerr << "Failed to open shader \"" << filename << "\"\n"; exit(1); }

    string str;
    filestream.seekg(0, ios::end);
    str.resize(filestream.tellg());
    filestream.seekg(0, ios::beg);
    filestream.read(&str[0], str.size());
	filestream.close();

	const char* source = str.c_str();
	
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	
	GLint compileSuccess;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
	if (compileSuccess == GL_FALSE) { glGetShaderInfoLog(shader, 512, NULL, compileLog); cerr << "Shader \"" << filename << "\" failed to compile. Error log:\n" << compileLog; glDeleteShader(shader); exit(1); }
}
