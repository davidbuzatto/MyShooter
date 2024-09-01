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
        .radius = 0.2f,
        .color = color,
        .hDistance = 0.0f,
        .vDistance = 0.0f,
        .hAngle = 0.0f,
        .vAngle = 0.0f
    };

    return bullet;

}

void drawBullet( Bullet *bullet ) {
    DrawSphere( bullet->pos, bullet->radius, bullet->color );
}
