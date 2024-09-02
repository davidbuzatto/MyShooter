#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "Types.h"
#include "GameWorld.h"
#include "Block.h"
#include "Bullet.h"
#include "Enemy.h"
#include "ResourceManager.h"
#include "raylib.h"
#include "raymath.h"

Enemy createEnemy( Vector3 pos, Color color, Color eyeColor ) {

    float cpThickness = 1.0f;
    float cpDiff = 0.7f;
    float enemyThickness = 2.0f;

    Enemy enemy = {
        .id = entityIdCounter++,
        .pos = pos,
        .lastPos = {
            .x = 0.0f,
            .y = 1.0f,
            .z = 0.0f
        },
        .dim = {
            .x = enemyThickness, 
            .y = enemyThickness,
            .z = enemyThickness
        },
        .vel = {
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f
        },
        .speed = 20.0f,
        .jumpSpeed = 20.0f,
        .color = color,
        .eyeColor = eyeColor,
        .showWiresOnly = false,
        .showCollisionProbes = false,

        .model = { 0 },
        .rotationAxis = { 0.0f, 1.0f, 0.0f },
        .rotationHorizontalAngle = 0.0f,
        .rotationVel = 0.0f,
        .rotationSpeed = 150.0f,
        .scale = { 1.0f, 1.0f, 1.0f },

        .cpLeft = { .visible = true },
        .cpRight = { .visible = true  },
        .cpBottom = { .visible = true },
        .cpTop = { .visible = true },
        .cpFar = { .visible = true },
        .cpNear = { .visible = true },
        .cpDimLR = { cpThickness, enemyThickness - cpDiff, enemyThickness - cpDiff },
        .cpDimBT = { enemyThickness - cpDiff, cpThickness, enemyThickness - cpDiff },
        .cpDimFN = { enemyThickness - cpDiff, enemyThickness - cpDiff, cpThickness },

        .positionState = ENEMY_POSITION_STATE_ON_GROUND,
        .state = ENEMY_STATE_ALIVE,

        .maxHp = 5,
        .currentHp = 5,

        .detectedByPlayer = false,
        .showHpBar = false,
        .timeShowingHpBar = 4.0f,
        .hpBarShowCounter = 0.0f,
        .damageOnContact = 1,

        .maxCollidedBullets = 5,
        .collidedBulletCount = 0

    };

    enemy.cpLeft.dim = enemy.cpDimLR;
    enemy.cpRight.dim = enemy.cpDimLR;
    enemy.cpBottom.dim = enemy.cpDimBT;
    enemy.cpTop.dim = enemy.cpDimBT;
    enemy.cpFar.dim = enemy.cpDimFN;
    enemy.cpNear.dim = enemy.cpDimFN;

    enemy.cpLeft.color = BLUE;
    enemy.cpRight.color = GREEN;
    enemy.cpBottom.color = RED;
    enemy.cpTop.color = GRAY;
    enemy.cpFar.color = YELLOW;
    enemy.cpNear.color = WHITE;

    return enemy;

}

void drawEnemy( Enemy *enemy ) {

    if ( enemy->showCollisionProbes ) {
        drawBlock( &enemy->cpLeft );
        drawBlock( &enemy->cpRight );
        drawBlock( &enemy->cpBottom );
        drawBlock( &enemy->cpTop );
        drawBlock( &enemy->cpFar );
        drawBlock( &enemy->cpNear );
    }

    if ( !enemy->showWiresOnly ) {

        DrawModelEx( enemy->model, enemy->pos, enemy->rotationAxis, enemy->rotationHorizontalAngle, enemy->scale, enemy->color );

        float a = 45.0f;

        DrawSphere(
            (Vector3){
                .x = enemy->pos.x - cos( DEG2RAD * ( enemy->rotationHorizontalAngle + a ) ) * 1.0f,
                .y = enemy->pos.y + 1.0f,
                .z = enemy->pos.z + sin( DEG2RAD * ( enemy->rotationHorizontalAngle + a ) ) * 1.0f,
            },
            0.5f, 
            enemy->eyeColor
        );

        DrawSphere(
            (Vector3){
                .x = enemy->pos.x - cos( DEG2RAD * ( enemy->rotationHorizontalAngle - a ) ) * 1.0f,
                .y = enemy->pos.y + 1.0f,
                .z = enemy->pos.z + sin( DEG2RAD * ( enemy->rotationHorizontalAngle - a ) ) * 1.0f,
            },
            0.5f, 
            enemy->eyeColor
        );

    }

    int collidedBullets = enemy->collidedBulletCount < enemy->maxCollidedBullets ? enemy->collidedBulletCount : enemy->maxCollidedBullets;
    for ( int i = 0; i < collidedBullets; i++ ) {
        drawBullet( &enemy->collidedBullets[i] );
    }

    DrawModelWiresEx( enemy->model, enemy->pos, enemy->rotationAxis, enemy->rotationHorizontalAngle, enemy->scale, BLACK );

}

void drawEnemyHpBar( Enemy *enemy, Camera3D camera ) {

    if ( enemy->showHpBar && enemy->state == ENEMY_STATE_ALIVE && enemy->detectedByPlayer ) {

        float distance = Vector3Distance( enemy->pos, camera.position );
        float barWidth = 1200.0f / distance;
        int barHeight = (int) ( 200.f / distance );

        Vector3 p = enemy->pos;
        p.x += cos( DEG2RAD * ( enemy->rotationHorizontalAngle + 180 ) ) * enemy->dim.x / 2;
        p.y += enemy->dim.y - 0.5f;
        p.z += -sin( DEG2RAD * ( enemy->rotationHorizontalAngle + 180 ) ) * enemy->dim.z / 2;

        Vector2 v = GetWorldToScreen( p, camera );
        DrawRectangle( v.x - barWidth / 2, v.y - barHeight / 2, (int) (barWidth * enemy->currentHp / enemy->maxHp), barHeight, RED );
        DrawRectangleLines( v.x - barWidth / 2, v.y - barHeight / 2, barWidth, barHeight, BLACK );

    }

}

void updateEnemy( Enemy *enemy, Player *player, float delta ) {

    enemy->lastPos = enemy->pos;

    enemy->pos.x += enemy->vel.x * delta;
    enemy->pos.y += enemy->vel.y * delta;
    enemy->pos.z += enemy->vel.z * delta;

    enemy->vel.y -= GRAVITY * delta;

    //enemy->rotationHorizontalAngle += enemy->rotationVel * delta;

    if ( enemy->pos.y < enemy->lastPos.y ) {
        enemy->positionState = ENEMY_POSITION_STATE_FALLING;
    } else if ( enemy->pos.y > enemy->lastPos.y ) {
        enemy->positionState = ENEMY_POSITION_STATE_JUMPING;
    } else {
        enemy->positionState = ENEMY_POSITION_STATE_ON_GROUND;
    }

    if ( enemy->showHpBar ) {
        enemy->hpBarShowCounter += delta;
        if ( enemy->hpBarShowCounter >= enemy->timeShowingHpBar ) {
            enemy->hpBarShowCounter = 0.0f;
            enemy->showHpBar = false;
        }
    }

    enemy->rotationHorizontalAngle = - ( RAD2DEG * atan2( enemy->pos.z - player->pos.z, enemy->pos.x - player->pos.x ) );

    int collidedBullets = enemy->collidedBulletCount < enemy->maxCollidedBullets ? enemy->collidedBulletCount : enemy->maxCollidedBullets;
    for ( int i = 0; i < collidedBullets; i++ ) {
        Bullet *bullet = &enemy->collidedBullets[i];
        int h = enemy->rotationHorizontalAngle + 180;
        bullet->pos.x = enemy->pos.x - ( cos( DEG2RAD * ( h - bullet->hAngle ) ) * bullet->hDistance );
        bullet->pos.z = enemy->pos.z + ( sin( DEG2RAD * ( h - bullet->hAngle ) ) * bullet->hDistance );
        bullet->pos.y = enemy->pos.y - ( sin( DEG2RAD * ( bullet->vAngle ) ) * bullet->vDistance );
    }

}

void updateEnemyCollisionProbes( Enemy *enemy ) {

    enemy->cpLeft.pos =
        (Vector3){ 
            enemy->pos.x - enemy->dim.x / 2 + enemy->cpLeft.dim.x / 2, 
            enemy->pos.y, 
            enemy->pos.z
        };

    enemy->cpRight.pos =
        (Vector3){ 
            enemy->pos.x + enemy->dim.x / 2 - enemy->cpRight.dim.x / 2, 
            enemy->pos.y, 
            enemy->pos.z
        };

    enemy->cpBottom.pos =
        (Vector3){ 
            enemy->pos.x, 
            enemy->pos.y - enemy->dim.y / 2 + enemy->cpBottom.dim.y / 2, 
            enemy->pos.z
        };

    enemy->cpTop.pos =
        (Vector3){ 
            enemy->pos.x, 
            enemy->pos.y + enemy->dim.y / 2 - enemy->cpTop.dim.y / 2, 
            enemy->pos.z
        };

    enemy->cpFar.pos =
        (Vector3){ 
            enemy->pos.x, 
            enemy->pos.y, 
            enemy->pos.z - enemy->dim.z / 2 + enemy->cpFar.dim.z / 2
        };

    enemy->cpNear.pos =
        (Vector3){ 
            enemy->pos.x, 
            enemy->pos.y, 
            enemy->pos.z + enemy->dim.z / 2 - enemy->cpNear.dim.z / 2
        };

}

void jumpEnemy( Enemy *enemy ) {
    if ( enemy->positionState == ENEMY_POSITION_STATE_ON_GROUND ) {
        enemy->vel.y = enemy->jumpSpeed;
    }
}

EnemyCollisionType checkCollisionEnemyBlock( Enemy *enemy, Block *block, bool checkCollisionProbes ) {

    BoundingBox enemyBB = getEnemyBoundingBox( enemy );
    BoundingBox blockBB = getBlockBoundingBox( block );

    if ( checkCollisionProbes ) {

        if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpLeft ), blockBB ) ) {
            return ENEMY_COLLISION_LEFT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpRight ), blockBB ) ) {
            return ENEMY_COLLISION_RIGHT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpBottom ), blockBB ) ) {
            return ENEMY_COLLISION_BOTTOM;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpTop ), blockBB ) ) {
            return ENEMY_COLLISION_TOP;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpFar ), blockBB ) ) {
            return ENEMY_COLLISION_FAR;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &enemy->cpNear ), blockBB ) ) {
            return ENEMY_COLLISION_NEAR;
        }

    } else if ( CheckCollisionBoxes( enemyBB, blockBB ) ) {
        return ENEMY_COLLISION_ALL;
    }

    return ENEMY_COLLISION_NONE;

}

BoundingBox getEnemyBoundingBox( Enemy *enemy ) {

    return (BoundingBox) {
        .min = {
            .x = enemy->pos.x - enemy->dim.x / 2,
            .y = enemy->pos.y - enemy->dim.y / 2,
            .z = enemy->pos.z - enemy->dim.z / 2
        },
        .max = {
            .x = enemy->pos.x + enemy->dim.x / 2,
            .y = enemy->pos.y + enemy->dim.y / 2,
            .z = enemy->pos.z + enemy->dim.z / 2,
        },
    };

}

void createEnemies( GameWorld *gw, Vector3 *positions, int enemyQuantity, Color color, Color eyeColor ) {

    gw->enemyQuantity = enemyQuantity;
    gw->enemies = (Enemy*) malloc( sizeof( Enemy ) * gw->enemyQuantity );

    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        gw->enemies[i] = createEnemy( positions[i], color, eyeColor );
    }

    createEnemiesModel( gw->enemies, gw->enemyQuantity );

}

void createEnemiesModel( Enemy *enemies, int enemyQuantity ) {

    if ( !rm.enemyModelCreated ) {

        Enemy *baseEnemy = &enemies[0];

        Mesh mesh = GenMeshCube( baseEnemy->dim.x, baseEnemy->dim.y, baseEnemy->dim.z );
        Model model = LoadModelFromMesh( mesh );

        Image img = GenImageChecked( 2, 2, 1, 1, WHITE, LIGHTGRAY );
        Texture2D texture = LoadTextureFromImage( img );
        UnloadImage( img );

        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

        rm.enemyModel = model;
        rm.enemyModelCreated = true;

    }

    for ( int i = 0; i < enemyQuantity; i++ ) {
        enemies[i].model = rm.enemyModel;
    }

}

void setEnemyDetectedByPlayer( Enemy *enemy, Player *player, bool showLines ) {

    float distance = 200.0f;
    float angle = 44.0f;

    Vector3 vpos = player->pos;
    vpos.y = 0;

    Vector3 vdes1 = {
        vpos.x + cos( DEG2RAD * ( player->rotationHorizontalAngle + angle ) ) * distance,
        vpos.y,
        vpos.z + -sin( DEG2RAD * ( player->rotationHorizontalAngle + angle ) ) * distance
    };

    Vector3 vdes2 = {
        vpos.x + cos( DEG2RAD * ( player->rotationHorizontalAngle - angle ) ) * distance,
        vpos.y,
        vpos.z + -sin( DEG2RAD * ( player->rotationHorizontalAngle - angle ) ) * distance
    };

    if ( showLines ) {
        DrawLine3D( vpos, vdes1, BLACK );
        DrawLine3D( vpos, vdes2, BLACK );
    }

    Vector2 pEnemy = {
        .x = enemy->pos.x,
        .y = enemy->pos.z
    };

    Vector2 ptri1 = {
        .x = vpos.x,
        .y = vpos.z
    };

    Vector2 ptri2 = {
        .x = vdes1.x,
        .y = vdes1.z
    };

    Vector2 ptri3 = {
        .x = vdes2.x,
        .y = vdes2.z
    };

    enemy->detectedByPlayer = CheckCollisionPointTriangle( pEnemy, ptri1, ptri2, ptri3 );

}

void addBulletToEnemy( Enemy *enemy, Vector3 bulletPos, Color bulletColor ) {

    int i = enemy->collidedBulletCount % enemy->maxCollidedBullets;
    Bullet bullet = createBullet( bulletPos, bulletColor );
    
    float dX = enemy->pos.x - bulletPos.x;
    float dY = enemy->pos.y - bulletPos.y;
    float dZ = enemy->pos.z - bulletPos.z;

    bullet.hDistance = sqrt( dX * dX + dZ * dZ );
    bullet.vDistance = sqrt( dX * dX + dY * dY );
    bullet.hAngle = RAD2DEG * atan2( enemy->pos.z - bulletPos.z, enemy->pos.x - bulletPos.x ) + enemy->rotationHorizontalAngle + 180;
    bullet.vAngle = RAD2DEG * atan2( enemy->pos.y - bulletPos.y, enemy->pos.x - bulletPos.x );

    /*TraceLog( LOG_INFO, "hd: %.2f; vd: %.2f; h: %.2f; v: %.2f; he: %.2f", 
            bullet.hDistance,
            bullet.vDistance,
            bullet.hAngle,
            bullet.vAngle,
            enemy->rotationHorizontalAngle );*/

    enemy->collidedBullets[i] = bullet;
    enemy->collidedBulletCount++;

}

Matrix getEnemyTransformMatrix( Enemy *enemy ) {
    Matrix matScale = MatrixScale( enemy->scale.x, enemy->scale.y, enemy->scale.z );
    Matrix matRotation = MatrixRotate( enemy->rotationAxis, enemy->rotationHorizontalAngle * DEG2RAD );
    Matrix matTranslation = MatrixTranslate( enemy->pos.x, enemy->pos.y, enemy->pos.z );
    Matrix matTransform = MatrixMultiply( MatrixMultiply( matScale, matRotation ), matTranslation );
    return MatrixMultiply( enemy->model.transform, matTransform );
}

void cleanDeadEnemies( GameWorld *gw ) {

    // naive algorithm
    int deadCount = 0;
    Enemy *enemies = gw->enemies;

    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        if ( enemies[i].state == ENEMY_STATE_DEAD ) {
            deadCount++;
        }
    }

    int *collectedIds = (int*) malloc( deadCount * sizeof( int ) );
    int t = 0;

    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        if ( enemies[i].state == ENEMY_STATE_DEAD ) {
            collectedIds[t++] = enemies[i].id;
        }
    }

    for ( int i = 0; i < deadCount; i++ ) {
        for ( int j = gw->enemyQuantity-1; j >= 0; j-- ) {
            if ( enemies[j].id == collectedIds[i] ) {
                for ( int k = j+1; k < gw->enemyQuantity; k++ ) {
                    enemies[k-1] = enemies[k];
                }
                (gw->enemyQuantity)--;
                break;
            }
        }
    }

    free( collectedIds );

}
