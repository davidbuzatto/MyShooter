#pragma once

struct GameWorld;

#include "GameWorld.h"
#include "raylib/raylib.h"

typedef enum PowerUpType {
    POWER_UP_TYPE_HP,
    POWER_UP_TYPE_AMMO
} PowerUpType;

typedef enum PowerUpCollisionType {
    POWER_UP_COLLISION_ALL,
    POWER_UP_COLLISION_NONE
} PowerUpCollisionType;

typedef enum PowerUpPositionState {
    POWER_UP_POSITION_STATE_ON_GROUND,
    POWER_UP_POSITION_STATE_JUMPING,
    POWER_UP_POSITION_STATE_FALLING
} PowerUpPositionState;

typedef enum PowerUpState {
    POWER_UP_STATE_ACTIVE,
    POWER_UP_STATE_CONSUMED
} PowerUpState;

typedef struct PowerUp {

    int id;
    
    Vector3 pos;
    Vector3 lastPos;
    float radius;
    Vector3 vel;

    float jumpSpeed;

    Color hpColor;
    Color ammoColor;
    
    bool showWiresOnly;

    Model model;
    Vector3 rotationAxis;
    float rotationHorizontalAngle;
    float rotationVel;
    float rotationSpeed;
    Vector3 scale;

    PowerUpType type;
    PowerUpState state;
    PowerUpPositionState positionState;

} PowerUp;

PowerUp createPowerUp( Vector3 pos, PowerUpType powerUpType );
void drawPowerUp( PowerUp *powerUp );
void updatePowerUp( PowerUp *powerUp, float delta );
void jumpPowerUp( PowerUp *powerUp );
PowerUpCollisionType checkCollisionPowerUpBlock( PowerUp *powerUp, Block *block );
BoundingBox getPowerUpBoundingBox( PowerUp *powerUp );
void createPowerUps( struct GameWorld *gw, Vector3 *positions, PowerUpType *types, int powerUpQuantity );
void createPowerUpsModel( PowerUp *powerUps, int powerUpQuantity );
void cleanConsumedPowerUps( struct GameWorld *gw );
