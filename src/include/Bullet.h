#pragma once

#include <stdbool.h>

#include "Types.h"
#include "raylib/raylib.h"

Bullet createBullet( Vector3 pos, Color color, float radius );
void drawBullet( Bullet *bullet );
