
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
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// The fragment shader needs to know the texture coordinates to determine the color
// at the pixel
out vec2 fragTexCoord;
// The camera position and the directional light's direction are in world space
// But gl_Position is in clip space
// Getting the correct vector from the surface to the camera requires a position in world space
// The input vertex normals also need to be in world space

// Normal (in world space)
out vec3 fragNormal;
// Position (in world space)
out vec3 fragWorldPos;

void main()
{
	// Convert position to homogeneous coordinates
	vec4 pos = vec4(inPosition, 1.0);
	// Transform position to world space
	pos = pos * uWorldTransform;
	// Save world position
	// .xyz is called swizzle
	fragWorldPos = pos.xyz;
	// Transform to clip space
	gl_Position = pos * uViewProj;

	// Transform normal into world space
	// w = 0 because the normal is not a position, so translating it does not make sense
	// If w is 0 the translation component of the world transform matrix zeros out in the multiplication
	fragNormal = (vec4(inNormal, 0.0f) * uWorldTransform).xyz;

	// Pass along the texture coordinates to frag shader
	fragTexCoord = inTexCoord;
}