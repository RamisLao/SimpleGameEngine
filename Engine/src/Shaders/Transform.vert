
// Request GLSL 3.3
#version 330

// Uniforms are global variable that typically stay the same between numerous invocations of the shader program
// 4xt matrices are needed for a 3D space with homogeneous coordinates
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// Vertex attributes for each vertex
// Corresponds to the data stored for each vertex in the vertex buffer (vertex attributes)
in vec3 inPosition;

void main()
{
	vec4 pos = vec4(inPosition, 1.0);
	gl_Position = pos * uWorldTransform * uViewProj;
}