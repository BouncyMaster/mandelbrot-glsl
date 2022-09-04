#version 460 core

out vec4 colorOut;
uniform double screen_ratio;
uniform dvec2 screen_size;
uniform dvec2 center;
uniform double zoom;
uniform int itr;

void main()
{
	dvec2 z, c;
	c.x = screen_ratio * (gl_FragCoord.x / screen_size.x - 0.5);
	c.y = (gl_FragCoord.y / screen_size.y - 0.5);

	c.x /= zoom;
	c.y /= zoom;

	c.x += center.x;
	c.y += center.y;

	int i;
	for(i = 0; i < itr; i++) {
		double x = (z.x * z.x - z.y * z.y) + c.x;
		double y = (z.y * z.x + z.x * z.y) + c.y;

		if((x * x + y * y) > 2.0) break;
		z.x = x;
		z.y = y;
	}

	float t = float(i) / float(itr);

	colorOut = vec4(9.0 * (1.0 - t) * t * t * t,
			15.0 * (1.0 - t) * (1.0 - t) * t * t,
			8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t,
			1);
}