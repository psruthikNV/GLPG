#pragma once

#include <stdio.h>
#include "loader.h"

class shader
{
private:
	int vs;
	int fs;
	int pId;

public:

	void compileShader(const char *);
	void linkShader();
	void use();

	shader() : vs(0), fs(0), pId(0) {}

};

void shader::compileShader(const char *path)
{
	FILE *fp = fopen(path, "r");

	if (!fp) {
		OutputDebugString("Shader not found!\n");
	}

}

void shader::linkShader()
{

}

void shader::use()
{

}