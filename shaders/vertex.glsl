#version 460 core
in ivec2 aPos;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
}
