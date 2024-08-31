#pragma once

#include <stdbool.h>

#include "Types.h"
#include "ResourceManager.h"
#include "raylib.h"

Player createPlayer( Vector3 pos );
void drawPlayer( Player *player );
void drawPlayerHud( Player *player );
void updatePlayer( Player *player, float delta );
void updatePlayerCollisionProbes( Player *player );
void jumpPlayer( Player *player );
PlayerCollisionType checkCollisionPlayerBlock( Player *player, Block *block, bool checkCollisionProbes );
PlayerCollisionType checkCollisionPlayerEnemy( Player *player, Enemy *enemy, bool checkCollisionProbes );
PlayerCollisionType checkCollisionPlayerPowerUp( Player *player, PowerUp *powerUp );
BoundingBox getPlayerBoundingBox( Player *player );
void createPlayerModel( Player *player );

void playerShotBullet( GameWorld *gw,Player *player, IdentifiedRayCollision *irc, Color bulletColor );
void cleanCollidedBullets( Player *player );

void playerAcquirePowerUp( Player *player, PowerUp *powerUp );
Ray getPlayerToEnemyRay( Player *player, Enemy *enemy );
Ray getPlayerToVector3Ray( Player *player, Vector3 v3 );
