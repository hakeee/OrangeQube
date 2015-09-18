
#include <GL\glew.h>
#include <iostream>
#include <GLFW\glfw3.h>
#include <Windows.h>

void error_callback(int error, const char* description)
{
	fprintf(stderr, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main() {

	if (!glewInit())
		exit(EXIT_FAILURE);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwSetErrorCallback(error_callback);

	GLFWwindow* window = glfwCreateWindow(640, 480, "OrangeQube", NULL, NULL);

	glfwSetKeyCallback(window, key_callback);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window))
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		// Keep running

		glClearColor(1, 0.5, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}