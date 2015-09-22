#pragma once
#include <string>
#include <GL/glew.h>

class shader
{
public:
	shader(const std::string& filename);
	~shader();
	GLuint GetProgram();
private:
	GLuint program_;
};

