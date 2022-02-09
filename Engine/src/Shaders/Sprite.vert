
// Request GLSL 3.3
#version 330

// Uniforms are global variable that typically stay the same between numerous invocations of the shader program
// 4xt matrices are needed for a 3D space with homogeneous coordinates
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// Vertex attributes for each vertex
// Corresponds to the data stored for each vertex in the vertex buffer (vertex attributes)
// Attribute 0 is position, 1 is tex coords
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

// The fragment shader needs to know the texture coordinates to determine the color
// at the pixel
out vec2 fragTexCoord;

void main()
{
	vec4 pos = vec4(inPosition, 1.0);
	gl_Position = pos * uWorldTransform * uViewProj;

	// Pass along the texture coordinates to frag shader
	fragTexCoord = inTexCoord;
}