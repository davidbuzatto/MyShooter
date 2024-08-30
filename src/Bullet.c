#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "Types.h"
#include "GameWorld.h"
#include "Enemy.h"
#include "Block.h"
#include "Bullet.h"
#include "raylib.h"

Bullet createBullet( Vector3 pos, Color color ) {

    Bullet bullet = {
        .id = entityIdCounter++,
        .pos = pos,
        .radius = 0.05f,
        .color = color,
        .collided = false
    };

    return bullet;

}

void drawBullet( Bullet *bullet ) {
    DrawSphere( bullet->pos, bullet->radius, bullet->color );
}
