
// Request GLSL 3.3
#version 330

// Vertex attributes for each vertex
// Corresponds to the data stored for each vertex in the vertex buffer (vertex attributes)
in vec3 inPosition;

void main()
{
	// The vertex shader outputs a 4D coordinate
	// Assume, for now, that w is 1
	gl_Position = vec4(inPosition, 1.0);
}