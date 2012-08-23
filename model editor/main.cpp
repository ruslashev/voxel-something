#include "main.h"

using namespace std;

int width = 16, height = 16, depth = 16;
int posx = 0, posy = 0, posz = 0;
voxel* voxels = NULL;
vector<vertex> vertices;

#define curvox posx * height * depth + posy * depth + posz

int main()
{
	// cout << "Enter width, height and depth of the model: \n";
	// cin >> width;
	// cin >> height;
	// cin >> depth;
	voxels = new voxel[width*height*depth];
	
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int z = 0; z < depth; z++)
			{
				int i = x * height * depth + y * depth + z;
				
				voxels[i].empty = true;
				voxels[i].r = (x * 255.f) / width / 255.f;
				voxels[i].g = (y * 255.f) / height/ 255.f;
				voxels[i].b = (z * 255.f) / depth / 255.f;
			}
		}
	}
	
	openWindowNotherSimilarStuffLikeGlewAndSoOn();
	updateVoxels();
	
	glm::mat4 model;
	glm::mat4 view = glm::lookAt(glm::vec3(0, 20, 40), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projection = glm::perspective(60.0f, 800.0f / 600.0f, 1.0f, 100.0f);
	glm::mat4 mvp;
	GLint mvpUniform = glGetUniformLocation(shaderProgram, "mvp");
		
	while (glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC))
	{
		// update
		{
			int x, y;
			glfwGetMousePos(&x, &y);
			if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))  { rotx += y - msy; roty += x - msx; }
			if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) { rotz += x - msx; rotz += y - msy; }
			msx = x; msy = y;
			
			model = glm::rotate(glm::mat4(1.f), rotx, glm::vec3(1, 0, 0));
			model = glm::rotate(model, 			roty, glm::vec3(0, 0, 1));
			model = glm::rotate(model, 			rotz, glm::vec3(0, 1, 0));
			model = glm::translate(model, glm::vec3(-width/2.f, -height/2.f, -depth/2.f));
			mvp = projection * view * model;
			glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));
		}
		
		// draw
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glDrawArrays(GL_QUADS, 0, vertices.size());
			
			// I'm terribly sorry
			
				glBegin(GL_LINES);

				glColor3f(1.0f, 1.0f, 1.0f);
				glVertex3f(-100.0f, posy+s/2, posz-s/2);
				glVertex3f( 100.0f, posy+s/2, posz-s/2);
				
				glVertex3f(posx+s/2, -100.0f, posz-s/2);
				glVertex3f(posx+s/2,  100.0f, posz-s/2);
				
				glVertex3f(posx+s/2, posy+s/2, -100.0f);
				glVertex3f(posx+s/2, posy+s/2,  100.0f);

				glEnd();
			
			// seriously, the last time
			
			glfwSwapBuffers();
		}
	}
	
	cleanup();
	
	return 0;
}

void GLFWCALL onkeysomething(int key, int action)
{
	if (action)
	{
		if (key == 'W')
		{ if (posz > 0) { posz--; }
		} else if (key == 'S')
		{ if (posz < depth - 2) { posz++; } }
		
		if (key == 'A')
		{ if (posx > 0) { posx--; }
		} else if (key == 'D')
		{ if (posx < width - 2) { posx++; } }
		
		if (key == GLFW_KEY_UP)
		{ if (posy < height - 2) { posy++; }
		} else if (key == GLFW_KEY_DOWN)
		{ if (posy > 0) { posy--; } }
		
		if (key == GLFW_KEY_SPACE)
		{ voxels[curvox].empty = !voxels[curvox].empty; updateVoxels(); }
	}
}

void updateVoxels()
{
	vertices.erase(vertices.begin(), vertices.end());
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
	
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(colAttrib);
	glEnableVertexAttribArray(posAttrib);
}

void pushVoxel(vector<vertex>& vertices, float x, float y, float z, float r, float g, float b)
{	
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
		
		// vertices.push_back(vertex(x*s,   y*s+s, z*s-s, r, g, b));
		// vertices.push_back(vertex(x*s+s, y*s+s, z*s-s, r, g, b));
		// vertices.push_back(vertex(x*s+s, y*s,   z*s-s, r, g, b));
		// vertices.push_back(vertex(x*s,   y*s,   z*s-s, r, g, b));
		
		// vertices.push_back(vertex(x*s,   y*s+s, z*s+s, r, g, b));
		// vertices.push_back(vertex(x*s+s, y*s+s, z*s+s, r, g, b));
		// vertices.push_back(vertex(x*s+s, y*s,   z*s+s, r, g, b));
		// vertices.push_back(vertex(x*s,   y*s,   z*s+s, r, g, b));
}

void openWindowNotherSimilarStuffLikeGlewAndSoOn()
{
	// I'm doing that because initializing GLEW and opening window everytime you place block is kind of awkward
	
	if (glfwInit() == GL_FALSE) { cerr << "GLFW did boo-boo\n"; cleanup(); exit(1); }
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 1);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	glfwOpenWindow(800, 600, 0, 0, 0, 0, 24, 0, GLFW_WINDOW);
	glfwSetWindowTitle("Voxel something model editor");
	glfwSetKeyCallback(onkeysomething);
	
	if (glewInit() != GLEW_OK) { cerr << "GLEW did boo-boo\n"; cleanup(); exit(1); }
		
	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
	loadShader(GL_VERTEX_SHADER, vertexShader, "#version 120\n\nuniform mat4 mvp;\n\nattribute vec3 position;\nattribute vec3 color;\n\nvarying vec3 fcolor;\n\nvoid main()\n{\nfcolor = color;\ngl_Position = mvp * vec4(position, 1.0);\n}");
	loadShader(GL_FRAGMENT_SHADER, fragmentShader, "#version 120\n\nvarying vec3 fcolor;\n;\n\nvoid main()\n{\ngl_FragColor = vec4(fcolor, 1.0);\n}");
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);
	
	posAttrib = glGetAttribLocation(shaderProgram, "position");
	colAttrib = glGetAttribLocation(shaderProgram, "color");
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