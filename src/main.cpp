// ImGui - standalone example application for SDL2 + OpenGL
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <string>
#include <unordered_map>
#include <iostream>
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
#include "Texture.h"
#include "Shader.h"
#include "Ray.h"
#include <glm/ext.hpp>

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void buildPlane(GLfloat topleft_x, GLfloat topleft_y, int rows, int cols, std::vector<GLfloat>& vertices, std::vector<unsigned int>& indices) {
	GLfloat colwidth = std::fabs(topleft_x) * 2 / cols;
	GLfloat rowwidth = std::fabs(topleft_y) * 2 / rows;

	for (int i = 0; i <= rows; i++) {
		for (int j = 0; j <= cols; j++) {
			vertices.push_back(topleft_x + (j*colwidth));
			vertices.push_back(0.0f);;
			vertices.push_back(topleft_y - (i*rowwidth));

			vertices.push_back(topleft_x + (j*colwidth));
			vertices.push_back(topleft_y - (i*rowwidth));
		}
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			// Left triangle
			indices.push_back((i*(cols+1))+j);
			indices.push_back(((i+1)*(cols+1))+j);
			indices.push_back(((i+1)*(cols+1))+j+1);

			// Right triangle
			indices.push_back((i*(cols+1)) + j);
			indices.push_back((i*(cols+1)) + j + 1);
			indices.push_back(((i + 1)*(cols+1)) + j + 1);
		}
	}
}


struct textureinfo {
	std::string name;
	bool state;
};
int main(int, char**)
{
	bool wireframe = false;
	std::unordered_map<GLuint, textureinfo> texture_list;
	int screenWidth = 1280;
	int screenHeight = 720;
	std::string shaderpath = "F:/github/WorldBuilder/shaders/";
	std::string mediapath = "F:/github/WorldBuilder/media/";

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

	glEnable(GL_DEPTH_TEST);

	std::vector<GLfloat> cube = {
		-0.5, -0.5, -0.5,
		0.5, -0.5, -0.5,
		0.5,  0.5, -0.5,
		-0.5,  0.5, -0.5,
		-0.5, -0.5,  0.5,
		0.5, -0.5,  0.5,
		0.5,  0.5,  0.5,
		-0.5,  0.5,  0.5,
	};

	std::vector<unsigned int> cube_indices{
		0,1,1,2,2,3,3,0, // First square
		0,4,1,5,2,6,3,7, // connections
		4,5,5,6,6,7,7,4  // Second square
	};

	std::vector<GLfloat> genverts;
	std::vector<unsigned int> indices;
	buildPlane(-1.0f, 1.0f, 100, 100, genverts, indices);

	ShaderProgram textureProgram(shaderpath + "model.vert", shaderpath + "texture.frag");
	ShaderProgram cubeProgram(shaderpath + "simple.vert", shaderpath + "simple.frag");

	GLuint texture = LoadTexture(mediapath + "container2.png", GL_RGBA);
	textureinfo t;
	t.name = "Container2";
	t.state = true;
	texture_list.insert({ texture, t });

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * genverts.size(), genverts.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	GLuint CUBEVBO, CUBEVAO, CUBEEBO;
	glGenVertexArrays(1, &CUBEVAO);
	glGenBuffers(1, &CUBEVBO);
	glGenBuffers(1, &CUBEEBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(CUBEVAO);

	glBindBuffer(GL_ARRAY_BUFFER, CUBEVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cube.size(), cube.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CUBEEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * cube_indices.size(), cube_indices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Is this needed?
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

    bool show_test_window = true;
    bool show_another_window = false;
    ImVec4 window_color = ImColor(100, 100, 100);
    ImVec4 clear_color = ImColor(0, 0, 0);
	float color[3] = { 0.0f, 1.0f, 0.0f };

	glm::vec4 selected_vertex;
	bool vertex_selected = false;
	bool triangle_selected = false;
	int selected_index;
	int selected_triangle;
	GLfloat moveX = 0.0;
	GLfloat moveY = 0.0;
	GLfloat moveZ = 0.0;
    // Main loop
    bool done = false;
	bool mousedown = false;
	bool keys[1024] = { false };

	bool boxdraw = false;
	bool shiftdown = false;
	Ray boxstart;
	glm::mat4 box_model;
	glm::mat4 scaled_box_model;
	std::vector<int> indices_in_box;
	GLfloat box_scale = 0.1f;
	Ray ray;
    while (!done)
    {
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

		// Key handling
		moveX = 0.0f;
		moveY = 0.0f;
		moveZ = 0.0f;
		// if vertex selected
		if (keys[SDL_SCANCODE_LSHIFT]) {
			shiftdown = true;
		}
		else {
			shiftdown = false;
		}
		if (keys[SDL_SCANCODE_H]) {
			moveX = -0.01f;
		}
		if (keys[SDL_SCANCODE_L]) {
			moveX = 0.01f;
		}
		if (keys[SDL_SCANCODE_J]) {
			moveY = -0.01f;
		}
		if (keys[SDL_SCANCODE_K]) {
			moveY = 0.01f;
		}
		if (keys[SDL_SCANCODE_N]) {
			moveZ = -0.01f;
		}
		if (keys[SDL_SCANCODE_M]) {
			moveZ = 0.01f;
		}


        ImGui_ImplSdlGL3_NewFrame(window);
		char buf[256] = "";

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Mouse X: %d Mouse Y: %d", xpos, ypos);
			ImGui::Text("Pick Ray: X: %f Y: %f Z: %f", ray.d_x(), ray.d_y(), ray.d_z());
			if (ImGui::InputText("Add Texture", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
				textureinfo t;
				t.name = std::string(buf);
				t.state = false;
				GLuint texture = LoadTexture(mediapath + t.name, GL_RGB);
				texture_list.insert({ texture, t });
			}

			ImGui::Checkbox("Wireframe", &wireframe);

			for (auto it = texture_list.begin(); it != texture_list.end(); ++it) {
				ImGui::Checkbox(it->second.name.c_str(), &it->second.state);
			}

        }

        // Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		textureProgram.useProgram();
		glm::mat4 model;
		//transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, glm::vec3(1.0, 1.0, 1.0));

		// Get matrix's uniform location and set matrix
		GLint modelLoc = textureProgram.getUniformLocation("model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 view = camera.GetViewMatrix();
		//std::cout << glm::to_string(view) << std::endl;
		GLint viewLoc = textureProgram.getUniformLocation("view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::perspective(camera.Zoom(), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		//std::cout << glm::to_string(projection) << std::endl;
		GLint projectionLoc = textureProgram.getUniformLocation("projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		auto camerapos = glm::vec4(camera.Position(),1.0f);

		if (mousedown) {
			float norm_x = (GLfloat)(2.0f *xpos) / screenWidth - 1.0f;
			float norm_y = 1.0f - (GLfloat)(2.0f*ypos) / screenHeight;
			ray.reset(norm_x, norm_y, camera.Position(), projection, view);

			if (shiftdown & !boxdraw) {
				boxdraw = true;
				boxstart = ray;
				glm::vec3 normal(0.0f, 1.0f, 0.0f);
				glm::vec3 offset = camerapos;
				float planedistance = ray.intersectWithPlane(normal, offset);

				box_model = glm::translate(glm::vec3(ray.d_x() * planedistance +camerapos.x, 0.0f, ray.d_z()*planedistance+camerapos.z));
				scaled_box_model = glm::scale(box_model, glm::vec3(box_scale, 1.0f, box_scale));
			}
			else if( shiftdown & boxdraw ) {
				GLfloat movement = distance(ray, boxstart)*2;
				if (movement < 0.1f) {
					movement = 0.1f;
				}
				scaled_box_model = glm::scale(box_model, glm::vec3(movement, 1.0f, movement));
			
				double leftbound = box_model[3][0] + (movement * -0.5);
				double rightbound = box_model[3][0] + (movement * 0.5);
				double frontbound = box_model[3][2] + (movement * 0.5);
				double backbound = box_model[3][2] + (movement * -0.5);
				indices_in_box.clear();
				for (int i = 0; i < genverts.size(); i += 5) {
					glm::vec3 currentpoint = glm::vec3(genverts[i], genverts[i + 1], genverts[i + 2]);
					if ((currentpoint.x <= rightbound) && (currentpoint.x >= leftbound) &&
						(currentpoint.z <= frontbound) && (currentpoint.z >= backbound) 
						) {
						indices_in_box.push_back(i);
					}
				}
			}
			else {	// If not drawing a box, check the normal ray intersections
				boxdraw = false;
				// First check for triangle intersection
				// Check 2 triangles at a time
				vertex_selected = false;
				triangle_selected = false;
				for (int i = 0; i < genverts.size(); i += 5) {
					glm::vec3 currentpoint = glm::vec3(genverts[i], genverts[i + 1], genverts[i + 2]);
					double distance = glm::distance(camerapos, glm::vec4(currentpoint, 1.0f));
					if ( ray.intersectWithPoint(currentpoint, distance)){
						vertex_selected = true;
						selected_vertex = glm::vec4(currentpoint, 1.0f);
						selected_index = i;
						break;
					}
				}

				if (!vertex_selected) {
					for (int i = 0; i < indices.size(); i += 6) {
						glm::vec3 lt_0 = glm::vec3(genverts[indices[i] * 5], genverts[indices[i] * 5 + 1], genverts[indices[i] * 5 + 2]);
						glm::vec3 lt_1 = glm::vec3(genverts[indices[i + 1] * 5], genverts[indices[i + 1] * 5 + 1], genverts[indices[i + 1] * 5 + 2]);
						glm::vec3 lt_2 = glm::vec3(genverts[indices[i + 2] * 5], genverts[indices[i + 2] * 5 + 1], genverts[indices[i + 2] * 5 + 2]);
						if (ray.intersectWithTriangle(lt_0, lt_1, lt_2)) {
							triangle_selected = true;
							selected_triangle = i;
							break;
						}
						
						glm::vec3 rt_0 = glm::vec3(genverts[indices[i + 3] * 5], genverts[indices[i + 3] * 5 + 1], genverts[indices[i + 3] * 5 + 2]);
						glm::vec3 rt_1 = glm::vec3(genverts[indices[i + 4] * 5], genverts[indices[i + 4] * 5 + 1], genverts[indices[i + 4] * 5 + 2]);
						glm::vec3 rt_2 = glm::vec3(genverts[indices[i + 5] * 5], genverts[indices[i + 5] * 5 + 1], genverts[indices[i + 5] * 5 + 2]);
						if (ray.intersectWithTriangle(rt_0, rt_1, rt_2)) {
							triangle_selected = true;
							selected_triangle = i + 3;
							break;
						}
					}
				}
			}

		}

		// adjust selected vertex
		if (vertex_selected | triangle_selected | boxdraw) {
			if (boxdraw) {
				for (int i = 0; i < indices_in_box.size(); i++) {
					genverts[indices_in_box[i]] += moveX;
					genverts[indices_in_box[i] + 1] += moveY;
					genverts[indices_in_box[i] + 2] += moveZ;
				}
			}
			else if (triangle_selected) {
				genverts[indices[selected_triangle] * 5] += moveX;
				genverts[indices[selected_triangle] * 5 + 1] += moveY;
				genverts[indices[selected_triangle] * 5 + 2] += moveZ;

				genverts[indices[selected_triangle + 1] * 5] += moveX;
				genverts[indices[selected_triangle + 1] * 5 + 1] += moveY;
				genverts[indices[selected_triangle + 1] * 5 + 2] += moveZ;

				genverts[indices[selected_triangle + 2] * 5] += moveX;
				genverts[indices[selected_triangle + 2] * 5 + 1] += moveY;
				genverts[indices[selected_triangle + 2] * 5 + 2] += moveZ;

			}
			else {
				genverts[selected_index] += moveX;
				genverts[selected_index + 1] += moveY;
				genverts[selected_index + 2] += moveZ;

				selected_vertex.x = genverts[selected_index];
				selected_vertex.y = genverts[selected_index + 1];
				selected_vertex.z = genverts[selected_index + 2];
			}

			// Modify the vertex data
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * genverts.size(), genverts.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		GLint selectedvertexLoc = textureProgram.getUniformLocation("selected_vertex");
		glUniform4fv(selectedvertexLoc, 1, glm::value_ptr(selected_vertex));

		// Render app
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Turn on wireframe
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		for (auto it = texture_list.begin(); it != texture_list.end(); ++it) {
			if (it->second.state) {
				glBindTexture(GL_TEXTURE_2D, it->first); //bind the texture
			}
		}

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, genverts.size());
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		if (boxdraw) {
			cubeProgram.useProgram();

			GLint viewLoc = cubeProgram.getUniformLocation("view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			GLint projectionLoc = cubeProgram.getUniformLocation("projection");
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			GLint modelLoc = textureProgram.getUniformLocation("model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(scaled_box_model));

			glBindVertexArray(CUBEVAO);
			//glDrawArrays(GL_TRIANGLES, 0, genverts.size());
			glDrawElements(GL_LINES, cube_indices.size(), GL_UNSIGNED_INT, 0); //glDrawElements for indices, glDrawArrays for vertices
			glBindVertexArray(0);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Turn off wireframe before rendering gui

		// Render gui
        ImGui::Render();

        SDL_GL_SwapWindow(window);
    }

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteVertexArrays(1, &CUBEVAO);
	glDeleteBuffers(1, &CUBEVBO);
	glDeleteBuffers(1, &CUBEEBO);

    // Cleanup
    ImGui_ImplSdlGL3_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
