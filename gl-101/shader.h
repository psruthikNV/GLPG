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

	void compileIntShader(const char *, int);

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
		compileIntShader(source, vs);
	}

	else {
		OutputDebugString("\n Fragment Shader");
		fs = glCreateShader(GL_FRAGMENT_SHADER);
		compileIntShader(source, fs);
	}
}

void shader::compileIntShader(const char *source, int obj)
{
	int success = 0;

	glShaderSource(obj, 1, &source, nullptr);
	glCompileShader(obj);

	glGetShaderiv(obj, GL_COMPILE_STATUS, &success);

	if (!success) {
		OutputDebugString("FAILED IN COMPILING SHADER");
		int maxLength = 0;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(obj, maxLength, &maxLength, &errorLog[0]);
		glDeleteShader(obj);

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

	if (!isLinked) {
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