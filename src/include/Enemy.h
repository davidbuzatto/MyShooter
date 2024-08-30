#pragma once

#include <stdbool.h>

#include "Types.h"
#include "raylib.h"

Enemy createEnemy( Vector3 pos, Color color, Color eyeColor );
void drawEnemy( Enemy *enemy );
void drawEnemyHpBar( Enemy *enemy, Camera3D camera );
void updateEnemy( Enemy *enemy, Player *player, float delta );
void updateEnemyCollisionProbes( Enemy *enemy );
void jumpEnemy( Enemy *enemy );
EnemyCollisionType checkCollisionEnemyBlock( Enemy *enemy, Block *block, bool checkCollisionProbes );
BoundingBox getEnemyBoundingBox( Enemy *enemy );
void createEnemies( GameWorld *gw, Vector3 *positions, int enemyQuantity, Color color, Color eyeColor );
void createEnemiesModel( Enemy *enemies, int enemyQuantity );
void setEnemyDetectedByPlayer( Enemy *enemy, Player *player, bool showLines );
void addBulletToEnemy( Enemy *enemy, Vector3 bulletPos );

void cleanDeadEnemies( GameWorld *gw );
