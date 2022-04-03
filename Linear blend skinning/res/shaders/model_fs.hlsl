#version 430 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;

uniform vec3 viewPos;


void main()
{

	FragColor = vec4(1.0f, 0.5f, 0.31f, 1.0f);


}