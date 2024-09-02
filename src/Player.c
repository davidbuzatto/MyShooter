#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "Types.h"
#include "GameWorld.h"
#include "Block.h"
#include "Bullet.h"
#include "PowerUp.h"
#include "Enemy.h"
#include "Player.h"
#include "ResourceManager.h"
#include "raylib.h"
#include "utils.h"

Player createPlayer( Vector3 pos ) {

    float cpThickness = 1.0f;
    float cpDiff = 0.7f;
    float playerThickness = 2.0f;

    Player player = {
        .pos = pos,
        .lastPos = {
            .x = 0.0f,
            .y = 1.0f,
            .z = 0.0f
        },
        .dim = {
            .x = playerThickness, 
            .y = playerThickness,
            .z = playerThickness
        },
        .vel = {
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f
        },
        .speed = 20.0f,
        .walkingSpeed = 20.0f,
        .runningSpeed = 40.0f,
        .jumpSpeed = 20.0f,
        .color = Fade( BLUE, 0.8f ),
        .showWiresOnly = false,
        .showCollisionProbes = false,

        .model = { 0 },
        .rotationAxis = { 0.0f, 1.0f, 0.0f },
        .rotationHorizontalAngle = 0.0f,
        .rotationVerticalAngle = 90.0f,
        .rotationHVel = 0.0f,
        .rotationVVel = 0.0f,
        .rotationSpeed = 150.0f,
        .scale = { 1.0f, 1.0f, 1.0f },

        .weaponState = PLAYER_WEAPON_STATE_IDLE,
        .timeToNextShot = 0.1f,
        .timeToNextShotCounter = 0.0f,

        .cpLeft = { .visible = true },
        .cpRight = { .visible = true  },
        .cpBottom = { .visible = true },
        .cpTop = { .visible = true },
        .cpFar = { .visible = true },
        .cpNear = { .visible = true },
        .cpDimLR = { cpThickness, playerThickness - cpDiff, playerThickness - cpDiff },
        .cpDimBT = { playerThickness - cpDiff, cpThickness, playerThickness - cpDiff },
        .cpDimFN = { playerThickness - cpDiff, playerThickness - cpDiff, cpThickness },

        .positionState = PLAYER_POSITION_STATE_ON_GROUND,

        .maxHp = 100,
        .currentHp = 100,
        .immortal = false,

        .currentAmmo = 200,
        .weaponType = PLAYER_WEAPON_TYPE_SHOTGUN,

        .state = PLAYER_STATE_ALIVE

    };

    player.cpLeft.dim = player.cpDimLR;
    player.cpRight.dim = player.cpDimLR;
    player.cpBottom.dim = player.cpDimBT;
    player.cpTop.dim = player.cpDimBT;
    player.cpFar.dim = player.cpDimFN;
    player.cpNear.dim = player.cpDimFN;

    player.cpLeft.color = BLUE;
    player.cpRight.color = GREEN;
    player.cpBottom.color = RED;
    player.cpTop.color = GRAY;
    player.cpFar.color = YELLOW;
    player.cpNear.color = WHITE;

    createPlayerModel( &player );

    return player;

}

void drawPlayer( Player *player ) {
    
    if ( player->showCollisionProbes ) {
        drawBlock( &player->cpLeft );
        drawBlock( &player->cpRight );
        drawBlock( &player->cpBottom );
        drawBlock( &player->cpTop );
        drawBlock( &player->cpFar );
        drawBlock( &player->cpNear );
    }

    if ( !player->showWiresOnly ) {
        DrawModelEx( player->model, player->pos, player->rotationAxis, player->rotationHorizontalAngle, player->scale, WHITE );
    }

    DrawModelWiresEx( player->model, player->pos, player->rotationAxis, player->rotationHorizontalAngle, player->scale, BLACK );

}

void drawPlayerHud( Player *player ) {

    int h = GetScreenHeight();
    int xMargin = 10;
    int yMargin = 10;
    float t = (float) player->currentHp / player->maxHp;
    int hpBarWidth = 140;

    DrawRectangle( xMargin, h - yMargin - 47, (int) ( hpBarWidth * t ), 20, interpolate3Color( RED, ORANGE, LIME, t ) );
    DrawRectangleLines( xMargin, h - yMargin - 47, hpBarWidth, 20, BLACK );

    if ( player->immortal ) {
        DrawText( "* immortal *", xMargin * 2, h - yMargin - 46, 20, BLACK );
    }

    DrawText( TextFormat( "Ammo: %d", player->currentAmmo ), xMargin, h - yMargin - 20, 20, player->currentAmmo > 0 ? BLACK : MAROON );


}

void updatePlayer( Player *player, float delta ) {

    // not moving in x and z axis
    if ( player->pos.x == player->lastPos.x &&
         player->pos.z == player->lastPos.z ) {
        player->speed = player->walkingSpeed;
    }

    player->lastPos = player->pos;

    player->pos.x += player->vel.x * delta;
    player->pos.y += player->vel.y * delta;
    player->pos.z += player->vel.z * delta;

    player->vel.y -= GRAVITY * delta;

    player->rotationHorizontalAngle += player->rotationHVel * delta;
    player->rotationVerticalAngle += player->rotationVVel * delta;

    if ( player->rotationVerticalAngle < 0.0f ) {
        player->rotationVerticalAngle = 0.0f;
    } else if ( player->rotationVerticalAngle > 180.0f ) {
        player->rotationVerticalAngle = 180.0f;
    }

    if ( player->pos.y < player->lastPos.y ) {
        player->positionState = PLAYER_POSITION_STATE_FALLING;
    } else if ( player->pos.y > player->lastPos.y ) {
        player->positionState = PLAYER_POSITION_STATE_JUMPING;
    } else {
        player->positionState = PLAYER_POSITION_STATE_ON_GROUND;
    }

}

void updatePlayerCollisionProbes( Player *player ) {

    player->cpLeft.pos =
        (Vector3){ 
            player->pos.x - player->dim.x / 2 + player->cpLeft.dim.x / 2, 
            player->pos.y, 
            player->pos.z
        };

    player->cpRight.pos =
        (Vector3){ 
            player->pos.x + player->dim.x / 2 - player->cpRight.dim.x / 2, 
            player->pos.y, 
            player->pos.z
        };

    player->cpBottom.pos =
        (Vector3){ 
            player->pos.x, 
            player->pos.y - player->dim.y / 2 + player->cpBottom.dim.y / 2, 
            player->pos.z
        };

    player->cpTop.pos =
        (Vector3){ 
            player->pos.x, 
            player->pos.y + player->dim.y / 2 - player->cpTop.dim.y / 2, 
            player->pos.z
        };

    player->cpFar.pos =
        (Vector3){ 
            player->pos.x, 
            player->pos.y, 
            player->pos.z - player->dim.z / 2 + player->cpFar.dim.z / 2
        };

    player->cpNear.pos =
        (Vector3){ 
            player->pos.x, 
            player->pos.y, 
            player->pos.z + player->dim.z / 2 - player->cpNear.dim.z / 2
        };

}

void jumpPlayer( Player *player ) {
    if ( player->positionState == PLAYER_POSITION_STATE_ON_GROUND ) {
        player->vel.y = player->jumpSpeed;
    }
}

PlayerCollisionType checkCollisionPlayerBlock( Player *player, Block *block, bool checkCollisionProbes ) {

    BoundingBox playerBB = getPlayerBoundingBox( player );
    BoundingBox blockBB = getBlockBoundingBox( block );

    if ( checkCollisionProbes ) {

        if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpLeft ), blockBB ) ) {
            return PLAYER_COLLISION_LEFT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpRight ), blockBB ) ) {
            return PLAYER_COLLISION_RIGHT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpBottom ), blockBB ) ) {
            return PLAYER_COLLISION_BOTTOM;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpTop ), blockBB ) ) {
            return PLAYER_COLLISION_TOP;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpFar ), blockBB ) ) {
            return PLAYER_COLLISION_FAR;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpNear ), blockBB ) ) {
            return PLAYER_COLLISION_NEAR;
        }

    } else if ( CheckCollisionBoxes( playerBB, blockBB ) ) {
        return PLAYER_COLLISION_ALL;
    }

    return PLAYER_COLLISION_NONE;

}

PlayerCollisionType checkCollisionPlayerEnemy( Player *player, Enemy *enemy, bool checkCollisionProbes ) {

    BoundingBox playerBB = getPlayerBoundingBox( player );
    BoundingBox enemyBB = getEnemyBoundingBox( enemy );

    if ( checkCollisionProbes ) {

        if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpLeft ), enemyBB ) ) {
            return PLAYER_COLLISION_LEFT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpRight ), enemyBB ) ) {
            return PLAYER_COLLISION_RIGHT;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpBottom ), enemyBB ) ) {
            return PLAYER_COLLISION_BOTTOM;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpTop ), enemyBB ) ) {
            return PLAYER_COLLISION_TOP;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpFar ), enemyBB ) ) {
            return PLAYER_COLLISION_FAR;
        } else if ( CheckCollisionBoxes( getBlockBoundingBox( &player->cpNear ), enemyBB ) ) {
            return PLAYER_COLLISION_NEAR;
        }

    } else if ( CheckCollisionBoxes( playerBB, enemyBB ) ) {
        return PLAYER_COLLISION_ALL;
    }

    return PLAYER_COLLISION_NONE;

}

PlayerCollisionType checkCollisionPlayerPowerUp( Player *player, PowerUp *powerUp ) {

    BoundingBox playerBB = getPlayerBoundingBox( player );
    BoundingBox powerUpBB = getPowerUpBoundingBox( powerUp );

    if ( CheckCollisionBoxes( playerBB, powerUpBB ) ) {
        return PLAYER_COLLISION_ALL;
    }

    return PLAYER_COLLISION_NONE;

}

BoundingBox getPlayerBoundingBox( Player *player ) {
    return (BoundingBox) {
        .min = {
            .x = player->pos.x - player->dim.x / 2,
            .y = player->pos.y - player->dim.y / 2,
            .z = player->pos.z - player->dim.z / 2
        },
        .max = {
            .x = player->pos.x + player->dim.x / 2,
            .y = player->pos.y + player->dim.y / 2,
            .z = player->pos.z + player->dim.z / 2,
        },
    };
}

void createPlayerModel( Player *player ) {

    if ( !rm.playerModelCreated ) {

        Mesh mesh = GenMeshCube( player->dim.x, player->dim.y, player->dim.z );
        Model model = LoadModelFromMesh( mesh );

        Image img = GenImageChecked( 2, 2, 1, 1, BLUE, DARKBLUE );
        Texture2D texture = LoadTextureFromImage( img );
        UnloadImage( img );

        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

        rm.playerModel = model;
        rm.playerModelCreated = true;

    }

    player->model = rm.playerModel;

}

void playerShotHandgun( GameWorld *gw, Player *player, IdentifiedRayCollision *irc, Color bulletColor ) {
    
    if ( player->currentAmmo > 0 ) {
        
        Enemy *enemyShot = NULL;
        bool createBulletWorld = true;
        player->currentAmmo--;

        for ( int i = 0; i < gw->enemyQuantity; i++ ) {

            Enemy *enemy = &gw->enemies[i];

            if ( enemy->state == ENEMY_STATE_ALIVE && 
                    enemy->id == irc->entityId ) {
                
                enemy->currentHp--;
                enemy->showHpBar = true;
                enemyShot = enemy;

                if ( enemy->currentHp == 0 ) {
                    enemy->state = ENEMY_STATE_DEAD;
                    enemyShot = NULL;
                    createBulletWorld = false;
                    cleanDeadEnemies( gw );
                }

                break;

            }

        }

        if ( irc->entityType != ENTITY_TYPE_NONE ) {

            if ( enemyShot != NULL ) {
                addBulletToEnemy( enemyShot, irc->collision.point, bulletColor );
            } else if ( createBulletWorld ) {
                gw->collidedBullets[gw->collidedBulletCount%gw->maxCollidedBullets] = createBullet( irc->collision.point, bulletColor );
                gw->collidedBulletCount++;
            }

        }
        
    }

}

void playerShotMachinegun( GameWorld *gw, Player *player, IdentifiedRayCollision *irc, Color bulletColor ) {
    
    float delta = GetFrameTime();
    player->timeToNextShotCounter += delta;

    if ( player->timeToNextShotCounter >= player->timeToNextShot ) {

        player->timeToNextShotCounter = 0.0f;

        if ( player->currentAmmo > 0 ) {
            
            Enemy *enemyShot = NULL;
            bool createBulletWorld = true;
            player->currentAmmo--;

            for ( int i = 0; i < gw->enemyQuantity; i++ ) {

                Enemy *enemy = &gw->enemies[i];

                if ( enemy->state == ENEMY_STATE_ALIVE && 
                     enemy->id == irc->entityId ) {
                    
                    enemy->currentHp--;
                    enemy->showHpBar = true;
                    enemyShot = enemy;

                    if ( enemy->currentHp == 0 ) {
                        enemy->state = ENEMY_STATE_DEAD;
                        enemyShot = NULL;
                        createBulletWorld = false;
                        cleanDeadEnemies( gw );
                    }

                    break;

                }

            }

            if ( irc->entityType != ENTITY_TYPE_NONE ) {

                if ( enemyShot != NULL ) {
                    addBulletToEnemy( enemyShot, irc->collision.point, bulletColor );
                } else if ( createBulletWorld ) {
                    gw->collidedBullets[gw->collidedBulletCount%gw->maxCollidedBullets] = createBullet( irc->collision.point, bulletColor );
                    gw->collidedBulletCount++;
                }

            }

        }

    }

}

void playerShotShotgun( GameWorld *gw, Player *player, MultipleIdentifiedRayCollision *mirc, Color bulletColor ) {
    
    if ( player->currentAmmo > 0 ) {
        for ( int i = 0; i < mirc->quantity; i++ ) {

            IdentifiedRayCollision *irc = &mirc->irCollisions[i];
            //playerShotHandgun( gw, player, irc, bulletColor );

            Enemy *enemyShot = NULL;
            bool createBulletWorld = true;
            player->currentAmmo--;

            for ( int i = 0; i < gw->enemyQuantity; i++ ) {

                Enemy *enemy = &gw->enemies[i];

                if ( enemy->state == ENEMY_STATE_ALIVE && 
                        enemy->id == irc->entityId ) {
                    
                    enemy->currentHp--;
                    enemy->showHpBar = true;
                    enemyShot = enemy;

                    if ( enemy->currentHp == 0 ) {
                        enemy->state = ENEMY_STATE_DEAD;
                        enemyShot = NULL;
                        createBulletWorld = false;
                        cleanDeadEnemies( gw );
                    }

                    break;

                }

            }

            if ( irc->entityType != ENTITY_TYPE_NONE ) {

                if ( enemyShot != NULL ) {
                    addBulletToEnemy( enemyShot, irc->collision.point, bulletColor );
                } else if ( createBulletWorld ) {
                    gw->collidedBullets[gw->collidedBulletCount%gw->maxCollidedBullets] = createBullet( irc->collision.point, bulletColor );
                    gw->collidedBulletCount++;
                }

            }

        }
    }

}

void playerAcquirePowerUp( Player *player, PowerUp *powerUp ) {
    
    if ( powerUp->state == POWER_UP_STATE_ACTIVE ) {

        switch ( powerUp->type ) {

            case POWER_UP_TYPE_HP:

                if ( player->currentHp != player->maxHp ) {
                    player->currentHp += 20;
                    if ( player->currentHp > player->maxHp ) {
                        player->currentHp = player->maxHp;
                    }
                    powerUp->state = POWER_UP_STATE_CONSUMED;
                }

                break;

            case POWER_UP_TYPE_AMMO:
                player->currentAmmo += 50;
                powerUp->state = POWER_UP_STATE_CONSUMED;
                break;

            default:
                break;

        }

    }

}

Ray getPlayerToEnemyRay( Player *player, Enemy *enemy ) {

    // https://devforum.roblox.com/t/how-to-calculate-raycast-direction/1050858
    // rayOrigin + rayDirection = rayDestination

    // aqui
    // player->pos + direction = enemy->pos
    // direction = enemy->pos - player->pos

    return (Ray){
        .position = player->pos,
        .direction = {
            .x = enemy->pos.x - player->pos.x,
            .y = enemy->pos.y - player->pos.y,
            .z = enemy->pos.z - player->pos.z
        }
    };

}

Ray getPlayerToVector3Ray( Player *player, Vector3 v3 ) {
    return (Ray){
        .position = player->pos,
        .direction = {
            .x = v3.x - player->pos.x,
            .y = v3.y - player->pos.y,
            .z = v3.z - player->pos.z
        }
    };
}
