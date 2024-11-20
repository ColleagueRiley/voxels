#define _VECTOR_IMPL
#include "world.h"
#include "chunk.h"
#include "render.h"
#include "utils.h"

VectorImpl(Chunk, ChunkVector)

	struct World world_init(void) {
	struct World world = {ChunkVector_init(0, 64)};

	for (int i = 0; i < 4; i++) {
		unsigned int x = i % 2 - 1;
		unsigned int z = i / 2 - 1;
		world_chunk_generate(&world, x, 0, z);
	}

	// for (unsigned int i = 0; i < world.chunks.size; i++) {
	// 	world_mesh_generate(&world, i);
	// }

	return world;
}

void world_chunk_generate(struct World *world, int x, int y, int z) {
	ChunkVector_append(&world->chunks, chunk_init(x, y, z));
	unsigned int index = world->chunks.size - 1;
	Chunk *chunk = &world->chunks.data[index];

	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				chunk_set(chunk, x, y, z, 1);
			}
		}
	}
}

// void world_mesh_generate(World *world, unsigned int index) {
// 	world->chunks.data[index].mesh = chunk_genmesh(&world->chunks.data[index]);
// 	world->chunks.data[index].VAO =
// 		render_create_vao(&world->chunks.data[index].mesh);
// }

Chunk *world_chunk_get(const struct World *world, int x, int y, int z) {
	for (unsigned int i = 0; i < world->chunks.size; i++) {
		Chunk *chunk = &world->chunks.data[i];
		if (chunk->position.x == x && chunk->position.y == y &&
			chunk->position.z == z) {
			return chunk;
		}
	}
	return NULL;
}

static int mod(int a, int b) {
	int r = a % b;
	return r < 0 ? r + b : r;
}

unsigned char world_block_get(const struct World *world, int x, int y, int z) {
	unsigned int blockX = mod(x, 32);
	unsigned int blockY = mod(y, 32);
	unsigned int blockZ = mod(z, 32);
	int chunkX = floorf((float)x / 32);
	int chunkY = floorf((float)y / 32);
	int chunkZ = floorf((float)z / 32);
	return chunk_get(world_chunk_get(world, chunkX, chunkY, chunkZ), blockX,
					 blockY, blockZ);
}

UnsignedIntVector world_chunk_circle(const struct World *world, int x, int y,
									 int z, int radius) {
	UnsignedIntVector chunk_indices = UnsignedIntVector_init(0, 64);

	for (unsigned int i = 0; i < world->chunks.size; i++) {
		const Vec3i *chunkPos = &world->chunks.data[i].position;
		int dx = chunkPos->x - x;
		int dy = chunkPos->y - y;
		int dz = chunkPos->z - z;
		float dist = sqrtf(dx * dx + dy * dy + dz * dz);
		if (dist < radius) {
			UnsignedIntVector_append(&chunk_indices, i);
		}
	}

	return chunk_indices;
}

void world_free(struct World *world) {
	for (unsigned int i = 0; i < world->chunks.size; i++)
		chunk_free(&world->chunks.data[i]);
	// mesh_free(&world->chunks.data->mesh);
}
