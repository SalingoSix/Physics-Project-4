#version 400

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 fTangent;		// For bump (or normal) mapping
in vec3 fBitangent;

out vec4 FragColor;

void main()
{	
	FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}