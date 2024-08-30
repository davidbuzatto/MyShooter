#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "Types.h"
#include "GameWorld.h"
#include "EnemyBullet.h"
#include "Block.h"
#include "Bullet.h"
#include "raylib.h"

EnemyBullet createEnemyBullet( Vector3 pos, Color color ) {

    EnemyBullet bullet = {
        .pos = pos,
        .drawPos = pos,
        //.radius = 0.05f,
        //.color = color,
        .radius = 0.2f,
        .color = WHITE,
        .hDistance = 0.0f,
        .vDistance = 0.0f,
        .hAngle = 0.0f,
        .vAngle = 0.0f
    };

    return bullet;

}

void drawEnemyBullet( EnemyBullet *bullet ) {
    DrawSphere( bullet->drawPos, bullet->radius, bullet->color );
}
