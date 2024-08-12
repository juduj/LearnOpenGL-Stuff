#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glWin.h"

void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor\n;"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.8f, 0.5f, 1.0f);\n"
"}\0";

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

int main()
{
	glfwInit();
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

	//Keep this here please
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, gl_Win::width, gl_Win::height);
	
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallBack);
	
	//shader object - our vertex shader that we create with a cool little id
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

	//heres where object ids come into place.
	//we need to distinctively know which shader we are compiling so 
	//we use the vertexShader as our object id
	glCompileShader(vertexShader); 

	//this is horrid but for now i will leave it here until code cleanup.
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// END OF VERTEX SHADER END OF VERTEX SHADER END OF VERTEX SHADER END OF VERTEX SHADER END OF VERTEX SHADER END OF VERTEX SHADER

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	

	//Shader Program
	unsigned int shaderProgram = glCreateProgram();
	
	//attaching our shaders to the newly created shaderProgram object
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//basically compiling our shader program
	glLinkProgram(shaderProgram);
	
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}


	//flushing our shaders because we no longer need them.
	//why are we doing this? well its because they are now linked into a shader program object
	//so the gpu is now ready to use what we have all linked together.
	//if we dont delete these shaders off the heap then we well have FREE MEMORY
	//which is bad because that memory is taken up when its no longer being used
	//if this was on the stack(which it should never be) then it would die once its out of the scope. 
	//the reason its on the heap is so its basically able to be used by the entire EXE - PROGRAM ITSELF
	//as I mentioned a stack variable is in a scope. if you think clearly enough you would understand
	//that is this was on the stack then whats the point? 
	//what if i had this shader somewhere else? HOW WOULD I USE IT?
	//  
	//	- jud
	//
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//our trigangle vertices :) so stinkin cute! 
	//*EDIT - rip stinkin cute triangle vertices :(
	float verts[] = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	//The virtual buffer object is the first real occurence of an object
	//in the opengl series, i however am a little confuse but ok nonetheless.
	//the vbo is generated with an ID.
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//copies the previously defined vertex data into the buffer's memory:
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	//telling OpenGL how we should handle the vertex buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//INDEXED DRAWING
	//this is a really good performance method to minimize how many indices you use.
	//in the naked eye you would expect a rectangle to use 6 indices(or vertexs or whatever you call them)
	//3 indices can make 1 triangle so you expect to make 6 for a rectangle.
	//well if you look at the indices there are repeats?
	//whats the most of doing that if we can re-use that same indices to start another triangle?
	//thats where index drawing comes in and EBO
	//ok so create ebo object, bind it to an id = GL_ELEMENT_ARRAY_BUFFER.
	//use the binded ID type GL_ELEMENT_ARRAY_BUFFER to take in the indices using the GL_STATIC_DRAW method
	//ok now 
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//BIND this after you bind all your buffers into the vertex array
	//if you dont this is basically compiling that before 
	//using any of the objects you put in the array
	glBindVertexArray(0);


	//        WIREFRAME POLYGONS!
	//although there are 3 vertices!!! LOL!!!
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		//Input
		ProcessInput(window);

		//This basically just renders our background color
		glClearColor(0.8f, 0.4f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		//Check events and swap buffers[leave these here PLEASE]
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();

	return 0;
}
//this function is basically called when the window is resized 
//this is to help reset the viewport. 
void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
