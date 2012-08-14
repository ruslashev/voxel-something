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
	glm::mat4 view = glm::lookAt(glm::vec3(0, 20, 40), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(60.0f, 800.0f / 600.0f, 1.0f, 100.0f);
	glm::mat4 mvp;
	GLint mvpUniform = glGetUniformLocation(shaderProgram, "mvp");
	
	// From awesome tutorial at http://gafferongames.com/game-physics/fix-your-timestep/
	double time = 0.0;
	const double constdt = 0.01;
	double oldTime = glfwGetTime();
	double dt = 0.0;
	
	bool pause = false;
	
	while (glfwGetWindowParam(GLFW_OPENED) && glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS)
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
				voxel v = voxels[x * height * depth + y * depth + z];
				if (v.empty) continue;
				pushVoxel(vertices, x, y, z, v.r, v.g, v.b);
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
			// create a new empty model
			for (int x = 0; x < w; x++)
			{
				for (int y = 0; y < h; y++)
				{
					for (int z = 0; z < d; z++)
					{
						int i = x * h * d + y * d + z;
						
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
			int x, y, z;
			int rgbhex;
			float r, g, b;
			s >> x;
			s >> y;
			s >> z;
			s >> hex >> rgbhex;
			
			// RRGGBB
			// each hex digit takes 4 bits (1111_2 -> F_16), so shifting by 16 bits would leave 0xRR from 0xRRGGBB
			// for green we shift by 8 bits (0xRRGGBB -> 0xRRGG) and bitmasking it to 0xGG
			r = (rgbhex >> 16) & 0xFF;
			g = (rgbhex >> 8) & 0xFF;
			b = rgbhex & 0xFF;
			
			int i = x * h * d + y * d + z;
			voxels[i].empty = false;
			voxels[i].r = r / 255.0f;
			voxels[i].g = g / 255.0f;
			voxels[i].b = b / 255.0f;
		} else { /* everything else is not parsed */ }
	}
	fs.close();
}

void pushVoxel(vector<vertex>& vertices, float x, float y, float z, float r, float g, float b)
{
	float s = 1.f;
/*
	y
	^  __________
	| /         /|
	|/         / |
	|---------x  |
	|         |  |
	| this    |  |
	| thingy  |  |
	| first   | /
	|         |/
  --+----------------> x
-z /                                   */
	vertices.push_back(vertex(x*s,   y*s+s, z*s,   r, g, b));
	vertices.push_back(vertex(x*s,   y*s,   z*s,   r, g, b));
	vertices.push_back(vertex(x*s+s, y*s,   z*s,   r, g, b));
	vertices.push_back(vertex(x*s+s, y*s+s, z*s,   r, g, b));
	
	// opposite to that thingy
	vertices.push_back(vertex(x*s,   y*s+s, z*s-s, r, g, b));
	vertices.push_back(vertex(x*s,   y*s,   z*s-s, r, g, b));
	vertices.push_back(vertex(x*s+s, y*s,   z*s-s, r, g, b));
	vertices.push_back(vertex(x*s+s, y*s+s, z*s-s, r, g, b));
	
	// to the left to that thingy
	vertices.push_back(vertex(x*s,   y*s+s, z*s-s, r, g, b));
	vertices.push_back(vertex(x*s,   y*s+s, z*s,   r, g, b));
	vertices.push_back(vertex(x*s,   y*s,   z*s,   r, g, b));
	vertices.push_back(vertex(x*s,   y*s,   z*s-s, r, g, b));
	
	// opposite to the left to that thingy
	vertices.push_back(vertex(x*s+s, y*s+s, z*s-s, r, g, b));
	vertices.push_back(vertex(x*s+s, y*s+s, z*s,   r, g, b));
	vertices.push_back(vertex(x*s+s, y*s,   z*s,   r, g, b));
	vertices.push_back(vertex(x*s+s, y*s,   z*s-s, r, g, b));
	
	// thingy perpendicular to that thingy
	vertices.push_back(vertex(x*s,   y*s,   z*s-s, r, g, b));
	vertices.push_back(vertex(x*s,   y*s,   z*s,   r, g, b));
	vertices.push_back(vertex(x*s+s, y*s,   z*s,   r, g, b));
	vertices.push_back(vertex(x*s+s, y*s,   z*s-s, r, g, b));
	
	// thingy opposite to thingy perpendicular to that thingy
	vertices.push_back(vertex(x*s,   y*s+s, z*s-s, r, g, b));
	vertices.push_back(vertex(x*s,   y*s+s, z*s,   r, g, b));
	vertices.push_back(vertex(x*s+s, y*s+s, z*s,   r, g, b));
	vertices.push_back(vertex(x*s+s, y*s+s, z*s-s, r, g, b));
}

void loadGL()
{
	if (glfwInit() == GL_FALSE) { cerr << "GLFW did boo-boo\n"; cleanup(); exit(1); }
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2); // not
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 1); // funny
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	glfwOpenWindow(800, 600, 0, 0, 0, 0, 24, 0, GLFW_WINDOW);
	glfwSetWindowTitle("Voxel something");
	
	if (glewInit() != GLEW_OK) { cerr << "GLEW did boo-boo\n"; cleanup(); exit(1); }
		
	glViewport(0, 0, 800, 600);
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);
	
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
	glEnableVertexAttribArray(colAttrib);
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3*sizeof(float)));
}

void loadShader(GLenum type, GLuint& shader, const char* source)
{
	char compileLog[513];
	
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	
	GLint compileSuccess;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
	if (compileSuccess == GL_FALSE) { glGetShaderInfoLog(shader, 512, NULL, compileLog); cerr << "Shader compile error. Error log:\n" << compileLog; glDeleteShader(shader); cleanup(); exit(1); }
}