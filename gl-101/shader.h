#pragma once

#include <stdio.h>
#include <string.h>
#include <vector>
#include "loader.h"

class shader
{
private:
	int vs;
	int fs;
	int pId;

	void compileIntShader(const char *);
	void compileIntFShader(const char *);

public:

	void compileShader(const char *, int);
	void linkShader();
	void use();

	shader() : vs(0), fs(0), pId(0) {}

};

void shader::compileShader(const char *source, int flag)
{
	if (!flag) {
		OutputDebugString("\n Vertex Shader");
		vs = glCreateShader(GL_VERTEX_SHADER);
		compileIntShader(source);
	}

	else {
		OutputDebugString("\n Fragment Shader");
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		compileIntFShader(source);
	}
}

void shader::compileIntShader(const char *source)
{
	int success = 0;

	glShaderSource(vs, 1, &source, nullptr);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		OutputDebugString("FAILED IN COMPILING SHADER");
		int	 maxLength = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);
		glDeleteShader(vs);

		std::string s(errorLog.begin(), errorLog.end());
		OutputDebugString(s.c_str());
	}
}

void shader::compileIntFShader(const char *source)
{
	int success = 0;

	glShaderSource(fs, 1, &source, nullptr);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		OutputDebugString("FAILED IN COMPILING SHADER");
		int	 maxLength = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(fs, maxLength, &maxLength, &errorLog[0]);
		glDeleteShader(fs);

		std::string s(errorLog.begin(), errorLog.end());
		OutputDebugString(s.c_str());
	}
}

void shader::linkShader()
{
	int isLinked = 0;
	pId = glCreateProgram();

	glAttachShader(pId, vs);
	glAttachShader(pId, fs);

	glLinkProgram(pId);

	glGetProgramiv(pId, GL_LINK_STATUS, &isLinked);

	if (isLinked == GL_FALSE) {
		OutputDebugString("FAILED IN LINKING SHADER");
		int maxLength = 0;
		glGetProgramiv(pId, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(pId, maxLength, &maxLength, &errorLog[0]);
		glDeleteShader(vs);
		glDeleteShader(fs);

		std::string s(errorLog.begin(), errorLog.end());
		OutputDebugString(s.c_str());
	}
}

void shader::use()
{
	glUseProgram(pId);

	//glDeleteShader(vs);
	//glDeleteShader(fs);
}