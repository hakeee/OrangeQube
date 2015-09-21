
#include <GL\glew.h>
#include <iostream>
#include <GLFW\glfw3.h>
#include <Windows.h>

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


	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Poor filtering. Needed !

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_FLOAT, 0);
	glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	// "Bind" the newly created texture : all future texture functions will modify this texture
	//glBindTexture(GL_TEXTURE_2D, textureID);

	GLuint vertexId;
	float arr[16] = {
					-1,1,1,0,
					1,1,1,1,
					-1,-1,0,0,
					1,-1,0,1};

	GLuint drawshader = createDrawShader();

	GLuint varr;
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

	GLuint computeshader = createComputeShader();


	while (!glfwWindowShouldClose(window))
	{
		//int width, height;
		//glfwGetFramebufferSize(window, &width, &height);
		//glViewport(0, 0, width, height);
		// Keep running

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(1, 0.5, 0, 0);

		glUseProgram(computeshader);

		int greenPos = glGetUniformLocation(computeshader, "green");
		float greenVal = 0.9f;
		glUniform1fv(greenPos, 1, &greenVal);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, textureID);
		//glUniform1i(glGetUniformLocation(computeshader, "outputTexture"), 0);


		glDispatchCompute(512 / 16, 512 / 16, 1);
		//checkErrors("Compute shader");

		glMemoryBarrier(GL_ALL_BARRIER_BITS);



		glUseProgram(drawshader);

		
		glBindVertexArray(varr);


		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, textureID);
		//glUniform1i(glGetUniformLocation(drawshader, "Texture"), 0);
		//auto a = glDrawArrays;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

GLuint createComputeShader()
{
	GLuint shaderProgramHandle = glCreateProgram();
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	const char* computeShaderCode[] = {
		"#version 430\n",
		"layout(binding=0, rgba8) uniform image2D outputTexture;\
		uniform float green;\
		layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;\
		void main() {\
			ivec2 outputPos = ivec2(gl_GlobalInvocationID.xy);\
			imageStore(outputTexture, outputPos, vec4(0.9, green, 0.9, 0.0));\
		}"
	};
	glShaderSource(computeShader, 2, computeShaderCode, NULL);
	glCompileShader(computeShader);
	int rvalue;
	glGetShaderiv(computeShader, GL_COMPILE_STATUS, &rvalue);
	if (!rvalue) {
		GLint maxLength = 0;
		glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &maxLength);

		GLint logLength = 0;
		GLchar* log = new char[maxLength];
		glGetShaderInfoLog(computeShader, maxLength, &logLength, log);
		if (logLength > 0)
			printf("%s\n", log);
		return -1;//error info here
	}
	glAttachShader(shaderProgramHandle, computeShader);
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

	glUniform1i(glGetUniformLocation(shaderProgramHandle, "outputTexture"), 0);
	/*int greenPos = glGetUniformLocation(shaderProgramHandle, "green");
	float greenVal = 0.7f;
	glUniform1fv(greenPos, 1, &greenVal);*/
	//glDispatchCompute(512 / 16, 512 / 16, 1);
	//checkErrors("Compute shader");

	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	return shaderProgramHandle;
}

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