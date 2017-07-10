#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec4 vertexColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 selected_vertex;

void main()
{
	vec4 worldcoords = model * vec4(position, 1.0f); 
    gl_Position = projection * view * worldcoords;
	if ( worldcoords == selected_vertex)	{
		vertexColor = vec4(1.0,1.0,1.0,1.0);
	}
	else {
		vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
	}

	TexCoord = texCoord;
}