#include "utils.h"

#ifndef _PLAYER_H
#define _PLAYER_H
typedef struct {
	Vec3 position;
	Vec3 velocity;
	Vec3 acceleration;
	float speed;
	Vec3 direction;
} Player;

void player_move(Player *player, bool keys[1024], float dt);
#endif // _PLAYER_H
