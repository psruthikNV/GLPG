#pragma once

#include "loader.h"
#include "shader.h"
#include "timer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

timer t;
int vertexColorLocation;

float vertices[] = {
	-0.5, -0.5, 0.0,
	-0.5, 0.5, 0.0,
	0.5, -0.5, 0.0
};

float vertexData[] = {
	-0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
	-0.5, 0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
	0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
	-0.5, 0.5, 0.0, 0.25, 0.52, 0.0, 0.0, 1.0, 
	0.5, -0.5, 0.0, 0.72, 0.1, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0
};

const char *vertexSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec3 aColorOut;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	TexCoord = aTexCoord;\n"
"   //aColorOut = aColor;\n"
"}\0";

const char *fragmentSource = 
"#version 330 core\n"
"uniform vec4 aColorOut;\n"
"//in vec3 aColorOut;\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D texSampler;\n"
"void main()\n"
"{\n"
"   //FragColor = vec4(aColorOut.x, aColorOut.y, aColorOut.z, 1.0);\n"
"   FragColor = aColorOut;\n"
"	FragColor = texture(texSampler, TexCoord);\n"
"}\n\0";

shader s;
GLuint VAO;
GLuint VBO;
GLuint texture;

void createTriangle()
{
	int width, height, numChannels;
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 8 * sizeof(float)); // Alternative to glVertexAttribPointer
	glVertexArrayVertexBuffer(VAO, 1, VBO, (GLintptr)(3 * sizeof(GL_FLOAT)), 8 * sizeof(float));
	glVertexArrayVertexBuffer(VAO, 2, VBO, (GLintptr)(6 * sizeof(GL_FLOAT)), 8 * sizeof(float));

	

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0); // By default, all client side capabilities are disabled. We need to enable any shader inputs
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0); // Breaks the existing VAO binding.

	unsigned char *data = stbi_load("test_texture.png", &width, &height, &numChannels, 0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	
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