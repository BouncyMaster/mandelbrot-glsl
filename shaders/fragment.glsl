#version 460 core
out vec4 FragColor;

uniform float offset;

void main()
{
	FragColor = vec4(offset, 0, 0, 1);
}
