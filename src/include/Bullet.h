#pragma once

#include <stdbool.h>

#include "Types.h"
#include "raylib.h"

Bullet createBullet();
void drawBullet( Bullet *bullet );
void updateBullet( Bullet *bullet, float delta );
bool checkCollisionBulletBlock( Bullet *bullet, Block *block );
bool checkCollisionBulletEnemy( Bullet *bullet, Enemy *enemy );
