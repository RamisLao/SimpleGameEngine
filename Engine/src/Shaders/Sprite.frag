
#version 330

// Any out variables in the vertex shader need a corresponding in variable in the fragment shader
// Must have the same name and type
in vec2 fragTexCoord;

// Output color to the color buffer
out vec4 outColor;

// Texture sampler (that can get a color from a texture given a texture coordinate
// You don't need any code to bind this sampler uniform, because we're currently binding
// only one texture at a time, so OpenGL automatically knows that the only texture sampler
// in the shader corresponds to the active texture
uniform sampler2D uTexture;

void main() 
{
	// Sample color from texture
	outColor = texture(uTexture, fragTexCoord);
}
