// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"
#include <stdio.h>
#include <vector>
#include <GL/glew.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

void buildPlane(GLfloat topleft_x, GLfloat topleft_y, int rows, int cols, std::vector<GLfloat>& vertices) {
	// Split into squares made up of 2 triangles
	GLfloat colwidth = std::fabs(topleft_x) * 2 / cols;
	GLfloat rowwidth = std::fabs(topleft_y) * 2 / rows;

	vertices.clear();
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			// Build the two triangles
			// 1 Top Left
			vertices.push_back(topleft_x + (j*colwidth));
			vertices.push_back(topleft_y - (i*rowwidth));
			vertices.push_back(0.0f);
			// 1 Bottom Left
			vertices.push_back(topleft_x + (j*colwidth));
			vertices.push_back(topleft_y - ((i + 1)*rowwidth));
			vertices.push_back(0.0f);
			// 1 Bottom Right
			vertices.push_back(topleft_x + ((j + 1)*colwidth));
			vertices.push_back(topleft_y - ((i + 1)*rowwidth));
			vertices.push_back(0.0f);

			// 2 Top Left
			vertices.push_back(topleft_x + (j*colwidth));
			vertices.push_back(topleft_y - (i*rowwidth));
			vertices.push_back(0.0f);
			// 2 Top Right
			vertices.push_back(topleft_x + ((j + 1)*colwidth));
			vertices.push_back(topleft_y - (i*rowwidth));
			vertices.push_back(0.0f);
			// 2 Bottom Right
			vertices.push_back(topleft_x + ((j + 1)*colwidth));
			vertices.push_back(topleft_y - ((i + 1)*rowwidth));
			vertices.push_back(0.0f);
		}
	}
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

int main(int, char**)
{
	int screenWidth = 1280;
	int screenHeight = 720;
	std::string shaderpath = "F:/github/WorldBuilder/shaders/";
	Camera camera(screenWidth, screenHeight);
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	glewInit();

    // Setup ImGui binding
    ImGui_ImplSdlGL3_Init(window);

	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = CreateShader(shaderpath+"model.vert", GL_VERTEX_SHADER);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = CreateShader(shaderpath + "simple.frag", GL_FRAGMENT_SHADER);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// Set up vertex data (and buffer(s)) and attribute pointers
	//GLfloat vertices[] = {
	//  // First triangle
	//   0.5f,  0.5f,  // Top Right
	//   0.5f, -0.5f,  // Bottom Right
	//  -0.5f,  0.5f,  // Top Left 
	//  // Second triangle
	//   0.5f, -0.5f,  // Bottom Right
	//  -0.5f, -0.5f,  // Bottom Left
	//  -0.5f,  0.5f   // Top Left
	//}; 
	GLfloat vertices[] = {
		0.5f,  0.5f, 0.0f,  // Top Right
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  // Bottom Left
		-0.5f,  0.5f, 0.0f   // Top Left 
	};
	GLfloat new_vertex[] = { 0.0f, 0.5f, 0.0f };

	std::vector<GLfloat> genverts;
	buildPlane(-1.0f, 1.0f, 100, 100, genverts);
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3,  // First Triangle
		1, 2, 3   // Second Triangle
	};
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * genverts.size(), genverts.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 window_color = ImColor(100, 100, 100);
    ImVec4 clear_color = ImColor(0, 0, 0);
	float color[3] = { 0.0f, 1.0f, 0.0f };

    // Main loop
    bool done = false;
	bool mousedown = false;
	bool keys[1024] = { false };
    while (!done)
    {
		mousedown = false;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
			switch (event.type) {
				case SDL_QUIT:
					/* Quit */
					done = true;
					break;
				case SDL_KEYDOWN:
					keys[event.key.keysym.scancode] = true;
					break;
				case SDL_KEYUP:
					keys[event.key.keysym.scancode] = false;
					break;
				case SDL_MOUSEWHEEL:
					camera.SetFov(event.wheel.y);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						mousedown = true;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if (event.button.button == SDL_BUTTON_LEFT) {
						mousedown = false;
					}
					break;
				default:
					break;
			}
        }
		int xpos, ypos;
		SDL_GetMouseState(&xpos, &ypos);
		camera.Update(xpos, ypos, keys);

        ImGui_ImplSdlGL3_NewFrame(window);

		if (mousedown) {
			// Check if vertex or face was selected
			// Convert either screen to local or local vertices to screen but that seems redundant
			// How should coordinates be stored?
			std::cout << xpos << " " << ypos << std::endl;
		}

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&window_color);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Mouse X: %d Mouse Y: %d", xpos, ypos);
			
        }

        // Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glm::mat4 model;
		//transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(5.0, 5.0, 5.0));

		// Get matrix's uniform location and set matrix
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 projection = glm::perspective(camera.Zoom(), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 view = camera.GetViewMatrix();
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		new_vertex[0] += 0.001f;
		if (new_vertex[0] > 1.0f) {
			new_vertex[0] = 0.0f;
		}

		// Modify the vertex data
		/*
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(new_vertex), &new_vertex);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		*/
		// Render app
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, genverts.size());
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// Render gui
        ImGui::Render();


        SDL_GL_SwapWindow(window);
    }

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

    // Cleanup
    ImGui_ImplSdlGL3_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
