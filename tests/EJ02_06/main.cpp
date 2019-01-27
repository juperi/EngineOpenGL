#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdint>

float PI = 3.14159265358979323846;

void onChangeframeBufferSize(GLFWwindow* window, const int32_t width,
	const int32_t height) {
	glViewport(0, 0, width, height);
}

void handlerInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

bool checkShader(uint32_t shader) {	//Check Shader Compilation
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Error Compiling Shader" <<
			std::endl << infoLog << std::endl;
		return false;
	}
	return true;
}

bool checkProgram(uint32_t program) {	//Check Program Link
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Error Linking Program" <<
			std::endl << infoLog << std::endl;
		return false;
	}
	return true;
}

uint32_t createProgram() {	//Upload and compile Shaders
	//Font code
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main() {\n"
		"  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"  FragColor = vec4(0.7, 0.3, 0.0, 1.0);\n"
		"}\0";

	//Create Vertex Shaders in the CPU
	uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);	// Upload Source
	glCompileShader(vertexShader);	//Compile
	checkShader(vertexShader);

	//Create Fragment Shaders in the CPU
	uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);	// Upload Source
	glCompileShader(fragmentShader);	//Compile
	checkShader(fragmentShader);

	//Create a Program to attach Shaders and link
	uint32_t shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkProgram(shaderProgram);

	//Delete Shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Use program
	return shaderProgram;
}

uint32_t createVertexData(uint32_t *VBO, uint32_t *EBO) {	//Create VAO that compiles everything
	//Hexagon
	float vertices[18];
	uint32_t indices[18];

	int pos = 0;
	for (int i = 0; i < 6; ++i) {
		// Create vertex
		vertices[pos] = cos(i * 2 * PI / 6);
		vertices[pos+1] = sin(i * 2 * PI / 6);
		vertices[pos+2] = 0.0f;

		// Create index
		indices[pos] = 0;
		indices[pos + 1] = i + 1;
		indices[pos + 2] = i + 2;
		
		pos += 3;
	}

	//Generate vertex and elements
	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	//Bind Buffers and Upload vertex and elements
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //Unbind EBO AFTER unbinding VAO

	return VAO;
}

void render(uint32_t VAO, uint32_t program) {
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
}

int main(int args, char* argv[]) {
	if (!glfwInit()) {	//Initialize GLFW
		std::cout << "Failed To Initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	//Use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Core Profile

	GLFWwindow* window;	//Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(800, 600, "EJ02_6", NULL, NULL);
	if (!window) {
		std::cout << "Failed To Create GLFW Window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);	//Make the window's context current

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {	//Init GLAD
		std::cout << "Failed To Initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, onChangeframeBufferSize);	//ViewPort Callback

	uint32_t program = createProgram();	//Upload and compile Shaders function
	uint32_t VBO, EBO;
	uint32_t VAO = createVertexData(&VBO, &EBO);	//Create Vertex Array Object that compiles everything

	glClearColor(0.4f, 0.7f, 0.7f, 1.0f);	//Clear befor entering main loop

	while (!glfwWindowShouldClose(window)) {	//Loop until user closes window
		handlerInput(window);	//Handle Input
		render(VAO, program);	//Paint
		glfwSwapBuffers(window);	//Swap front and back buffers
		glfwPollEvents();	//Poll for and process events
	}

	//Clean everything
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(program);

	glfwTerminate();	//Close
	return 0;	//Ends OK
}