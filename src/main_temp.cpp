#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLEW
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL.h>
#undef main

/* assimp include files. These three are usually needed. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Window dimensions
const GLuint WIDTH = 1200, HEIGHT = 960;

std::string programName = "Headerphile SDL2 - OpenGL thing";

// Our SDL_Window ( just like with SDL2 wihout OpenGL)
SDL_Window *mainWindow;

// Our opengl context handle
SDL_GLContext mainContext;

bool SetOpenGLAttributes()
{
	// Set our OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	// Turn on double buffering with a 24bit Z buffer.
	// You may need to change this to 16 or 32 for your system
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	return true;
}

void CheckSDLError(int line = -1)
{
	std::string error = SDL_GetError();

	if (error != "")
	{
		std::cout << "SDL Error : " << error << std::endl;

		if (line != -1)
			std::cout << "\nLine : " << line << std::endl;

		SDL_ClearError();
	}
}

int main() {
	std::string shaderpath = "F:/github/SAS_3D/shaders/";
	std::string mediapath = "F:/github/SAS_3D/media/";
	int screenWidth = WIDTH;
	int screenHeight = HEIGHT;
	Camera c(screenWidth, screenHeight);

	Shader shader;
	// Initialize SDL's Video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Failed to init SDL\n";
		return false;
	}

	// Create our window centered at 512x512 resolution
	mainWindow = SDL_CreateWindow(programName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	// Check that everything worked out okay
	if (!mainWindow)
	{
		std::cout << "Unable to create window\n" << std::endl;;
		CheckSDLError(__LINE__);
		return false;
	}

	SDL_CaptureMouse(SDL_TRUE);
	SDL_ShowCursor(0);
	SetOpenGLAttributes();

	// Create our opengl context and attach it to our window
	mainContext = SDL_GL_CreateContext(mainWindow);

	// This makes our buffer swap syncronized with the monitor's vertical refresh
	SDL_GL_SetSwapInterval(1);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Enable Z Buffer
	glEnable(GL_DEPTH_TEST);

	SDL_GL_SwapWindow(mainWindow);

	int lightingShader = shader.BuildNewProgram(shaderpath + "lighting.vert", shaderpath + "lighting.frag", DEFAULTSHADER);
	int lampShader = shader.BuildNewProgram(shaderpath + "lamp.vert", shaderpath + "lamp.frag", DEFAULTSHADER);
	int modelShader = shader.BuildNewProgram(shaderpath + "model.vert", shaderpath + "model.frag", DEFAULTSHADER);

	Model themodel("../../media/nanosuit/nanosuit.obj");

	bool keys[1024] = { false };
	bool quit = false;
	while (!quit)
	{
		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Clear color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Get SDL Input
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			// handle your event here
			switch (event.type) {
			case SDL_QUIT:
				/* Quit */
				quit = true;
				break;
			case SDL_KEYDOWN:
				keys[event.key.keysym.scancode] = true;
				break;
			case SDL_KEYUP:
				keys[event.key.keysym.scancode] = false;
				break;
			case SDL_MOUSEWHEEL:
				c.SetFov(event.wheel.y);
				break;
			default:
				break;
			}
		}

		// Mouse handling for yaw, pitch, zoom
		int xpos, ypos;
		SDL_GetMouseState(&xpos, &ypos);
		c.Update(xpos, ypos, keys);

		// Transformation matrices
		shader.UseProgram(modelShader);
		glm::mat4 projection = glm::perspective(c.Zoom(), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view = c.GetViewMatrix();
		glUniformMatrix4fv(shader.GetUniformlocation(modelShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(shader.GetUniformlocation(modelShader, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(shader.GetUniformlocation(modelShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		themodel.Draw(shader, modelShader);

		SDL_GL_SwapWindow(mainWindow);
	}


	// Delete our OpengL context
	SDL_GL_DeleteContext(mainContext);

	// Destroy our window
	SDL_DestroyWindow(mainWindow);

	// Shutdown SDL 2
	SDL_Quit();

	return 0;
}