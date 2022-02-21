#version 330

// Any out variables in the vertex shader need a corresponding in variable in the fragment shader
// Must have the same name and type
// Tex coord
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;

// Output color to the color buffer
out vec4 outColor;

// Texture sampler (that can get a color from a texture given a texture coordinate
// You don't need any code to bind this sampler uniform, because we're currently binding
// only one texture at a time, so OpenGL automatically knows that the only texture sampler
// in the shader corresponds to the active texture
uniform sampler2D uTexture;

struct DirectionalLight 
{
	// Direction of light
	vec3 m_Direction;
	// Diffuse Color
	vec3 m_DiffuseColor;
	// Specular Color
	vec3 m_SpecColor;
};

// Uniforms for lighting
// Camera position (in world space)
uniform vec3 uCameraPos;
// Specular power for this surface
uniform float uSpecPower;
// Ambient light level
uniform vec3 uAmbientLight;

// Directional Light
uniform DirectionalLight uDirLight;

void main() 
{
	// Surface normal (need to normalize because OpenGL interpolates the vertex normal across the face of the triangle
	// Interpolating two normalized vectors does not guarantee a normalized vector at each step of the interpolation
	vec3 N = normalize(fragNormal);
	// Vector from surface to light
	vec3 L = normalize(-uDirLight.m_Direction);
	// Vector from surface to camera
	vec3 V = normalize(uCameraPos - fragWorldPos);
	// Reflection of -L about N
	vec3 R = normalize(reflect(-L, N));

	// Compute Phong reflection
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		vec3 Diffuse = uDirLight.m_DiffuseColor * NdotL;
		// If dot(R, V) ends up negative, light from the scene will end up being removed
		vec3 Specular = uDirLight.m_SpecColor * pow(max(0.0, dot(R, V)), uSpecPower);
		Phong += Diffuse + Specular;
	}

	// Final color is texture color times phong light (alpha = 1)
	outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}
