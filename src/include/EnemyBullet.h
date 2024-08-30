#pragma once

#include <stdbool.h>

#include "Types.h"
#include "raylib.h"

EnemyBullet createEnemyBullet( Vector3 pos, Color color );
void drawEnemyBullet( EnemyBullet *bullet );
