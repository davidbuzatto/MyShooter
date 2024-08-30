#pragma once

#include <stdbool.h>

#include "Types.h"
#include "raylib.h"

Bullet createBullet( Vector3 pos, Color color );
void drawBullet( Bullet *bullet );
