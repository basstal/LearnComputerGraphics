#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

float vertices1[] = {
	-0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
};

float vertices2[] = {
	-0.5f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
};

float verticesIBO[] = {
	// 位置					颜色				纹理坐标
	0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f, 	1.f, 1.f,
	0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.f, .0f,
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	.0f, .0f,
	-0.5f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,  	.0f, 1.f,
	// 0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f
};


float verticesFromBook[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};


glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f), 
  glm::vec3(-1.5f, -2.2f, -2.5f),  
  glm::vec3(-3.8f, -2.0f, -12.3f),  
  glm::vec3( 2.4f, -0.4f, -3.5f),  
  glm::vec3(-1.7f,  3.0f, -7.5f),  
  glm::vec3( 1.3f, -2.0f, -2.5f),  
  glm::vec3( 1.5f,  2.0f, -2.5f), 
  glm::vec3( 1.5f,  0.2f, -1.5f), 
  glm::vec3(-1.3f,  1.0f, -1.5f)  
};

unsigned int indices[] = {
	0, 1, 2,
	0, 2, 3,
};

float transparent = 0.2f;
float fov = .0f;
float aspect_ratio[] ={
	(float)800/600, (float)1920/1080, (float)1024/768, (float)1600/900, (float)1342/784,
};
int aspect_ratio_count = 5;

float viewX = .0f;
float viewY = .0f;
float viewZ = -3.0f;
char currentSelection = '\0';

void viewChange(GLFWwindow * window, float step);
void frame_buffer_viewport(GLFWwindow *, int, int);
void processInput(GLFWwindow *);
unsigned int loadImage(const char * fileName, GLint format, bool, GLint);
// void ShaderCode(const char *fileName, std::string &shaderCode);
// void CheckCompileShader(unsigned int vertexShader);
// void CheckLinkProgram(unsigned int shaderProgram);
// unsigned int ProcessShader(const char * vert, const char * frag);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "TEST OPENGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "glfwCreateWindow failed!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "glad load failed!" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, frame_buffer_viewport);

	Shader shaderProgram = Shader("VertexShader.glsl", "FragmentShader.glsl");
	// unsigned int shaderProgram = ProcessShader("VertexShader.glsl", "FragmentShader.glsl");
	// unsigned int shaderProgram1 = ProcessShader("VertexShader.glsl", "FragmentShader1.glsl");

	

	// unsigned int VBO1;
	// glGenBuffers(1, &VBO1);
	// glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	// glEnableVertexAttribArray(0);


	// unsigned int VAO2;
	// glGenVertexArrays(1, &VAO2);

	// glBindVertexArray(VAO2);

	// unsigned int VBO2;
	// glGenBuffers(1, &VBO2);
	// glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	// glEnableVertexAttribArray(0);

	// unsigned int VAO3;
	// glGenVertexArrays(1, &VAO3);
	// glBindVertexArray(VAO3);
	// glBindBuffer(GL_ARRAY_BUFFER, VAO3);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(verticesIBO), verticesIBO, GL_STATIC_DRAW);
	
	unsigned int texture1, texture2;
	texture1 = loadImage("container.jpg", GL_RGB, false, GL_REPEAT);
	texture2 = loadImage("awesomeface.png", GL_RGBA, true, GL_REPEAT);

	// VBO 顶点缓冲对象 基础的数据，不能直接在core mode 中绘制；
	// VAO 顶点数组对象 对应到顶点缓冲对象 可以绘制
	// EBO 元素缓冲对象（或称为IBO索引缓冲对象） 对应到VBO中数据的索引 可以绘制
	// unsigned int VAO, VBO, EBO;
	// glGenVertexArrays(1, &VAO);

	// glGenBuffers(1, &VBO);
	// glGenBuffers(1, &EBO);

	// glBindVertexArray(VAO);

	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(verticesIBO), verticesIBO, GL_STATIC_DRAW);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	// glEnableVertexAttribArray(1);
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	// glEnableVertexAttribArray(2);

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFromBook), verticesFromBook, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	shaderProgram.use();
	shaderProgram.setInt("texture1", 0);
	shaderProgram.setInt("texture2", 1);
	shaderProgram.setFloat("transparent", transparent);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	// glm::mat4 model;
	// model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.f, 0.f));

	// glm::mat4 view;
	// view = glm::translate(view, glm::vec3(0.0f, 0.f, -3.0f));

	// glm::mat4 projection;
	// projection = glm::perspective(glm::radians(90.0f), (float) 800/600, 0.1f, 100.0f);

	unsigned int location;
	// location = glGetUniformLocation(shaderProgram.ID, "model");
	// glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));

	// location = glGetUniformLocation(shaderProgram.ID, "view");
	// glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
	
	// location = glGetUniformLocation(shaderProgram.ID, "projection");
	// glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);
	int i = 0;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// glUseProgram(shaderProgram);
		shaderProgram.use();
		shaderProgram.setFloat("transparent", transparent);

		// glm::mat4 trans;
		// trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		// trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

		// int transformLoc = glGetUniformLocation(shaderProgram.ID, "transform");
		// glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		//glBindVertexArray(VAO2);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);

		// 这里按时间设置shader中uniform变量 改变颜色值
		// float time = glfwGetTime();
		// float greenValue = (sin(time) / 2.0f) + 0.5f;
		// int vertexColorLocation = glGetUniformLocation(shaderProgram.ID, "ourColor");
		// glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		// glBindVertexArray(VAO3);
		// glBindVertexArray(0);

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		

		// glBindVertexArray(VAO);
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0);

		// glm::mat4 newTrans;
		// newTrans = glm::translate(newTrans, glm::vec3(-0.5f, 0.5f, 0.0f));
		// newTrans = glm::scale(newTrans, glm::vec3(std::abs(std::sin((float)glfwGetTime())), std::abs(std::sin((float)glfwGetTime())), 0.0f));

		// glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(newTrans));

		// glBindVertexArray(VAO);
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0);

		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		{
			i = (i + 1) % aspect_ratio_count;
			std::cout<< "aspect_ratio => " << aspect_ratio[i] << " index => " << i << std::endl;
		}
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		{
			i = i > 0? i - 1: 4;
			std::cout<< "aspect_ratio => " << aspect_ratio[i] << " index => " << i << std::endl;
		}
		std::cout << "fov => " << fov  << " view x y z => (" << viewX << ", " << viewY << ", " << viewZ << ")"<< std::endl;

		glm::mat4 view;
		view = glm::translate(view, glm::vec3(viewX, viewY, viewZ));

		location = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), aspect_ratio[i], 0.1f, 100.0f);
		
		location = glGetUniformLocation(shaderProgram.ID, "projection");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));

		// glBindVertexArray(VAO);
		// glDrawArrays(GL_TRIANGLES, 0, 36);
		// glBindVertexArray(0);

		glBindVertexArray(VAO);
		for ( int i = 1 ; i <= 10 ; ++ i )
		{
			glm::mat4 newModel;
			newModel = glm::translate(newModel, cubePositions[i]);
			if (i % 3 == 0)
				newModel = glm::rotate(newModel, (float)glfwGetTime() * glm::radians(50.f) , glm::vec3(1.0f, 0.3f, .5f));

			location = glGetUniformLocation(shaderProgram.ID, "newModel");
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(newModel));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// glm::mat4 rotateByTime;
		// rotateByTime = glm::rotate(rotateByTime, (float)glfwGetTime() * glm::radians(55.0f), glm::vec3(1.0f, 0.5f, 0.0f));

		// location = glGetUniformLocation(shaderProgram.ID, "rotateByTime");
		// glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(rotateByTime));

		

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void frame_buffer_viewport(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		// transparent += 0.1f;
		fov += 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		// transparent -= 0.1f;
		fov -= 1.f;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		viewChange(window, 0.1f);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		viewChange(window, -0.1f);
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		currentSelection = 'x';
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		currentSelection = 'y';
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		currentSelection = 'z';
	}
	
}

void viewChange(GLFWwindow * window, float step)
{
	switch(currentSelection)
	{
		case 'x': viewX += step;
				break;
		case 'y': viewY += step;
				break;
		case 'z': viewZ += step;
				break;
	}
}

unsigned int loadImage(const char * fileName, GLint format, bool verticalFlip, GLint wrapMode)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(verticalFlip);
	unsigned char * data = stbi_load(fileName, &width, &height, &nrChannels, 0);
	if ( data )
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "ERROR::LOAD TEXTURE FAILED" << std::endl;
	}
	stbi_image_free(data);
	return texture;
}