#include "main.hpp"

using namespace std;

int width = 16, height = 16, depth = 16;
voxel* voxels = NULL;
vector<vertex> vertices;

int main()
{
	loadModel("model.vx", width, height, depth);
	
	vertices.reserve(width*height*depth);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int z = 0; z < depth; z++)
			{
				voxel v = voxels[x * height * depth + y * depth + z];
				if (v.empty) continue;
				float s = 1.f;
				glm::vec3 color = v.color;
				vertices.push_back(vertex(glm::vec3(x*s,   y*s+s, z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s,   z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s,   z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s+s, z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s+s, z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s,   z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s,   z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s+s, z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s+s, z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s+s, z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s,   z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s,   z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s+s, z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s+s, z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s,   z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s,   z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s,   z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s,   z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s,   z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s,   z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s+s, z*s-s), color));
				vertices.push_back(vertex(glm::vec3(x*s,   y*s+s, z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s+s, z*s  ), color));
				vertices.push_back(vertex(glm::vec3(x*s+s, y*s+s, z*s-s), color));
			}
		}
	}

	loadGL();


	glm::mat4 model;
	glm::mat4 view = glm::lookAt(glm::vec3(0, 20, 40), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(60.0f, 800.0f / 600.0f, 1.0f, 100.0f);
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
				
				model = glm::rotate(glm::mat4(1.f), rotx, glm::vec3(1, 0, 0));
				model = glm::rotate(model, 			roty, glm::vec3(0, 1, 0));
				model = glm::rotate(model, 			rotz, glm::vec3(0, 0, 1));
				model = glm::translate(model, glm::vec3(-width/2.f, -height/2.f, -depth/2.f));
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
			
			glDrawArrays(GL_QUADS, 0, vertices.size());
			glfwSwapBuffers();
		}
	}
	
	cleanup();
		
	return 0;
}

void loadModel(string filename, int& w, int& h, int& d)
{
	ifstream fs(filename.c_str());
	if (!fs.is_open()) { cout << "Failed to open model file \"" << filename << "\". It probably doesn't exist\n"; exit(1); }
		
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
			// create a new empty model
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
		} else if (line.substr(0, 3) == "vox")
		{			
			string vals = line.substr(line.find("[")+1);
			istringstream s(vals.erase(vals.size()-1, vals.size()));
			int x, y, z;
			int rgbhex;
			s >> x;
			s >> y;
			s >> z;
			s >> hex >> rgbhex;
			
			// to future myself learning about bitmasking:
			// RRGGBB
			// each hex digit takes 4 bits (1111_2 -> F_16), so shifting by 16 bits would leave 0xRR from 0xRRGGBB
			// for green we shift by 8 bits (0xRRGGBB -> 0xRRGG) and bitmasking it to 0xGG
			int i = x * h * d + y * d + z;
			voxels[i].empty = false;
			voxels[i].color = glm::vec3(((rgbhex >> 16) & 0xFF) / 255.f, ((rgbhex >> 8) & 0xFF) / 255.f, (rgbhex & 0xFF) / 255.f);
		} else { /* everything else is not parsed */ }
	}
	fs.close();
}

void loadGL()
{
	if (glfwInit() == GL_FALSE) { cerr << "GLFW failed to initialize\n"; cleanup(); exit(1); }
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2); glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1); glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	//glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 16); // in case of bragging on the Internet uncomment this line
	if (glfwOpenWindow(800, 600, 0, 0, 0, 0, 24, 8, GLFW_WINDOW) == GL_FALSE) { cerr << "Failed to open window\n"; cleanup(); exit(1); }
	glfwSetWindowTitle("Voxel something");
	
	GLenum glewInitStatus = glewInit();
	if (glewInitStatus != GLEW_OK) { cerr << "GLEW failed to initialize. Error string:\n" << glewGetErrorString(glewInitStatus) << endl; cleanup(); exit(1); }
	
	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);
	
	loadShader(GL_VERTEX_SHADER, vertexShader, "shaders/vert.glsl");
	loadShader(GL_FRAGMENT_SHADER, fragmentShader, "shaders/frag.glsl");
	
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	
	GLint posAttrib = glGetAttribLocation(shaderProgram, "vposition");
	GLint colAttrib = glGetAttribLocation(shaderProgram, "vcolor");

	glEnableVertexAttribArray(colAttrib);
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(glm::vec3)));
}


void loadShader(GLenum type, GLuint& shader, const char* filename)
{
	char compileLog[513];
	ifstream fileStream (filename);
	if (!fileStream)
	{ cerr << "Error loading file \"" << filename << "\". It probably doesn't exist\n"; cleanup(); exit(1); }
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
	if (compileSuccess == GL_FALSE) { glGetShaderInfoLog(shader, 512, NULL, compileLog); cerr << "Shader \"" << filename << "\" failed to compile. Error log:\n" << compileLog; glDeleteShader(shader); cleanup(); exit(1); }
}
