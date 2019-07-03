#pragma once

#include "loader.h"
#include "shader.h"
#include "timer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "utils/math/template_math_ops.hpp"

timer t;
int vertexColorLocation;
int modelMatrixLocation;
mat4x4_f modelMatrix;
vec<3, float> translateVector;

float vertices[] = {
	-0.5, -0.5, 0.0,
	-0.5, 0.5, 0.0,
	0.5, -0.5, 0.0
};

unsigned int indices[] = {
	0, 1, 3,
	1, 2, 3
};
/*
float vertexData[] = {
	-0.5, 0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
	-0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
	0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
	-0.5, 0.5, 0.0, 0.25, 0.52, 0.0, 0.0, 1.0, 
	0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0,
	0.5, -0.5, 0.0, 0.0, 0.1, 0.0, 1.0, 0.0
};*/

float vertexData[] = {
	0.0, 0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
	0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
	0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0,
	0.0, 0.5, 0.0, 0.25, 0.52, 0.0, 0.0, 1.0,
	0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0,
	0.5, 0.0, 0.0, 0.0, 0.1, 0.0, 1.0, 0.0
};

float testData[] = {
	-0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
	-0.5, -0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.5, -0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
};
const char *vertexSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"//aPos.x = aPos.x + 1.0;\n"
"uniform mat4 modelMatrix;\n"
"out vec3 aColorOut;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = modelMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   //gl_Position = vec4(aPos.x - 0.5, aPos.y, aPos.z, 1.0);"
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
"   //FragColor = aColorOut;\n"
"	FragColor = texture(texSampler, TexCoord);\n"
"}\n\0";

shader s;
GLuint VAO;
GLuint VBO;
GLuint EBO;
GLuint texture;

void createTriangle()
{
	int width, height, numChannels;
	s.compileShader(vertexSource, 0);
	s.compileShader(fragmentSource, 1);
	s.linkShader();
	vertexColorLocation = glGetUniformLocation(s.pId, "aColorOut");
	modelMatrixLocation = glGetUniformLocation(s.pId, "modelMatrix");

	glClearColor(0.0, 0.5, 1.0, 0.0);
	//glViewport(0, 0, 1920, 1080); // Sets the coordinates for NDC to Window Coord conversion
	// By default width and height values are taken from the dimensions of the window.

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	//glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 8 * sizeof(float)); // Alternative to glVertexAttribPointer. Okay this isn't the case.
	//glVertexArrayVertexBuffer(VAO, 1, VBO, (GLintptr)(3 * sizeof(GL_FLOAT)), 8 * sizeof(float));
	//glVertexArrayVertexBuffer(VAO, 2, VBO, (GLintptr)(6 * sizeof(GL_FLOAT)), 8 * sizeof(float));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0); // By default, all client side capabilities are disabled. We need to enable any shader inputs
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0); // Breaks the existing VAO binding.

	unsigned char *data = stbi_load("brick.jpg", &width, &height, &numChannels, 0);
	if (!data) {
		printf("\n Failed to load Texture");
		return;
	}
	printf("\n Num Channels : %i", numChannels);
	printf("\n Width : %i", width);
	printf("\n Height : %i", height);
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D); // Looks like we have to call this function always to get some tex output??
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
	std::cout << "Model Matrix : " << modelMatrix << std::endl;
	translateVector[0] = 0.5f;
	//translateVector[1] = 1.0f;
	modelMatrix = translate(modelMatrix, translateVector);
	std::cout << "Model Matrix : " << modelMatrix << std::endl;
}

void triangleLoop(HDC *hDc)
{
	const GLfloat vals[16] = { 1, 0, 0, -0.5,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };
	GLenum err;
	translateVector[0] = 0.5f;
	glClear(GL_COLOR_BUFFER_BIT);
	s.use();
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	double timeValue = t.getTime()*0.001*0.001;
	float val = (sin(timeValue) + 1.0f) / 2.0f;
	//printf("val: %f \n", val);
	std::cout << "Model matrix values: " << modelMatrix << std::endl;
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, modelMatrix.data());
	//glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, vals);
	glUniform4f(vertexColorLocation,val, 0.0f, 0.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_TRUE, vals);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	SwapBuffers(*hDc);
	glBindVertexArray(0);
}