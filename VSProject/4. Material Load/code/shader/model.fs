#version 430 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D diffuseMaps[8];

uniform vec3 viewPos;


void main()
{

	FragColor =  texture(diffuseMaps[0], TexCoord);

}