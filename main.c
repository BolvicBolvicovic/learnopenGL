#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define WIN_WIDTH					1024
#define WIN_HEIGHT					768
#define APP_NAME					("Learn")
#define MAX_SHADER_SIZE				0x2000

#define VEC3						3
#define VEC3_FLOAT_STRIDE			(VEC3 * sizeof(float))
#define LOCATION_VERTEX_ATTR_POS	0
#define LOCATION_VERTEX_ATTR_COLOR	1

static inline uint32_t
compile_shader_program(const char* vertex_path, const char* fragment_path)
{
	int			vertex_file		= open(vertex_path, O_RDONLY);
	int			fragment_file	= open(fragment_path, O_RDONLY);
	char		vertex_buffer[MAX_SHADER_SIZE]		= {0};
	char		fragment_buffer[MAX_SHADER_SIZE]	= {0};
	int			read_vertex_file					= read(vertex_file, vertex_buffer, MAX_SHADER_SIZE);
	int			read_fragment_file					= read(fragment_file, fragment_buffer, MAX_SHADER_SIZE);

	close(vertex_file);
	close(fragment_file);

	if (read_vertex_file == -1 || read_fragment_file == -1)
	{
		printf("Error: opening or reading shaders failed\n");
		return -1;
	}

	uint32_t	vertex_shader		= glCreateShader(GL_VERTEX_SHADER);
	uint32_t	fragment_shader		= glCreateShader(GL_FRAGMENT_SHADER);
	const char*	vertex_buffer_ptr	= (const char*)vertex_buffer;
	const char*	fragment_buffer_ptr	= (const char*)fragment_buffer;

	// Note: Set source
	glShaderSource(vertex_shader, 1, &vertex_buffer_ptr, 0);
	glShaderSource(fragment_shader, 1, &fragment_buffer_ptr, 0);

	// Note: Compile shaders
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	int		success = 0;
#define LOGS_BUF_SIZE	512
	char	logs[LOGS_BUF_SIZE] = {0};

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, LOGS_BUF_SIZE, 0, logs);
		printf("Error: Vertex shader compilation failed: %s\n", logs);
		return -1;
	}

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, LOGS_BUF_SIZE, 0, logs);
		printf("Error: Fragment shader compilation failed: %s\n", logs);
		return -1;
	}

	// Note: Link shader program
	uint32_t	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader_program, LOGS_BUF_SIZE, 0, logs);
		printf("Error: Shader program linkage failed: %s\n", logs);
		return -1;
	}

	// Note: Clean objects
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

// Note: Resize framebuffer callback on window resize
static void
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

// Note: Input handler
static void
process_input(GLFWwindow* window)
{
	// Note: Press Escape to close window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, 1);
	}
}

static GLFWwindow*
create_window(void)
{
	// Note: Init glfw and window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow*	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, APP_NAME, 0, 0);

	if (!window)
	{
		printf("Failed to create GLFW window\n");
		return 0;
	}

	glfwMakeContextCurrent(window);

	// Note: Init GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to init GLAD\n");
		return 0;
	}

	// Note: Init viewport
	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
	
	return window;
}

static inline void
register_callbacks(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Note: Register all callbacks here
}

int
main(int ac, char** av)
{
	// Note: Not taking arguments yet
	(void)ac;
	(void)av;

	GLFWwindow*	window = create_window();
	if (!window)
	{
		glfwTerminate();
		return 1;
	}

	register_callbacks(window);

	uint32_t	shader_program = compile_shader_program("shader.vert", "shader.frag");
	if (shader_program == (uint32_t)-1)
	{
		glfwTerminate();
		return 1;
	}

	float	vertices[] =
	{
		//x     y     z		colors
		 0.7,  0.7,  0.0,  1.0, 0.0, 0.0,	// top right
    	 0.7, -0.7,  0.0,  0.0, 1.0, 0.0,	// bottom right
    	 0.0, -0.0,  0.0,  0.0, 0.0, 1.0,	// center
    	-0.7, -0.7,  0.0,  0.0, 1.0, 0.0,	// bottom left
    	-0.7,  0.7,  0.0,  1.0, 0.0, 0.0	// top left
	};

	uint32_t	indices[] =
	{
		2, 1, 0,	// first triangle
		2, 3, 4		// second triangle
	};

	// Note: Init vertex array object
	uint32_t	vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Note: Init vertex buffer object
	uint32_t	vbo;
	glGenBuffers(1, &vbo);

	// Note: Init element buffer object
	uint32_t	ebo;
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set aPos
	glVertexAttribPointer(
		LOCATION_VERTEX_ATTR_POS,
		VEC3,
		GL_FLOAT,
		GL_FALSE,
		VEC3_FLOAT_STRIDE * 2,
		// Note: This is an offset that is a void* for some reason
		(void*)0
	);
	glEnableVertexAttribArray(LOCATION_VERTEX_ATTR_POS);

	// Set aColor
	glVertexAttribPointer(
		LOCATION_VERTEX_ATTR_COLOR,
		VEC3,
		GL_FLOAT,
		GL_FALSE,
		VEC3_FLOAT_STRIDE * 2,
		(void*)VEC3_FLOAT_STRIDE
	);
	glEnableVertexAttribArray(LOCATION_VERTEX_ATTR_COLOR);

	// Note: Unbind buffer and array (optional)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Note: Draw wireframe polygone (optional)
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Note: Uniforms
	int	u_Resolution	= glGetUniformLocation(shader_program, "u_Resolution");
	int	u_Time			= glGetUniformLocation(shader_program, "u_Time");

	// Note: Main loop
	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// Note: Set variable uniform here

		glUseProgram(shader_program);

		glUniform2f(u_Resolution, WIN_WIDTH, WIN_HEIGHT);
		glUniform1f(u_Time, glfwGetTime());

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Note: we could deallocate all GLFW ressources here

	glfwTerminate();
	return 0;
}
