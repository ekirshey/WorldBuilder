#pragma once

#include <GL/glew.h>
#include <SDL.h>

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

class Shader
{
public:
	#define DEFAULTSHADER 0xFFF

	bool AddVertexShader(GLuint& id, std::string file) {
		GLuint shaderId = CreateShader(file, GL_VERTEX_SHADER);
		if (TryCompileShader(shaderId)) {
			vertexIds.push_back(shaderId);
			id = shaderId;
			return true;
		}
		else 
			return false;
	}

	bool AddFragmentShader(GLuint& id, std::string file) {
		GLuint shaderId = CreateShader(file, GL_FRAGMENT_SHADER);
		if (TryCompileShader(shaderId)) {
			fragIds.push_back(shaderId);
			id = shaderId;
			return true;
		}
		else
			return false;
	}

	bool AddGeometryShader(GLuint& id, std::string file) {
		GLuint shaderId = CreateShader(file, GL_GEOMETRY_SHADER);
		if (TryCompileShader(shaderId)) {
			geomIds.push_back(shaderId);
			id = shaderId;
			return true;
		}
		else
			return false;
	}

	int BuildNewProgram(std::string vertexfile, std::string fragmentfile, GLuint geometryid ) {
		GLuint vertexid, fragmentid;
		bool vertres = AddVertexShader(vertexid, vertexfile);
		bool fragres = AddFragmentShader(fragmentid, fragmentfile);
		if (!(vertres && fragres))
			return -1;

		GLuint newProgram = glCreateProgram();
		// DEFAULTSHADER means, use default
		if (vertexid != DEFAULTSHADER)
			glAttachShader(newProgram, vertexid);
		if ( fragmentid != DEFAULTSHADER)
			glAttachShader(newProgram, fragmentid);
		if (geometryid != DEFAULTSHADER)
			glAttachShader(newProgram, geometryid);

		bool link = LinkShaders(newProgram);
		// delete shaders??
		// probably but if i delete, can i reuse?
		glDeleteShader(vertexid);
		glDeleteShader(fragmentid);
		if (link) {
			shaderPrograms.push_back(ShaderProgram{ newProgram, vertexid, fragmentid, geometryid });
			return shaderPrograms.size() - 1;
		}
		else
			return -1;
	}

	int BuildNewProgram(GLuint vertexid, GLuint fragmentid, GLuint geometryid ) {
		GLuint newProgram = glCreateProgram();
		// DEFAULTSHADER means, use default
		if (vertexid != DEFAULTSHADER)
			glAttachShader(newProgram, vertexid);
		if ( fragmentid != DEFAULTSHADER)
			glAttachShader(newProgram, fragmentid);
		if (geometryid != DEFAULTSHADER)
			glAttachShader(newProgram, geometryid);

		bool link = LinkShaders(newProgram);
		if (link) {
			shaderPrograms.push_back(ShaderProgram{ newProgram, vertexid, fragmentid, geometryid });
			return shaderPrograms.size() - 1;
		}
		else
			return -1;
	}

	void UseProgram(int id)
	{
		// Load the shader into the rendering pipeline
		glUseProgram(shaderPrograms[id].id);
	}

	GLint GetUniformlocation(int id, std::string var) const{
		return glGetUniformLocation(shaderPrograms[id].id, var.c_str());
	}

	void CleanUp()
	{
		// Cleanup all the things we bound and allocated
		glUseProgram(0);

		for (auto p : shaderPrograms) {
			if ( p.vertexId != DEFAULTSHADER)
				glDetachShader(p.id, p.vertexId);
			if ( p.fragId != DEFAULTSHADER)
				glDetachShader(p.id, p.fragId);
			if ( p.geomId != DEFAULTSHADER)
				glDetachShader(p.id, p.geomId);
			glDeleteProgram(p.id);
		}

		for (auto i : vertexIds)
			glDeleteShader(i);
		for (auto i : fragIds)
			glDeleteShader(i);
		for (auto i : geomIds)
			glDeleteShader(i);
	}

private:
	struct ShaderProgram {
		GLuint id;
		GLuint vertexId;
		GLuint fragId;
		GLuint geomId;

		ShaderProgram(GLuint id, GLuint vert, GLuint frag, GLuint geom) 
			: id(id)
			, vertexId(vert)
			, fragId(frag)
			, geomId(geom)
		{

		}
	};

	// Tries to compile the shader. Returns false if something fails
	bool TryCompileShader(int shaderId)
	{
		// Compile the vertex shader
		glCompileShader(shaderId);

		// Ask OpenGL if the shaders was compiled
		int wasCompiled = 0;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &wasCompiled);

		PrintShaderCompilationErrorInfo(shaderId);

		// Return false if compilation failed
		return (wasCompiled != 0);
	}

	// Create shader and set the source
	GLuint CreateShader(const std::string &fileName, GLenum shaderType)
	{
		// Read file as std::string 
		std::string str = ReadFile(fileName.c_str());

		// c_str() gives us a const char*, but we need a non-const one
		char* src = const_cast<char*>(str.c_str());
		int32_t size = str.length();

		// Create an empty vertex shader handle
		GLuint shaderId = glCreateShader(shaderType);

		// Send the vertex shader source code to OpenGL
		glShaderSource(shaderId, 1, &src, &size);

		return shaderId;
	}

	bool LinkShaders(GLuint programid)
	{
		// At this point, our shaders will be inspected/optized and the binary code generated
		// The binary code will then be uploaded to the GPU
		glLinkProgram(programid);

		// Verify that the linking succeeded
		int isLinked;
		glGetProgramiv(programid, GL_LINK_STATUS, (int *)&isLinked);

		if (isLinked == false)
			PrintShaderLinkingError(programid);

		return isLinked != 0;
	}

	std::string ReadFile(const char* file)
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

	void PrintShaderLinkingError(GLuint programId)
	{
		std::cout << "=======================================\n";
		std::cout << "Shader linking failed : " << std::endl;

		// Find length of shader info log
		int maxLength;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &maxLength);

		std::cout << "Info Length : " << maxLength << std::endl;

		// Get shader info log
		char* shaderProgramInfoLog = new char[maxLength];
		glGetProgramInfoLog(programId, maxLength, &maxLength, shaderProgramInfoLog);

		std::cout << "Linker error message : " << shaderProgramInfoLog << std::endl;

		// Handle the error by printing it to the console
		delete shaderProgramInfoLog;
		return;
	}

	// If something went wrong whil compiling the shaders, we'll use this function to find the error
	void PrintShaderCompilationErrorInfo(int32_t shaderId)
	{
		// Find length of shader info log
		int maxLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

		// Get shader info log
		char* shaderInfoLog = new char[maxLength];
		glGetShaderInfoLog(shaderId, maxLength, &maxLength, shaderInfoLog);

		std::string log = shaderInfoLog;

		if (log.length())
		{
			std::cout << "=======================================\n";
			std::cout << shaderInfoLog << std::endl;
			std::cout << "=======================================\n\n";
		}

		// Print shader info log
		delete shaderInfoLog;
	}

	// The handle to our shader program
	std::vector<ShaderProgram> shaderPrograms;

	std::vector<int32_t> vertexIds;
	std::vector<int32_t> fragIds;
	std::vector<int32_t> geomIds;

	// The handles to the induvidual shader
	GLuint vertexshader, fragmentShader;
};
