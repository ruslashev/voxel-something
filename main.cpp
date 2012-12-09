#include "main.hpp"

using namespace std;

int main()
{
	atexit(cleanup);

	mesh testMesh("model.vx");

	loadGL();

	glm::mat4 model;
	glm::mat4 view = glm::lookAt(glm::vec3(0, 20, 40), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(60.0f, (float)wind_width / wind_height, 1.0f, 100.0f);
	glm::mat4 mvp;
	GLint mvpUniform = glGetUniformLocation(shaderProgram, "mvp");
	
	double time = 0.0;
	const double constdt = 0.01;
	double oldTime = glfwGetTime();
	double dt = 0.0;
	
	bool pause = false;
	
	while (glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC) && !((glfwGetKey(GLFW_KEY_LCTRL) || glfwGetKey(GLFW_KEY_RCTRL)) && (glfwGetKey('C') || glfwGetKey('W') || glfwGetKey('D'))))
	{
		if (glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS) { pause = false; }
		
		double newTime = glfwGetTime();
		double frameTime = newTime - oldTime;
		frameTime = min(frameTime, 0.25); // max frame time to avoid spiral of death
		oldTime = newTime;
		dt += frameTime;
		
		while (dt >= constdt && !pause)
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
			
			testMesh.draw(GL_QUADS);

			glfwSwapBuffers();
		}
	}
			
	return 0;
}

bool mesh::loadModel(string filename)
{
	/*
	ifstream fs(filename.c_str());
	if (!fs.good()) { cout << "Failed to open model file \"" << filename << "\""; return false; }
	
	string line;
	getline(fs, line);
	string vals = line.substr(5);
	istringstream s(vals);
	//int w, h, d;
	// AABBCC
	s >> w >> h >> d;

	voxels = new voxel[w*h*d];
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			for (int z = 0; z < d; z++)
			{
				int i = x * h * d + y * d + z;
				
				voxels[i].empty = true;
				voxels[i].color = glm::vec3(0);
			}
		}
	}

	voxels[1 * h * d + 0 * d + 0].empty = false;
	voxels[1 * h * d + 0 * d + 0].color = glm::vec3(1);

	voxels[0 * h * d + 1 * d + 0].empty = false;
	voxels[0 * h * d + 1 * d + 0].color = glm::vec3(1);

	voxels[0 * h * d + 0 * d + 1].empty = false;
	voxels[0 * h * d + 0 * d + 1].color = glm::vec3(1);

	fs.close();
	return true;
	*/
	cout << "LOADING FILE " << filename << " LOL" << endl;
	return true;
}

void loadGL()
{
	if (!glfwInit()) { cerr << "GLFW failed to initialize\n"; exit(1); }
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2); glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1); glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // in case of bragging on the Internet uncomment this line
	if (!glfwOpenWindow(wind_width, wind_height, 0, 0, 0, 0, 24, 8, GLFW_WINDOW)) { cerr << "Failed to open window\n"; exit(1); }
	glfwSetWindowTitle("voxel something");
	
	GLenum glewInitStatus = glewInit();
	if (glewInitStatus != GLEW_OK) { cerr << "GLEW failed to initialize. Error string:\n" << glewGetErrorString(glewInitStatus) << endl; exit(1); }
	
	glViewport(0, 0, wind_width, wind_height);
	glEnable(GL_DEPTH_TEST);
	
	loadShader(GL_VERTEX_SHADER, vertexShader, "shaders/vert.glsl");
	loadShader(GL_FRAGMENT_SHADER, fragmentShader, "shaders/frag.glsl");
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader); glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram); glUseProgram(shaderProgram);
	posAttrib = glGetAttribLocation(shaderProgram, "vposition");
	normAttrib = glGetAttribLocation(shaderProgram, "vnormal");
	glEnableVertexAttribArray(posAttrib);
	glEnableVertexAttribArray(normAttrib);
}

void loadShader(GLenum type, GLuint& shader, const char* filename)
{
	char compileLog[513];
	ifstream fileStream (filename);
	if (!fileStream)
	{ cerr << "Error loading file \"" << filename << "\". It probably doesn't exist\n"; exit(1); }
	stringstream ss;
	ss << fileStream.rdbuf();
	fileStream.close();

	string sourceS = ss.str();
	const char* source = sourceS.c_str();
		
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	
	GLint compileSuccess;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
	if (compileSuccess == GL_FALSE) { glGetShaderInfoLog(shader, 512, NULL, compileLog); cerr << "Shader \"" << filename << "\" failed to compile. Error log:\n" << compileLog; glDeleteShader(shader); exit(1); }
}
