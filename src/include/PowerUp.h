#pragma once

#include "Types.h"
#include "raylib.h"

PowerUp createPowerUp( Vector3 pos, PowerUpType powerUpType );
void drawPowerUp( PowerUp *powerUp );
void updatePowerUp( PowerUp *powerUp, float delta );
void jumpPowerUp( PowerUp *powerUp );
PowerUpCollisionType checkCollisionPowerUpBlock( PowerUp *powerUp, Block *block );
BoundingBox getPowerUpBoundingBox( PowerUp *powerUp );
void createPowerUps( GameWorld *gw, Vector3 *positions, PowerUpType *types, int powerUpQuantity );
void createPowerUpsModel( PowerUp *powerUps, int powerUpQuantity );
void cleanConsumedPowerUps( GameWorld *gw );
