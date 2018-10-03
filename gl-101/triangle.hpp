#pragma once

#include "loader.h"
#include "shader.h"

const char *vertexSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentSource = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

shader s;
GLuint VAO;

float vertices[] = {
	-0.5, -0.5, 0.0,
	0.5, -0.5, 0.0,
	0,0, 0.5, 0.0
};

void createTriangle()
{
	GLuint VBO;
	
	shader s;

	glClearColor(0.0, 0.5, 1.0, 0.0);
	glViewport(0, 0, 800, 600);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	s.compileShader(vertexSource, 0);
	s.compileShader(fragmentSource, 1);

	s.linkShader();

	//s.use();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void triangleLoop(HDC *hDc)
{
	glClear(GL_COLOR_BUFFER_BIT);
	s.use();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	SwapBuffers(*hDc);
}
