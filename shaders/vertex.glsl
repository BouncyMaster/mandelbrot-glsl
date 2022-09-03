#version 460 core
in vec2 aPos;

uniform float offset;

void main()
{
	gl_Position = vec4(aPos.x+offset, aPos.y, 0.0, 1.0);
}
