#pragma once
#include <iostream>
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
	std::unordered_map<const char *, int> uniformLocations;
	int ID;

	void check(GLuint shader, std::string type);
	int getUniformLocation(const char* name);
public:
	Shader(const char* vert, const char* frag, const char* geo = nullptr);

	~Shader() {
		glDeleteShader(ID);
	}

	inline void setInt(const char* name, const int &value) {
		glUniform1i(getUniformLocation(name), value);
	}

	inline void setFloat(const char* name, const float &value)  {
		glUniform1f(getUniformLocation(name), value);
	}

	inline void setVec3(const char* name, const glm::vec3 &value) {
		glUniform3fv(getUniformLocation(name), 1, &value[0]);
	}

	inline void setMat4(const char* name, const glm::mat4 &value) {
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	inline void use() const { glUseProgram(ID); }
	inline void disable() const { glUseProgram(0); }
};

Shader::Shader(const char* vert, const char* frag, const char* geo) {
	std::ifstream vertFile, fragFile;
	std::stringstream vertexStream, fragmentStream;
	std::string vertexString, fragmentString, geometryString;

	vertFile.open(vert);
	fragFile.open(frag);

	vertexStream << vertFile.rdbuf();
	fragmentStream << fragFile.rdbuf();

	vertFile.close();
	fragFile.close();

	vertexString = vertexStream.str();
	fragmentString = fragmentStream.str();

	const char* vertexSource = vertexString.c_str();
	const char* fragmentSource = fragmentString.c_str();

	if (geo != nullptr) {
		std::ifstream geoFile;
		std::stringstream geometryStream;
		geoFile.open(geo);
		geometryStream << geoFile.rdbuf();
		geometryString = geometryStream.str();
		geoFile.close();
	}

	GLuint vertShader, fragShader, geoShader;
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertexSource, NULL);
	glCompileShader(vertShader);
	check(vertShader, "VERTEX");

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragmentSource, NULL);
	glCompileShader(fragShader);
	check(fragShader, "FRAGMENT");

	if (geo != nullptr) {
		const char* geometrySource = geometryString.c_str();

		geoShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geoShader, 1, &geometrySource, NULL);
		glCompileShader(geoShader);
		check(fragShader, "GEOMETRY");
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertShader);
	glAttachShader(ID, fragShader);
	if (geo != nullptr) glAttachShader(ID, geoShader);

	glLinkProgram(ID);
	check(ID, "PROGRAM");

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	if (geo != nullptr) glDeleteShader(geoShader);
}

void Shader::check(GLuint shader, std::string type) {
	GLint success;
	GLchar infoLog[1024];

	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR - " << type << ":\n" << infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR - PROGRAM:\n" << infoLog << std::endl;
		}
	}
}

int Shader::getUniformLocation(const char* name) {
	if (uniformLocations.find(name) != uniformLocations.end())
		return uniformLocations[name];
	else {
		int location = glGetUniformLocation(ID, name);
		uniformLocations[name] = location;
		return location;
	}
}