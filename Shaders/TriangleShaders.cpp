#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glWin.h"
#include "shaders.h"

enum ShaderType
{
	VERT,FRAG
};

void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);
int  LoadGlad();
void CheckShader(unsigned int shader, ShaderType shaderType);
void CheckProgram(unsigned int program);

float changeColorSpeed = 0.0f;

namespace ShaderShape
{
	float verts[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 

	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};
}


int main()
{
	if (!glfwInit())
	{
		std::cout << "Could not initate glfw :(" << std::endl;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(gl_Win::width, gl_Win::height, "Hello", NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW Window!\n";
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallBack);


	LoadGlad();


	glViewport(0, 0, gl_Win::width, gl_Win::height);
		
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &ShaderSources::vertexShaderSource, NULL);
	glCompileShader(vertexShader); 

	CheckShader(vertexShader, VERT);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &ShaderSources::fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	
	CheckShader(fragmentShader, FRAG);

	unsigned int shaderProgram1 = glCreateProgram();

	glAttachShader(shaderProgram1, vertexShader);
	glAttachShader(shaderProgram1, fragmentShader);

	glLinkProgram(shaderProgram1);

	CheckProgram(shaderProgram1);
	

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//first triangle
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//copies the previously defined vertex data into the buffer's memory:
	glBufferData(GL_ARRAY_BUFFER, sizeof(ShaderShape::verts), ShaderShape::verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	


	//        WIREFRAME POLYGONS!
	//although there are 3 vertices!!! LOL!!!
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//our uniform
	//int vertexColorLocation = glGetUniformLocation(shaderProgram2, "ourColor");
	//if (vertexColorLocation == -1) {
	//	std::cout << "Could not find uniform location ! = ourColor\n";
	//}

	//std::cout << "Press K to increase color changing speed!\n";
	//std::cout << "Press J to decrease color changing speed!\n";

	while (!glfwWindowShouldClose(window))
	{
		//Input
		ProcessInput(window);

		//This basically just renders our background color
		glClearColor(0.8f, 0.4f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		
		//float timeValue = glfwGetTime();
		//float greenValue = (sin(timeValue * changeColorSpeed) / 2.0) + 0.5f;



		glUseProgram(shaderProgram1);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3); 

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram1);

	glfwTerminate();

	std::cout << "GLFW Terminated!" << std::endl;

	return 0;
	
}

void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool kPress = false;
bool jPress = false;

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{

		glfwSetWindowShouldClose(window, true);
		std::cout << "GLFW CLOSED BY INPUT!" << std::endl;
		kPress = true;
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		if (!kPress)
		{
			changeColorSpeed++;
			std::cout << "NEW COLOR SPEED: " << changeColorSpeed << std::endl;
			kPress = true;
		}
	}
	else {
		kPress = false;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		if (!jPress)
		{
			changeColorSpeed--;
			std::cout << "NEW COLOR SPEED: " << changeColorSpeed << std::endl;
			jPress = true;
		}
	}
	else {
		jPress = false;
	}
}

int LoadGlad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
}

void CheckShader(unsigned int shader, ShaderType shaderType)
{
	int  success;
	char infoLog[512];

	switch (shaderType)
	{
	case VERT:
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	case FRAG:
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
}

void CheckProgram(unsigned int shaderProgramID)
{
	int  success;
	char infoLog[512];

	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
}
