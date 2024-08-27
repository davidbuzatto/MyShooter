#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "Types.h"
#include "GameWorld.h"
#include "PowerUp.h"
#include "Block.h"
#include "raylib.h"

PowerUp createPowerUp( Vector3 pos, PowerUpType powerUpType ) {

    PowerUp powerUp = {
        .id = powerUpCount++,
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

        .mesh = { 0 },
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

void createPowerUpModel( PowerUp *powerUp ) {

    powerUp->mesh = GenMeshSphere( powerUp->radius, 10, 10 );
    powerUp->model = LoadModelFromMesh( powerUp->mesh );

    Image img = GenImageChecked( 2, 2, 1, 1, WHITE, LIGHTGRAY );
    Texture2D texture = LoadTextureFromImage( img );
    UnloadImage( img );

    powerUp->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

}

void destroyPowerUpModel( PowerUp *powerUp ) {
    UnloadTexture( powerUp->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
    UnloadModel( powerUp->model );
}

void createPowerUps( GameWorld *gw ) {

    gw->powerUpQuantity = 22;
    gw->powerUps = (PowerUp*) malloc( sizeof( PowerUp ) * gw->powerUpQuantity );

    Vector3 positions[] = {
        
        { -6, 1, 8 },
        { -2, 1, 8 },
        { 2, 1, 8 },
        { 6, 1, 8 },

        { -4, 1, 6 },
        { 0, 1, 6 },
        { 4, 1, 6 },

        { -6, 1, 4 },
        { -2, 1, 4 },
        { 2, 1, 4
         },
        { 6, 1, 4 },

        { -6, 1, -10 },
        { -2, 1, -10 },
        { 2, 1, -10 },
        { 6, 1, -10 },

        { -4, 1, -8 },
        { 0, 1, -8 },
        { 4, 1, -8 },

        { -6, 1, -6 },
        { -2, 1, -6 },
        { 2, 1, -6 },
        { 6, 1, -6 }

    };

    PowerUpType types[] = {
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_HP,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
        POWER_UP_TYPE_AMMO,
    };

    for ( int i = 0; i < gw->powerUpQuantity; i++ ) {
        gw->powerUps[i] = createPowerUp( positions[i], types[i] );
    }

    createPowerUpsModel( gw->powerUps, gw->powerUpQuantity );

}

void createPowerUpsModel( PowerUp *powerUps, int powerUpQuantity ) {

    PowerUp *basePowerUp = &powerUps[0];

    Image img = GenImageChecked( 2, 2, 1, 1, WHITE, LIGHTGRAY );
    Texture2D texture = LoadTextureFromImage( img );
    UnloadImage( img );

    basePowerUp->mesh = GenMeshSphere( basePowerUp->radius, 10, 10 );
    basePowerUp->model = LoadModelFromMesh( basePowerUp->mesh );
    basePowerUp->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    for ( int i = 1; i < powerUpQuantity; i++ ) {
        powerUps[i].mesh = basePowerUp->mesh;
        powerUps[i].model = basePowerUp->model;
    }

}

void destroyPowerUpsModel( PowerUp *powerUps, int powerUpQuantity ) {
    PowerUp *basePowerUp = &powerUps[0];
    UnloadTexture( basePowerUp->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
    UnloadModel( basePowerUp->model );
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
