#include "shader.h"
#include <fstream>

shader::shader(const std::string & filename)
{
	GLuint is;
	std::ifstream in;
	std::string shaderSource;
	in.open(filename + ".glsl", std::ios_base::in);

	if (!in.is_open())
	{
		printf("Couldn't open file at \"%s\"", filename.c_str());
		return;
	}

	shaderSource = std::string();
	shaderSource.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	in.close();

	is = glCreateShader(GL_COMPUTE_SHADER);
	//printf("GL Error: %i\n", glGetError());
	if (is == 0)
	{
		printf("GL Error: %i\n", glGetError());
		printf("glCreateShader failed!");
		return;
	}

	const GLchar* shaderSourceChar = shaderSource.c_str();
	glShaderSource(is, 1, static_cast<const GLchar**>(&shaderSourceChar), 0);
	//printf("GL Error: %i\n", glGetError());
	glCompileShader(is);

	//printf("GL Error: %i\n", glGetError());
	GLint shaderCompiled = GL_FALSE;
	glGetShaderiv(is, GL_COMPILE_STATUS, &shaderCompiled);

	//printf("GL Error: %i\n", glGetError());
	if (shaderCompiled != GL_TRUE)
	{
		printf("GL Error: %i\n", glGetError());
		GLint maxLength = 0;
		glGetShaderiv(is, GL_INFO_LOG_LENGTH, &maxLength);

		GLint logLength = 0;
		GLchar* log = new char[maxLength];
		glGetShaderInfoLog(is, maxLength, &logLength, log);

		if (logLength >= 0)
			printf("%s\n", log);

		delete[] log;
		glDeleteShader(is);
		is = 0;
		return;
	}

	program_ = glCreateProgram();
	if (program_ == 0)
	{
		printf("Couldn't create shader program!");
		//UnloadProgram(contentManager);
		return;
	}

	glAttachShader(program_, is);
	glLinkProgram(program_);
	glUseProgram(program_);

	GLint linkSuccess = GL_TRUE;
	glGetProgramiv(program_, GL_LINK_STATUS, &linkSuccess);
	if (linkSuccess != GL_TRUE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &maxLength);

		GLint logLength = 0;
		GLchar* log = new char[maxLength];
		glGetShaderInfoLog(program_, maxLength, &logLength, log);
		if(logLength > 0)
			printf(log);
		else
			printf("An error occured while linking shader program but no log was available!");

		delete[] log;

		return;
	}
	return;
}

shader::~shader()
{
}

GLuint shader::GetProgram()
{
	return program_;
}
