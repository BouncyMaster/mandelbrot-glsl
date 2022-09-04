#include <stdio.h>
#include <limits.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <file_ops.h>

#include "data.h"

unsigned int VAO, VBO, shader_program;
double cx = 0, cy = 0, zoom = 1;
int itr = 400, scr[2] = {800, 800};

void
framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void
scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom += yoffset * 0.1 * zoom;
	if(zoom < 0.1) {
		zoom = 0.1;
	}
}

void
keyboard_callback(GLFWwindow* window)
{
	const double d = 0.02 / zoom;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cy += d;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cy -= d;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cx -= d;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cx += d;

	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS &&
			itr < (INT_MAX - 10))
		itr += 10;
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS && itr > 10)
		itr -= 10;
}

void
set_data(void)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_square), screen_square,
			GL_STATIC_DRAW);

	//position
	glVertexAttribPointer(0, 2, GL_BYTE, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	const char *vertex_shader_source = file_to_str("shaders/vertex.glsl"),
		*fragment_shader_source = file_to_str("shaders/fragment.glsl");

	unsigned int vertex_shader, fragment_shader;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	file_to_str_free(vertex_shader_source);
	file_to_str_free(fragment_shader_source);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program); 

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	glUseProgram(shader_program);
}

int
main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(scr[0], scr[1], "mandelbrot", 0, 0);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	set_data();

	char title[100];
	float cur_frame, last_frame = 0, frame;

	int scrsize_loc = glGetUniformLocation(shader_program, "screen_size");
	int scrratio_loc = glGetUniformLocation(shader_program, "screen_ratio");
	int center_loc = glGetUniformLocation(shader_program, "center");
	int zoom_loc = glGetUniformLocation(shader_program, "zoom");
	int itr_loc = glGetUniformLocation(shader_program, "itr");

	while (!glfwWindowShouldClose(window)) {
		keyboard_callback(window);

		glfwGetWindowSize(window, &scr[0], &scr[1]);

		cur_frame = glfwGetTime();
		frame = (cur_frame - last_frame) * 1000;
		last_frame = cur_frame;

		sprintf(title, "mandelbrot (%5.1fms, itr: %d, zoom: %.2f, pos: [%.2f %.2f])",
			frame, itr, zoom, cx, cy);
		glfwSetWindowTitle(window, title);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUniform2d(scrsize_loc, (double)scr[0], (double)scr[1]);
		glUniform1d(scrratio_loc, (double)scr[0] / (double)scr[1]);
		glUniform2d(center_loc, cx, cy);
		glUniform1d(zoom_loc, zoom);
		glUniform1i(itr_loc, itr);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader_program);

	glfwTerminate();
}
