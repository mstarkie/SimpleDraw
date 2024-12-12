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

/*
float three_points[] = {
	-0.5f, -0.5f,
	 0.0f,  0.5f,
	 0.5f, -0.5f
};

unsigned int indices[] = {
	0, 1, 2
};
*/

int modeIdx = 0;
int curMode = 0;
//float* points = three_points;

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

static void genBuffer(float points[], int p_size, unsigned int indxs[], int i_size, unsigned int vbuf, unsigned int ibuf, unsigned int points_per_vertex) {
	GlCall(glGenBuffers(1, &vbuf));
	GlCall(glBindBuffer(GL_ARRAY_BUFFER, vbuf));
	GlCall(glBufferData(GL_ARRAY_BUFFER, (p_size * sizeof(float)), points, GL_STATIC_DRAW));
	GlCall(glEnableVertexAttribArray(0));
	GlCall(glVertexAttribPointer(0, points_per_vertex, GL_FLOAT, GL_FALSE, 0, 0)); // tell GL the vertices start at idx 0 and are 2 floats long.
	GlCall(glGenBuffers(1, &ibuf));
	GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf));
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
	unsigned int vertex_buffer = 0;
	unsigned int idx_buffer = 0;
	genBuffer(points, 6, indices, 3, vertex_buffer, idx_buffer, 2);
	GlCall(glDrawElements(GL_POINTS, 6, GL_UNSIGNED_INT, nullptr)); // GL state machine knows the data to be drawn is in buffer.
	GlCall(glDeleteBuffers(1, &vertex_buffer));
	GlCall(glDeleteBuffers(1, &idx_buffer));
}

static void drawLines(int mode) {
	float points[] = { n(0.5), n(1.0), n(2.0), n(2.0), n(1.8), n(2.6), n(0.7), n(2.2), n(1.6), n(1.2), n(1.0), n(0.5) };
	unsigned int indices[] = { 0, 1, 2, 3, 4, 5 };
	unsigned int vertex_buffer = 0;
	unsigned int idx_buffer = 0;
	genBuffer(points, 12, indices, 6, vertex_buffer, idx_buffer, 2);
	GlCall(glDrawElements(mode, 6, GL_UNSIGNED_INT, nullptr)); // GL state machine knows the data to be drawn is in buffer.
	GlCall(glDeleteBuffers(1, &vertex_buffer));
	GlCall(glDeleteBuffers(1, &idx_buffer));
}

static void drawTriangles() {
	float points[] = {
		n(0.3), n(1.0), n(0.5),
		n(2.7), n(0.85), n(0.0),
		n(2.7), n(1.15), n(0.0),

		n(2.53), n(0.71), n(0.5),
		n(1.46), n(2.86), n(0.0),
		n(1.2), n(2.71), n(0.0),

		n(1.667), n(2.79), n(0.5),
		n(0.337), n(0.786), n(0.0),
		n(0.597), n(0.636), n(0.0)
	};
	unsigned int indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	unsigned int vertex_buffer = 0;
	unsigned int idx_buffer = 0;
	genBuffer(points, 27, indices, 9, vertex_buffer, idx_buffer, 3);
	GlCall(glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, nullptr)); // GL state machine knows the data to be drawn is in buffer.
	GlCall(glDeleteBuffers(1, &vertex_buffer));
	GlCall(glDeleteBuffers(1, &idx_buffer));
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
	int location = glGetUniformLocation(shader, "u_Color");
	ASSERT(location != -1);
	GlCall(glUniform4f(location, 1.0, 0.0, 0.0, 1.0)); //red

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "OpenGL Vendor : " << glGetString(GL_VENDOR) << std::endl;

	while (!glfwWindowShouldClose(window)) {
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		drawScene();
		//std::cout << "poll" << std::endl;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
}
