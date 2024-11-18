#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "logs.h"
#include "player.h"
#include "render.h"
#include "shader.h"
#include "utils.h"
#include "window.h"
#include "world.h"

int main() {
	Window window = createWindow(800, 600);
	if (initWindow(&window) != 0) {
		error("The window was not initialized.");
		glfwTerminate();
		return -1;
	}

	int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
      error("Failed to initialize OpenGL context.");
      return -1;
  }
  info("OpenGL loaded successfully.");

	unsigned int shader_program = render_create_shader();

	struct World world = world_init();

	Player player = {
		{0.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 5.0f,
		{0.0f, 0.0f, 0.0f},
	};
	Camera camera = {&player.position, &player.direction};
	Renderer renderer = {
		&window.polygon_mode,
		shader_program,
		&window.width,
		&window.height,
	};

	pre_render(window.width, window.height);

	info("The program has been fully initialized. Starting the game loop...");

	float start = 0.0f;
	float end = 0.0f;
	float dt = 0.0f;
	double lastX = window.width / 2.0f;
	double lastY = window.height / 2.0f;
	while (!windowShouldClose(&window)) {
		dt = end - start;
		start = glfwGetTime();

		// LOGIC
		glfwPollEvents();

		// Mouse rotate camera
		double xpos, ypos;
		getCursorPos(&window, &xpos, &ypos);
		float dx = (xpos - lastX);
		float dy = (ypos - lastY);
		lastX = xpos;
		lastY = ypos;

		camera_update(&camera, window.keys, dx, dy, dt);
		player_move(&player, window.keys, dt);

		// RENDER
		render(&renderer, &world, &player, &camera);
		swapBuffer(&window);

		end = glfwGetTime();
	}
	world_free(&world);
	glfwTerminate();
	info("The program has terminated.");

	return 0;
}

// TODO: optimize Mesh generating. Сейчас мы генерируем по 4 вершины на каждую
// грань вокселя. Это 24 вершины на воксель в худшем случае. Но воксель это куб
// и можно обойтись 8 вершинами
// TODO: add ability to change color of voxel.
// TODO: write a blazingly fast greedy mesher xd lol
// TODO: a system of loading-unloading chunks depending on the distance from the
// player
// TODO: world generation
// TODO: improve camera rotation (use quaternions O.O)
