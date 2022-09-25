#include <Shader.h>
#include <Utils.h>

Shader::Shader(const Shader& shader)
{
	vsPath = shader.vsPath;
	fsPath = shader.fsPath;
	gsPath = shader.gsPath;
	ID = shader.ID;
	recompileFromSource();
}

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

	if (gShaderCode && gShaderCode[0] != '\0')
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
	if (gShaderCode && gShaderCode[0] != '\0')
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
	if (gShaderCode && gShaderCode[0] != '\0')
		glDeleteShader(geometryShader);

}

Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath)
{
	readAndCompileAll(vertexShaderPath.c_str(), fragmentShaderPath.c_str(), geometryShaderPath.c_str());
}

void Shader::recompileFromSource()
{
	readAndCompileAll(vsPath.c_str(), fsPath.c_str(), gsPath.c_str());
}

std::string Shader::readShaderCode(const GLchar * shaderPath)
{
	if (shaderPath && strcmp(shaderPath, "") == 1)
	{
		std::ifstream shaderFile;

		shaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			shaderFile.open(shaderPath);
			std::stringstream shaderStram;
			shaderStram << shaderFile.rdbuf();
			shaderFile.close();
			return shaderStram.str();
		}
		catch(std::exception e)
		{
			std::cout<< "ERROR::SHADER::FILE NOT SUCCESSFULLY READ, shaderPath[" <<shaderPath << "]\n"<< e.what() << std::endl;
		};
	}
	return "";
}

void Shader::readAndCompileAll(const GLchar * vertexPath, const GLchar * fragmentPath, const GLchar * geometryPath)
{
	std::string vShaderCode, fShaderCode, gShaderCode;
	if(vertexPath && vertexPath[0] != '\0' && std::string(vertexPath).find("LearnOpenGL") == -1)
	{
		getProjectFilePath(vertexPath, vsPath);
		vShaderCode = readShaderCode(vsPath.c_str());
	}
	else
	{
	    vShaderCode = readShaderCode(vertexPath);
	}
	if (fragmentPath && fragmentPath[0] != '\0' && std::string(fragmentPath).find("LearnOpenGL") == -1)
	{
		getProjectFilePath(fragmentPath, fsPath);
		fShaderCode = readShaderCode(fsPath.c_str());

	}
	else
	{
		fShaderCode = readShaderCode(fragmentPath);

	}
	if (geometryPath && geometryPath[0] != '\0' && std::string(geometryPath).find("LearnOpenGL") == -1)
	{
		getProjectFilePath(geometryPath, gsPath);
		gShaderCode = readShaderCode(gsPath.c_str());

	}
	else
	{
		gShaderCode = readShaderCode(geometryPath);
	}
	compileAndLinkShaders(vShaderCode.c_str(), fShaderCode.c_str(), gShaderCode.c_str());
}

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath, const GLchar * geometryPath)
{
	readAndCompileAll(vertexPath, fragmentPath, geometryPath);
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

void Shader::setVec4(const std::string &varName, glm::vec4 val) const
{
	glUniform4fv(glGetUniformLocation(ID, varName.c_str()), 1, glm::value_ptr(val));
}
