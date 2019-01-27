#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdint>

//Two triangles
float vertices1[] = {	//First triangle
	-1.0f, -0.5f, 0.0f,
	0.0f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f
};
float vertices2[] = {	//Second triangle
	0.0f, -0.5f, 0.0f,
	1.0f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f
};

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

uint32_t createVertexData(uint32_t *VBO, float vertices[], int sizeofV) {	//Create VAO that compiles everything
	//Generate vertex and elements
	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	//Bind Buffers and Upload vertex
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeofV, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void render(uint32_t VAO1, uint32_t VAO2, uint32_t program) {
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glBindVertexArray(VAO1);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glUseProgram(program);
	glBindVertexArray(VAO2);
	glDrawArrays(GL_TRIANGLES, 0, 3);
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
	window = glfwCreateWindow(800, 600, "EJ02_2", NULL, NULL);
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
	
	uint32_t VBO1;
	uint32_t sizeofV1 = sizeof(vertices1);
	uint32_t VAO1 = createVertexData(&VBO1, vertices1, sizeofV1);	//3rd parametre allows us to have real sizeof. Otherwise, it takes reference in the method call

	uint32_t VBO2;
	uint32_t sizeofV2 = sizeof(vertices2);
	uint32_t VAO2 = createVertexData(&VBO2, vertices2, sizeofV2);

	glClearColor(0.4f, 0.7f, 0.7f, 1.0f);	//Clear before entering main loop

	while (!glfwWindowShouldClose(window)) {	//Loop until user closes window
		handlerInput(window);		//Handle Input
		render(VAO1, VAO2, program);//Paint
		glfwSwapBuffers(window);	//Swap front and back buffers
		glfwPollEvents();			//Poll for and process events
	}

	//Clean everything
	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);

	glDeleteProgram(program);

	glfwTerminate();	//Close
	return 0;	//Ends OK
}