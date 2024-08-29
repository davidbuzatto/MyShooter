#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "Types.h"
#include "GameWorld.h"
#include "PowerUp.h"
#include "Block.h"
#include "ResourceManager.h"
#include "raylib.h"

PowerUp createPowerUp( Vector3 pos, PowerUpType powerUpType ) {

    PowerUp powerUp = {
        .id = objectIdCounter++,
        .pos = pos,
        .lastPos = {
            .x = 0.0f,
            .y = 1.0f,
            .z = 0.0f
        },
        .radius = 0.5f,
        .vel = {
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f
        },
        .jumpSpeed = 5.0f,
        .hpColor = YELLOW,
        .ammoColor = GRAY,
        .showWiresOnly = false,

        .model = { 0 },
        .rotationAxis = { 0.0f, 1.0f, 0.0f },
        .rotationHorizontalAngle = 0.0f,
        .rotationVel = 0.0f,
        .rotationSpeed = 150.0f,
        .scale = { 1.0f, 1.0f, 1.0f },

        .type = powerUpType,
        .state = POWER_UP_STATE_ACTIVE,
        .positionState = POWER_UP_POSITION_STATE_ON_GROUND

    };

    return powerUp;

}

void drawPowerUp( PowerUp *powerUp ) {

    if ( !powerUp->showWiresOnly ) {

        Color color;

        switch ( powerUp->type ) {
            case POWER_UP_TYPE_HP: color = powerUp->hpColor; break;
            case POWER_UP_TYPE_AMMO: color = powerUp->ammoColor; break;
            default: color = BLACK; break;
        }

        DrawModelEx( powerUp->model, powerUp->pos, powerUp->rotationAxis, powerUp->rotationHorizontalAngle, powerUp->scale, color );

    }

    DrawModelWiresEx( powerUp->model, powerUp->pos, powerUp->rotationAxis, powerUp->rotationHorizontalAngle, powerUp->scale, BLACK );

}

void updatePowerUp( PowerUp *powerUp, float delta ) {

    powerUp->lastPos = powerUp->pos;

    powerUp->pos.x += powerUp->vel.x * delta;
    powerUp->pos.y += powerUp->vel.y * delta;
    powerUp->pos.z += powerUp->vel.z * delta;

    powerUp->vel.y -= GRAVITY * delta;

    if ( powerUp->pos.y < powerUp->lastPos.y ) {
        powerUp->positionState = POWER_UP_POSITION_STATE_FALLING;
    } else if ( powerUp->pos.y > powerUp->lastPos.y ) {
        powerUp->positionState = POWER_UP_POSITION_STATE_JUMPING;
    } else {
        powerUp->positionState = POWER_UP_POSITION_STATE_ON_GROUND;
    }

    powerUp->rotationHorizontalAngle += powerUp->rotationVel * delta;

}

void jumpPowerUp( PowerUp *powerUp ) {
    if ( powerUp->positionState == POWER_UP_POSITION_STATE_ON_GROUND ) {
        powerUp->vel.y = powerUp->jumpSpeed;
    }
}

PowerUpCollisionType checkCollisionPowerUpBlock( PowerUp *powerUp, Block *block ) {

    BoundingBox powerUpBB = getPowerUpBoundingBox( powerUp );
    BoundingBox blockBB = getBlockBoundingBox( block );

    if ( CheckCollisionBoxes( powerUpBB, blockBB ) ) {
        return POWER_UP_COLLISION_ALL;
    }

    return POWER_UP_COLLISION_NONE;

}

BoundingBox getPowerUpBoundingBox( PowerUp *powerUp ) {
    return (BoundingBox){
        .min = {
            .x = powerUp->pos.x - powerUp->radius,
            .y = powerUp->pos.y - powerUp->radius,
            .z = powerUp->pos.z - powerUp->radius
        },
        .max = {
            .x = powerUp->pos.x + powerUp->radius,
            .y = powerUp->pos.y + powerUp->radius,
            .z = powerUp->pos.z + powerUp->radius,
        },
    };
}

void createPowerUps( GameWorld *gw, Vector3 *positions, PowerUpType *types, int powerUpQuantity ) {

    gw->powerUpQuantity = powerUpQuantity;
    gw->powerUps = (PowerUp*) malloc( sizeof( PowerUp ) * gw->powerUpQuantity );

    for ( int i = 0; i < gw->powerUpQuantity; i++ ) {
        gw->powerUps[i] = createPowerUp( positions[i], types[i] );
    }

    createPowerUpsModel( gw->powerUps, gw->powerUpQuantity );

}

void createPowerUpsModel( PowerUp *powerUps, int powerUpQuantity ) {

    if ( !rm.powerUpModelCreated ) {

        PowerUp *basePowerUp = &powerUps[0];

        Mesh mesh = GenMeshSphere( basePowerUp->radius, 10, 10 );
        Model model = LoadModelFromMesh( mesh );

        Image img = GenImageChecked( 2, 2, 1, 1, WHITE, LIGHTGRAY );
        Texture2D texture = LoadTextureFromImage( img );
        UnloadImage( img );

        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

        rm.powerUpModel = model;
        rm.powerUpModelCreated = true;

    }

    for ( int i = 0; i < powerUpQuantity; i++ ) {
        powerUps[i].model = rm.powerUpModel;
    }

}

void cleanConsumedPowerUps( GameWorld *gw ) {

    // naive algorithm
    int consumedCount = 0;
    PowerUp *powerUps = gw->powerUps;

    for ( int i = 0; i < gw->powerUpQuantity; i++ ) {
        if ( powerUps[i].state == POWER_UP_STATE_CONSUMED ) {
            consumedCount++;
        }
    }

    int *collectedIds = (int*) malloc( consumedCount * sizeof( int ) );
    int t = 0;

    for ( int i = 0; i < gw->powerUpQuantity; i++ ) {
        if ( powerUps[i].state == POWER_UP_STATE_CONSUMED ) {
            collectedIds[t++] = powerUps[i].id;
        }
    }

    for ( int i = 0; i < consumedCount; i++ ) {
        for ( int j = gw->powerUpQuantity-1; j >= 0; j-- ) {
            if ( powerUps[j].id == collectedIds[i] ) {
                for ( int k = j+1; k < gw->powerUpQuantity; k++ ) {
                    powerUps[k-1] = powerUps[k];
                }
                (gw->powerUpQuantity)--;
                break;
            }
        }
    }

    free( collectedIds );

}
