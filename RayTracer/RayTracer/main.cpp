
#include <GL\glew.h>
#include <iostream>
#include <GLFW\glfw3.h>
#include <Windows.h>
#include "shader.h"
#include "glm\glm.hpp"
#include "camera.h"

GLuint createComputeShader();
GLuint createDrawShader();

void error_callback(int error, const char* description)
{
	fprintf(stderr, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

struct drawMainTexture {
	GLuint drawshader;
	GLuint varr;

	drawMainTexture() {
		GLuint vertexId;
		float arr[16] = {
			-1,1,1,0,
			1,1,1,1,
			-1,-1,0,0,
			1,-1,0,1 };

		drawshader = createDrawShader();

		glGenVertexArrays(1, &varr);
		glBindVertexArray(varr);

		glGenBuffers(1, &vertexId);
		glBindBuffer(GL_ARRAY_BUFFER, vertexId);
		glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), &arr[0], GL_STATIC_DRAW);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexId);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);
	}

	void draw() {
		// Draw texture to screen // do not touch!!!
		glUseProgram(drawshader);
		glBindVertexArray(varr);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glUseProgram(0);
		// Draw ends
	}

private:
	GLuint createDrawShader()
	{
		GLuint shaderProgramHandle = glCreateProgram();
		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		const char* vShaderCode[] = {
			"#version 430\n",
			"layout(location = 0) in vec2 position;\
		layout(location = 1) in vec2 texcoord;\
		out vec2 ftexcoord;\
		void main() {\
			gl_Position = vec4(position,0,1);\
			ftexcoord = texcoord;\
		}"
		};
		GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fShaderCode[] = {
			"#version 430\n",
			"in vec2 ftexcoord; \
		uniform sampler2D Texture;\n\
		out vec4 color;\
		void main() {\
			color = texture(Texture, ftexcoord);\
		\n}"
		};
		glShaderSource(vShader, 2, vShaderCode, NULL);
		glCompileShader(vShader);
		int rvalue;
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &rvalue);
		if (!rvalue) {
			GLint maxLength = 0;
			glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &maxLength);

			GLint logLength = 0;
			GLchar* log = new char[maxLength];
			glGetShaderInfoLog(vShader, maxLength, &logLength, log);
			if (logLength > 0)
				printf("%s\n", log);
			return -1;//error info here
		}
		glShaderSource(fShader, 2, fShaderCode, NULL);
		glCompileShader(fShader);
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &rvalue);
		if (!rvalue) {
			GLint maxLength = 0;
			glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &maxLength);

			GLint logLength = 0;
			GLchar* log = new char[maxLength];
			glGetShaderInfoLog(fShader, maxLength, &logLength, log);
			if (logLength > 0)
				printf("%s\n", log);
			return -1;//error info here
		}
		glAttachShader(shaderProgramHandle, vShader);
		glAttachShader(shaderProgramHandle, fShader);
		glLinkProgram(shaderProgramHandle);
		glGetProgramiv(shaderProgramHandle, GL_LINK_STATUS, &rvalue);
		if (!rvalue) {
			GLint maxLength = 0;
			glGetProgramiv(shaderProgramHandle, GL_INFO_LOG_LENGTH, &maxLength);

			GLint logLength = 0;
			GLchar* log = new char[maxLength];
			glGetShaderInfoLog(shaderProgramHandle, maxLength, &logLength, log);
			if (logLength > 0)
				printf("%s\n", log);
			else
				printf("An error occured while linking shader program but no log was available!");
			return -1;//error info here
		}
		glUseProgram(shaderProgramHandle);

		glUniform1i(glGetUniformLocation(shaderProgramHandle, "Texture"), 0);

		//glDispatchCompute(512 / 16, 512 / 16, 1);
		//checkErrors("Compute shader");
		return shaderProgramHandle;
	}
};

int main() {


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
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	drawMainTexture mainDraw;
	camera cam;

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_FLOAT, 0);
	glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	

	shader iteration_one = shader("iteration_one");
	GLuint computeshader = iteration_one.GetProgram();
	glUseProgram(computeshader);
	glUniform1i(glGetUniformLocation(computeshader, "outputTexture"), 0);


	while (!glfwWindowShouldClose(window))
	{
		cam.computeMatricesFromInputs(window);
		//int width, height;
		//glfwGetFramebufferSize(window, &width, &height);
		//glViewport(0, 0, width, height);
		// Keep running

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(1, 0.5, 0, 0);

		glUseProgram(computeshader);

		int pos = glGetUniformLocation(computeshader, "eye");
		glUniform3f(pos, cam.position.x, cam.position.y, cam.position.z);
		pos = glGetUniformLocation(computeshader, "ray00");
		glm::vec3 ray = cam.GetEyeRay(-1, -1);
		glUniform3f(pos, ray.x, ray.y, ray.z);
		pos = glGetUniformLocation(computeshader, "ray01");
		ray = cam.GetEyeRay(1, -1);
		glUniform3f(pos, ray.x, ray.y, ray.z);
		pos = glGetUniformLocation(computeshader, "ray10");
		ray = cam.GetEyeRay(-1, 1);
		glUniform3f(pos, ray.x, ray.y, ray.z);
		pos = glGetUniformLocation(computeshader, "ray11");
		ray = cam.GetEyeRay(1, 1);
		glUniform3f(pos, ray.x, ray.y, ray.z);


		glDispatchCompute(512 / 16, 512 / 16, 1);
		//checkErrors("Compute shader");

		glMemoryBarrier(GL_ALL_BARRIER_BITS);


		// draw main texture
		mainDraw.draw();
		// end

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

