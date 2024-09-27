#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "EntitySupport.h"
#include "GameWorld.h"
#include "Enemy.h"
#include "Block.h"
#include "Bullet.h"
#include "raylib/raylib.h"

Bullet createBullet( Vector3 pos, Color color, float radius ) {

    Bullet bullet = {
        .id = entityIdCounter++,
        .pos = pos,
        .radius = radius,
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
