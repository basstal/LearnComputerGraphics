#include <Shader.h>

Shader::Shader(const char * vShaderCode, const char *fShaderCode, const char * gShaderCode, bool isSource)
{
    compileAndLinkShaders(vShaderCode, fShaderCode, gShaderCode);
}

void Shader::compileAndLinkShaders(const char * vShaderCode, const char *fShaderCode, const char * gShaderCode)
{
	unsigned int vertexShader, fragmentShader, geometryShader;
    int success;
	char infoLog[512];

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::VERTEX SHADER::COMPILE FAILED:\n " << infoLog << std::endl;
	}
	else
	{
		glGetProgramInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "VERTEX SHADER::COMPILE SUCCESS:\n " << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::FRAGMENT SHADER::COMPILE FAILED:\n " << infoLog << std::endl;
	}
	else
	{
		glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "FRAGMENT SHADER::COMPILE SUCCESS:\n " << infoLog << std::endl;
	}

	if (gShaderCode != NULL)
	{
		std::cout << "use geometry" << std::endl;
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &gShaderCode, NULL);
		glCompileShader(geometryShader);
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
		if (! success)
		{
			glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
			std::cout << "ERROR::GEOMETRY SHADER::COMPILE FAILED:\n " << infoLog << std::endl;
		}
		else
		{
			glGetProgramInfoLog(geometryShader, 512, NULL, infoLog);
			std::cout << "GEOMETRY SHADER::COMPILE SUCCESS:\n " << infoLog << std::endl;
		}
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	if (gShaderCode != NULL)
		glAttachShader(ID, geometryShader);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM LINK FAILED:\n " << infoLog << std::endl;
	}
	else
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "SHADER::PROGRAM LINK SUCCESS:\n " << infoLog << std::endl;
	}
	

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (gShaderCode != NULL)
		glDeleteShader(geometryShader);

}

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath, const GLchar * geometryPath)
{
    std::string vertexCode;
    std::string fragmentCode;
	std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
	std::ifstream gShaderFile;

    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStram, fShaderStram;
        vShaderStram << vShaderFile.rdbuf();
        fShaderStram << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStram.str();
        fragmentCode = fShaderStram.str();
		if (geometryPath != NULL && strcmp(geometryPath, "") == 1)
		{
			std::cout << "use geometry" << geometryPath << std::endl;
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStram;
			gShaderStram << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStram.str();
		}
    }
    catch(std::exception e)
    {
        std::cout<< "ERROR::SHADER::FILE NOT SUCCESSFULLY READ, vertexPath[" <<vertexPath << "], fragmentPath[" << fragmentPath << "]" << "\n"<< e.what() << std::endl;
    };
	const char * vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();
	const char * gShaderCode = NULL;
	if (!geometryCode.empty())
		gShaderCode = geometryCode.c_str();
	compileAndLinkShaders(vShaderCode, fShaderCode, gShaderCode);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setInt(const std::string &varName, int val) const
{
    glUniform1i(glGetUniformLocation(ID, varName.c_str()), val);
}

void Shader::setBool(const std::string &varName, bool val) const
{
    glUniform1i(glGetUniformLocation(ID, varName.c_str()), (int)val);
}

void Shader::setFloat(const std::string &varName, float val) const
{
    glUniform1f(glGetUniformLocation(ID, varName.c_str()), val);
}

void Shader::setMat4(const std::string &varName, glm::mat4 val) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, varName.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setVec3(const std::string &varName, float *val, int count) const
{
	glUniform3fv(glGetUniformLocation(ID, varName.c_str()), count, val);
}

void Shader::setVec3(const std::string &varName, glm::vec3 val) const
{
	glUniform3fv(glGetUniformLocation(ID, varName.c_str()), 1, glm::value_ptr(val));
}

void Shader::setVec3(const std::string &varName, float x, float y, float z) const
{
	glm::vec3 v3 = glm::vec3(x, y, z);
	glUniform3fv(glGetUniformLocation(ID, varName.c_str()), 1, glm::value_ptr(v3));
}

void Shader::setVec2(const std::string &varName, glm::vec2 val) const
{
	glUniform2fv(glGetUniformLocation(ID, varName.c_str()), 1, glm::value_ptr(val));
}
