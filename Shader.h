#pragma once
#include <iostream>
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
	void check(GLuint shader, std::string type) {
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
	// TODO: add unordered map for storing UniformLocations
public:
	int ID;
	Shader(const char* vert, const char* frag, const char* geo = nullptr) {
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

	~Shader() {
		glDeleteShader(ID);
	}

	inline void setInt(const char* name, const int &value) const {
		glUniform1i(glGetUniformLocation(ID, name), value);
	}

	inline void setFloat(const char* name, const float &value) const {
		glUniform1f(glGetUniformLocation(ID, name), value);
	}

	inline void setVec3(const char* name, const glm::vec3 &value) const {
		glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
	}

	inline void setMat4(const char* name, const glm::mat4 &value) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(value));
	}

	inline void use() const { glUseProgram(ID); }
	inline void disable() const { glUseProgram(0); }
};