#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <GL/glew.h> 

class ShaderProgram {
	public:
		ShaderProgram(std::string vertexfile, std::string fragfile) { 

			GLuint vertexShader = _createShader(vertexfile, GL_VERTEX_SHADER);
			_compileShader(vertexShader);

			GLuint fragmentShader = _createShader(fragfile, GL_FRAGMENT_SHADER);
			_compileShader(fragmentShader);

			// Link shaders
			GLint success;
			GLchar infoLog[512];
			_shaderprogram = glCreateProgram();
			glAttachShader(_shaderprogram, vertexShader);
			glAttachShader(_shaderprogram, fragmentShader);
			glLinkProgram(_shaderprogram);
			// Check for linking errors
			glGetProgramiv(_shaderprogram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(_shaderprogram, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
		}

		void useProgram() {
			glUseProgram(_shaderprogram);
		}

		GLint getUniformLocation(std::string name) {
			if (_cacheduniforms.find(name) != _cacheduniforms.end()) {
				return _cacheduniforms[name];
			}
			GLint uniform = glGetUniformLocation(_shaderprogram, name.c_str());
			_cacheduniforms.insert({ name, uniform });

			return uniform;
		}

	private:

		bool _compileShader(GLuint shader) {
			// Build and compile our shader program
			// Vertex shader
			glCompileShader(shader);
			// Check for compile time errors
			GLint success;
			GLchar infoLog[512];
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
				return false;
			}

			return true;
		}

		std::string _readFile(const char* file)
		{
			// Open file
			std::ifstream t(file);

			// Read file into buffer
			std::stringstream buffer;
			buffer << t.rdbuf();

			// Make a std::string and fill it with the contents of buffer
			std::string fileContent = buffer.str();

			return fileContent;
		}

		// Create shader and set the source
		GLuint _createShader(const std::string &fileName, GLenum shaderType)
		{
			// Read file as std::string 
			std::string str = _readFile(fileName.c_str());

			// c_str() gives us a const char*, but we need a non-const one
			char* src = const_cast<char*>(str.c_str());
			int32_t size = str.length();

			// Create an empty vertex shader handle
			GLuint shaderId = glCreateShader(shaderType);

			// Send the vertex shader source code to OpenGL
			glShaderSource(shaderId, 1, &src, &size);

			return shaderId;
		}

		std::unordered_map<std::string, GLint> _cacheduniforms;
		GLuint _shaderprogram;
};
