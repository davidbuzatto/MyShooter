#pragma once

#include "raylib/raylib.h"

typedef struct ExplosionBillboard {
    Vector3 pos;
    Texture2D textures[3];
    int frameCount;
    int currentFrame;
    float frameTimeCounter;
    float timeToNextFrame;
    bool finished;
} ExplosionBillboard;

ExplosionBillboard createExplosionBillboard( Vector3 pos );
void updateExplosionBillboard( ExplosionBillboard *eb, float delta );
void drawExplosionBillboard( ExplosionBillboard *eb, Camera camera );