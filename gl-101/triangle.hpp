#pragma once

#include "loader.h"
#include "shader.h"
#include "timer.hpp"

timer t;
int vertexColorLocation;

float vertices[] = {
	-0.5, -0.5, 0.0,
	-0.5, 0.5, 0.0,
	0.5, -0.5, 0.0
};

float vertexData[] = {
	-0.5, -0.5, 0.0, 1.0, 0.0, 0.0,
	-0.5, 0.5, 0.0, 0.0, 1.0, 0.0,
	0.5, -1.5, 0.0, 0.0, 0.0, 1.0,
	-0.5, 0.5, 0.0, 0.25, 0.52, 0.0,
	0.5, -0.5, 0.0, 0.72, 0.1, 0.0,
	0.5, 0.5, 0.0, 0.0, 0.0, 0.0
};

const char *vertexSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"//out vec3 aColorOut;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   //aColorOut = aColor;\n"
"}\0";

const char *fragmentSource = 
"#version 330 core\n"
"uniform vec4 aColorOut;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   //vec4 aColorOut4 = vec4(aColorOut.x, aColorOut.y, aColorOut.z, 1.0);\n"
"   FragColor = aColorOut;\n"
"}\n\0";

shader s;
GLuint VAO;
GLuint VBO;

void createTriangle()
{
	s.compileShader(vertexSource, 0);
	s.compileShader(fragmentSource, 1);
	s.linkShader();
	vertexColorLocation = glGetUniformLocation(s.pId, "aColorOut");

	glClearColor(0.0, 0.5, 1.0, 0.0);
	//glViewport(0, 0, 1920, 1080); // Sets the coordinates for NDC to Window Coord conversion
	// By default width and height values are taken from the dimensions of the window.

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 6 * sizeof(float));
	//glVertexArrayVertexBuffer(VAO, 1, VBO, (GLintptr)(3 * sizeof(GL_FLOAT)), 6 * sizeof(float));

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1)
	glBindVertexArray(0);
	
}

void triangleLoop(HDC *hDc)
{
	glClear(GL_COLOR_BUFFER_BIT);
	s.use();
	glBindVertexArray(VAO);
	double timeValue = t.getTime()*0.001*0.001;
	float val = (sin(timeValue) + 1.0f) / 2.0f; printf("val: %f \n", val);
	glUniform4f(vertexColorLocation,val, 0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	SwapBuffers(*hDc);
}