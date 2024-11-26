#define GLAD_GL_IMPLEMENTATION

#include "render.h"
#include "logs.h"
#include "shader.h"
#include <cglm/cglm.h>

uint32_t render_create_shader(void) {
	uint32_t vertex_shader =
		compile_shader("shaders/default.vert", GL_VERTEX_SHADER);
	if (!vertex_shader) {
		return 0;
	}

	uint32_t fragment_shader =
		compile_shader("shaders/default.frag", GL_FRAGMENT_SHADER);
	if (!fragment_shader) {
		return 0;
	}

	uint32_t shader_program =
		compile_shader_program(vertex_shader, fragment_shader);
	if (!shader_program) {
		return 0;
	}

	// После линкинга в шейдерной программе они больше не нужны
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

uint32_t render_create_vao(Mesh *mesh) {
	FloatVector *vertices = &mesh->vertices;
	UInt32Vector *indices = &mesh->indices;

	// WARNING: A DUNGER STARTS!
	uint32_t VAO; // Vertex Attribute Object(?)
	uint32_t VBO; // Vertex Buffer Object
	uint32_t EBO; // Element (Index) Buffer Object

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->capacity * sizeof(float),
				 vertices->data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->capacity * sizeof(float),
				 indices->data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
						  (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
						  (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
						  (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	// WARNING: THE DUNGER ENDS!

	return VAO;
}

void pre_render(int width, int height) {
	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void render(Renderer *renderer, const MeshVector *meshes,
			const UInt32Vector *VAOs, Player *player, Camera *camera) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (*renderer->polygon_mode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	mat4 projection;
	glm_perspective_default((float)*renderer->width / (float)*renderer->height,
							projection);
	int projection_location =
		glGetUniformLocation(renderer->shader_program, "projection");

	int model_location =
		glGetUniformLocation(renderer->shader_program, "model");

	mat4 view;
	glm_mat4_identity(view);
	vec3 up = {1.0f, 0.0f, 0.0f};
	glm_rotate(view, -camera->direction->y, up);
	vec3 left = {0.0f, 1.0f, 0.0f};
	glm_rotate(view, -camera->direction->x, left);
	int view_location = glGetUniformLocation(renderer->shader_program, "view");

	glUseProgram(renderer->shader_program);
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection[0]);
	glUniformMatrix4fv(view_location, 1, GL_FALSE, view[0]);

	for (size_t i = 0; i < VAOs->size; i++) {
		if (!meshes->data[i].visible)
			continue;
		mat4 model;
		vec4 translation = {player->position.x, -player->position.y,
							player->position.z, 0.0f};
		glm_mat4_identity(model);
		glm_translate(model, translation);
		glUniformMatrix4fv(model_location, 1, GL_FALSE, model[0]);
		glBindVertexArray(VAOs->data[i]);
		glDrawElements(GL_TRIANGLES, meshes->data[i].indices.size,
					   GL_UNSIGNED_INT,
					   0); // With EBO

		glBindVertexArray(0);
	}
}

int loadGL(GLADloadfunc func) {
	int version = gladLoadGL(func);
	if (version == 0) {
		error("Failed to initialize OpenGL context.");
		return -1;
	}
	info("OpenGL loaded successfully.");
	return 0;
}