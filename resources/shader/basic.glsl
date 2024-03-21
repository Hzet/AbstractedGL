#vertex
#version 430 core
layout (location = 0) in vec3 vertex_position;
layout (location = 0) in vec4 vertex_color;

out vec4 color;

void main()
{
	color = vertex_color;
	gl_Position = vec4(vertex_position, 1);
}

#fragment
#version 430 core

in vec4 color;
out vec4 fragment_color;

void main()
{
	fragment_color = color;
}