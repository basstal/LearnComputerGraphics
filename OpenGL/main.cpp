#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <shader.h>

float vertices1[] = {
	-0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
};

float vertices2[] = {
	-0.5f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
};

float verticesIBO[] = {
	// 位置					颜色
	0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f
};

unsigned int indices[] = {
	1, 2, 4,
};

void frame_buffer_viewport(GLFWwindow *, int, int);
void processInput(GLFWwindow *);
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

	unsigned int VAO1;
	glGenVertexArrays(1, &VAO1);

	glBindVertexArray(VAO1);

	unsigned int VBO1;
	glGenBuffers(1, &VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);


	unsigned int VAO2;
	glGenVertexArrays(1, &VAO2);

	glBindVertexArray(VAO2);

	unsigned int VBO2;
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	unsigned int VAO3;
	glGenVertexArrays(1, &VAO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VAO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesIBO), verticesIBO, GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// glUseProgram(shaderProgram);
		shaderProgram.use();
		//glBindVertexArray(VAO2);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);

		// 这里按时间设置shader中uniform变量 改变颜色值
		// float time = glfwGetTime();
		// float greenValue = (sin(time) / 2.0f) + 0.5f;
		// int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		// glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		glBindVertexArray(VAO3);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glBindVertexArray(VAO1);
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
}

// void ShaderCode(const char *fileName, std::string &shaderCode)
// {
// 	std::ifstream shaderFile(fileName);
//     if(!shaderFile.is_open())
//     {
//         throw std::runtime_error(std::string("Failed to open file: ") + fileName);
//     }
// 	std::string str((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
// 	shaderFile.close();
// 	shaderCode.clear();
//     // std::cout << "content : " << str << std::endl;
// 	shaderCode.append(str);
// 	// std::cout<< shaderCode << std::endl;
// }

// void CheckCompileShader(unsigned int vertexShader)
// {
// 	int success;
// 	char infoLog[512];
// 	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
// 	if (!success)
// 	{
// 		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
// 		std::cout << "SHADER COMPILE ERROR :\n " << infoLog << std::endl;
// 	}
// }


// void CheckLinkProgram(unsigned int shaderProgram)
// {
// 	int success;
// 	char infoLog[512];
// 	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
// 	if (!success)
// 	{
// 		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
// 		std::cout << "PROGRAM LINK ERROR :\n " << infoLog << std::endl;
// 	}
// }

// unsigned int ProcessShader(const char * vert, const char * frag)
// {
	
// }