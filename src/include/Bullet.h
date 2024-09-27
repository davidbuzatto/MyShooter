#pragma once

#include <stdbool.h>

#include "raylib/raylib.h"

typedef struct Bullet {
    int id;
    Vector3 pos;
    float radius;
    Color color;
    float hDistance;
    float vDistance;
    float hAngle;
    float vAngle;
} Bullet;

Bullet createBullet( Vector3 pos, Color color, float radius );
void drawBullet( Bullet *bullet );
