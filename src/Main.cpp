/*
	This is the SampleDraw application used in the book 3-D Computer Graphics
	by Samuel R. Buss, 1st Edition published 2005.
	The original OpenGL v1 code can be found here: 
	https://mathweb.ucsd.edu/~sbuss/MathCG/OpenGLsoft/SimpleDraw/SimpleDraw.html
	This is working port of the same program to OpenGL v4.6
 */

/*
	MIT License

	Copyright (c) 2024 Michael Starkie

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define A_LENGTH(a) (sizeof(a) / sizeof(*a))
#define ASSERT(x) if (!(x)) __debugbreak();
#define GlCall(x) GlClearError();\
	x;\
	ASSERT(GlLogCall(#x, __FILE__, __LINE__))

static void GlClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool GlLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << "): " << function <<
			", " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

struct ShaderProgramSource {
	std::string VertexSouce;
	std::string FragmentSource;
};

int modes[] = {
	GL_POINTS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_TRIANGLES
};

int location = 0;
int modeIdx = 0;
int curMode = 0;
unsigned int vertex_buffer = 0;
unsigned int idx_buffer = 0;

static void error_callback(int error, const char* description) {
	std::cout << "error = " << error << ", description = " << description << std::endl;
}

static ShaderProgramSource ParseShader(const std::string& filepath) {
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream stream(filepath); // open file
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	std::cout << "VERTEX" << std::endl;
	std::cout << ss[0].str() << std::endl;
	std::cout << "FRAGMENT" << std::endl;
	std::cout << ss[1].str() << std::endl;

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	GlCall(glShaderSource(id, 1, &src, nullptr)); // set the source code in the shader to the 1 string
	GlCall(glCompileShader(id));

	int result;
	GlCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GlCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* infoLog = (char*)alloca(length * sizeof(char));
		GlCall(glGetShaderInfoLog(id, length, &length, infoLog));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << infoLog << std::endl;
		GlCall(glDeleteShader(id));
		return 0;

	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	// Attach shaders and link them to the program
	GlCall(glAttachShader(program, vs));
	GlCall(glAttachShader(program, fs));
	GlCall(glLinkProgram(program));
	GlCall(glValidateProgram(program));

	GlCall(glDeleteShader(vs));
	GlCall(glDeleteShader(fs));

	return program;
}

static void genBuffers() {
	GlCall(glGenBuffers(1, &vertex_buffer));
	GlCall(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
	GlCall(glGenBuffers(1, &idx_buffer));
	GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer));
	GlCall(glEnableVertexAttribArray(0));
}

static void bufferData(float points[], int p_size, unsigned int indxs[], int i_size, unsigned int points_per_vertex) {
	GlCall(glVertexAttribPointer(0, points_per_vertex, GL_FLOAT, GL_FALSE, 0, 0)); // tell GL the vertices start at idx 0 and are 2 floats long.
	GlCall(glBufferData(GL_ARRAY_BUFFER, (p_size * sizeof(float)), points, GL_STATIC_DRAW));
	GlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, (i_size * sizeof(unsigned int)), indxs, GL_STATIC_DRAW)); // initialize the buffer store with data
}

/* Normalize lower left screen coordinate system (0 to 3) to center screen coordinate system (-1 to +1)*/
static float n(float x) {
	float normalized = 2 * (((x - 0) / (3 - 0))) - 1;
	return normalized;
}

static void drawPoints() {
	float points[] = { n(1.0), n(1.0), n(2.0), n(1.0), n(2.0), n(2.0) };
	unsigned int indices[] = { 0, 1, 2 };
	bufferData(points, 6, indices, 3, 2);
	GlCall(glDrawElements(GL_POINTS, 6, GL_UNSIGNED_INT, nullptr)); // GL state machine knows the data to be drawn is in buffer.
}

static void drawLines(int mode) {
	float points[] = { n(0.5), n(1.0), n(2.0), n(2.0), n(1.8), n(2.6), n(0.7), n(2.2), n(1.6), n(1.2), n(1.0), n(0.5) };
	unsigned int indices[] = { 0, 1, 2, 3, 4, 5 };
	bufferData(points, 12, indices, 6, 2);
	GlCall(glDrawElements(mode, 6, GL_UNSIGNED_INT, nullptr)); // GL state machine knows the data to be drawn is in buffer.
}

static void drawTriangles() {
	float p1[] = {
		n(0.3), n(1.0), n(0.5),
		n(2.7), n(0.85), n(0.0),
		n(2.7), n(1.15), n(0.0)
	};
	float p2[] = {
		n(2.53), n(0.71), n(0.5),
		n(1.46), n(2.86), n(0.0),
		n(1.2), n(2.71), n(0.0)
	};
	float p3[] = {
		n(1.667), n(2.79), n(0.5),
		n(0.337), n(0.786), n(0.0),
		n(0.597), n(0.636), n(0.0)
	};
	unsigned int indices[] = { 0, 1, 2 };
	GlCall(glUniform4f(location, 1.0, 0.0, 0.0, 1.0)); // red
	bufferData(p1, 9, indices, 3, 3);
	GlCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr));
	GlCall(glUniform4f(location, 0.0, 1.0, 0.0, 1.0)); // green
	bufferData(p2, 9, indices, 3, 3);
	GlCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr));
	GlCall(glUniform4f(location, 0.0, 0.0, 1.0, 1.0)); // blue
	bufferData(p3, 9, indices, 3, 3);
	GlCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr));
	GlCall(glUniform4f(location, 1.0, 0.0, 0.0, 1.0)); // red
}

/*
 * drawScene() handles the animation and the redrawing of the
 *		graphics window contents.
 */
static void drawScene() {
	switch (curMode) {
		case GL_POINTS:
			drawPoints();
			break;
		case GL_LINES:
		case GL_LINE_STRIP:
		case GL_LINE_LOOP:
			drawLines(curMode);
			break;
		case GL_TRIANGLES:
			drawTriangles();
			break;
	}

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS)
		return;

	switch (key) {
		case GLFW_KEY_SPACE:
			modeIdx = modeIdx % A_LENGTH(modes);
			curMode = modes[modeIdx];
			modeIdx += 1;
			break;

		case GLFW_KEY_ESCAPE:
			std::cout << "Goodbye!" << std::endl;
			glfwSetWindowShouldClose(window, GL_TRUE);
			glfwDestroyWindow(window);
			glfwTerminate();
			exit(EXIT_SUCCESS);
	}
}



int main() {
	GLFWwindow* window;

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwSetErrorCallback(error_callback);

	window = glfwCreateWindow(640, 480, "SimpleDraw", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);  // synchronizes the vsync with the refresh rate of your monitor

	glfwSetKeyCallback(window, key_callback);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error.  GLEW init() not ok." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "OpenGL Vendor : " << glGetString(GL_VENDOR) << std::endl;

	glPointSize(3);
	glLineWidth(3);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points

	/* Compile the Shader source code */
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.VertexSouce, source.FragmentSource);
	GlCall(glUseProgram(shader));

	/* Set the global variable u_Color in the fragment shader to a specific color */
	location = glGetUniformLocation(shader, "u_Color");
	ASSERT(location != -1);
	GlCall(glUniform4f(location, 1.0, 0.0, 0.0, 1.0)); //red

	/* alloc the array and index buffers in the GPU */
	genBuffers();

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "OpenGL Vendor : " << glGetString(GL_VENDOR) << std::endl;

	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* handle user interaction and draw */
		drawScene();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
}
