#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdint>

void onChangeframeBufferSize(GLFWwindow* window, const int32_t width, 
	const int32_t height) {
	glViewport(0, 0, width, height);
}

void handlerInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void render() {
	glClearColor(0.4f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
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
	window = glfwCreateWindow(800, 600, "AG01", NULL, NULL);
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

	while(!glfwWindowShouldClose(window)){	//Loop until user closes window
		handlerInput(window);	//Handle Input
		render();	//Render Here
		glfwSwapBuffers(window);	//Swap front and back buffers
		glfwPollEvents();	//Poll for and process events
	}

	glfwTerminate();	//Close
	return 0;	//Ends OK
}