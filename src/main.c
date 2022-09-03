#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <file_ops.h>

#include "data.h"

unsigned int VAO[3], VBO[3], shader_program;


void
framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void
set_data(void)
{
	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);

	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(first_square), first_square,
			GL_STATIC_DRAW);

	//position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float),
			(void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(second_square), second_square,
			GL_STATIC_DRAW);

	//position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float),
			(void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);


	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wall_floor), wall_floor,
			GL_STATIC_DRAW);

	//position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glEnableVertexAttribArray(0);
	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float),
			(void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);


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
main(int argc, char **argv)
{
	if (argc < 2) {
		puts("ERROR: Specify mass difference");
		return 1;
	}

	float mass_difference = (float)atoi(argv[1]);
	if (!mass_difference) {
		puts("ERROR: Invalid mass difference");
		return 1;
	}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 800,
			"pi-collision-calculator", 0, 0);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	set_data();


	float velocity[] = {0, -.003}, aux_velocity[2],
		offset[] = {-.3, .5};
	unsigned short col_count = 0;
	char title[32];

	int offset_location = glGetUniformLocation(shader_program, "offset");

	while (!glfwWindowShouldClose(window)) {
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);

		sprintf(title, "pi-collision-calculator: %u", col_count);
		glfwSetWindowTitle(window, title);

		if (offset[0]+.2 >= offset[1]) {
			// First square collision with second square
			aux_velocity[0] = velocity[0]; aux_velocity[1] = velocity[1];

			velocity[0] =
				(1-mass_difference)/(mass_difference+1)*
				aux_velocity[0]+((2*mass_difference)/
				(mass_difference+1))*aux_velocity[1];

			velocity[1] =
				(mass_difference-1)/(mass_difference+1)*
				aux_velocity[1]+(2/(mass_difference+1))*
				aux_velocity[0];
			++col_count;
		} else if (offset[0] <= -.8) {
			// Second square collision with wall
			velocity[0] = -(velocity[0]);
			++col_count;
		}

		offset[0] += velocity[0];
		offset[1] += velocity[1];

		glClearColor(.2, .3, .3, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		if (offset[1] <= -.6) // fast collisions
			continue; // skip drawing and waiting

		glBindVertexArray(VAO[0]);

		glUniform1f(offset_location, offset[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


		glBindVertexArray(VAO[1]);

		glUniform1f(offset_location, offset[1]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


		glBindVertexArray(VAO[2]);

		glUniform1f(offset_location, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 6);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(3, VAO);
	glDeleteBuffers(3, VBO);
	glDeleteProgram(shader_program);

	glfwTerminate();
}
