/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Types.h"
#include "GameWorld.h"
#include "PowerUp.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Block.h"
#include "raylib.h"

//#include "raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raygui.h"              // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

const float GRAVITY = 50.0f;
const int CAMERA_TYPE_QUANTITY = 4;
const bool CREATE_OBSTACLES = true;
const int gamepadId = 0;

int bulletCount = 0;
int enemyCount = 0;
int powerUpCount = 0;

bool showInfo = true;
bool drawWalls = true;

float xCam = 0.0f;
float yCam = 25.0f;
float zCam = 30.0f;

Color enemyColor;
Color enemyEyeColor;

/*float xCam = 0.0f;
float yCam = 9.0f;
float zCam = 7.4f;*/

float firstPersonCameraTargetDist = 10.0f;

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    float blockSize = 2.0f;
    int lines = 10;
    int columns = 50;

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    Color wallColor = Fade( DARKGREEN, 0.5f );
    Color obstacleColor = LIME;

    enemyColor = RED;
    enemyEyeColor = (Color){ 38, 0, 82, 255 };

    gw->previousMousePos = (Vector2){0};

    gw->player = createPlayer();
    createEnemies( gw, enemyColor, enemyEyeColor );
    createPowerUps( gw );

    gw->ground = createGround( blockSize, lines, columns );
    createWalls( gw, wallColor );
    if ( CREATE_OBSTACLES ) {
        createObstacles( gw, blockSize, obstacleColor );
    } else {
        gw->obstacleQuantity = 0;
    }

    //gw->cameraType = CAMERA_TYPE_THIRD_PERSON_FIXED;
    gw->cameraType = CAMERA_TYPE_FIRST_PERSON;
    setupCamera( gw );
    updateCameraTarget( gw, &gw->player );
    updateCameraPosition( gw, &gw->player, xCam, yCam, zCam );

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    destroyPlayerModel( &gw->player );
    destroyEnemiesModel( gw->enemies, gw->enemyQuantity );
    free( gw->enemies );
    destroyPowerUpsModel( gw->powerUps, gw->powerUpQuantity );
    free( gw->powerUps );
    destroyGroundModel( &gw->ground );
    destroyObstaclesModel( gw->obstacles, gw->obstacleQuantity );
    free( gw->obstacles );
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void inputAndUpdateGameWorld( GameWorld *gw ) {

    float delta = GetFrameTime();
    Player *player = &gw->player;

    processOptionsInput( player, gw );
    
    if ( player->state == PLAYER_STATE_ALIVE ) {

        Player *player = &gw->player;
        Block *ground = &gw->ground;
        Block *leftWall = &gw->leftWall;
        Block *rightWall = &gw->rightWall;
        Block *farWall = &gw->farWall;
        Block *nearWall = &gw->nearWall;

        processCameraInput( &xCam, &yCam, &zCam );
        processPlayerInput( player, gw->cameraType, delta, false );

        updatePlayer( player, delta );
        updatePlayerCollisionProbes( player );
        
        resolveCollisionPlayerObstacles( player, gw );
        updatePlayerCollisionProbes( player );
        resolveCollisionPlayerGround( player, ground );
        resolveCollisionPlayerWalls( player, leftWall, rightWall, farWall, nearWall );

        for ( int i = 0; i < gw->powerUpQuantity; i++ ) {
            PowerUp *powerUp = &gw->powerUps[i];
            updatePowerUp( powerUp, delta );
            resolveCollisionPlayerPowerUp( player, powerUp );
            resolveCollisionPowerUpGround( powerUp, ground );
        }
        cleanConsumedPowerUps( gw );

        for ( int i = 0; i < gw->enemyQuantity; i++ ) {
            Enemy *enemy = &gw->enemies[i];
            if ( enemy->positionState == ENEMY_POSITION_STATE_ON_GROUND ) {
                if ( GetRandomValue( 0, 100 ) == 0 ) {
                    jumpEnemy( enemy );
                }
            }
            updateEnemy( enemy, player, delta );
            updateEnemyCollisionProbes( enemy );
            resolveCollisionEnemyObstacles( enemy, gw );
            resolveCollisionEnemyGround( enemy, ground );
            resolveCollisionEnemyWalls( enemy, leftWall, rightWall, farWall, nearWall );
            resolveCollisionPlayerEnemy( player, enemy );
            setEnemyDetectedByPlayer( enemy, &gw->player, true );
        }

        resolveCollisionBulletWorld( gw );

        updateCameraTarget( gw, &gw->player );
        updateCameraPosition( gw, &gw->player, xCam, yCam, zCam );

    }

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    BeginMode3D( gw->camera );

    //DrawGrid( 120, 1.0f );

    drawBlock( &gw->ground );
    drawPlayer( &gw->player );
    
    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        drawEnemy( &gw->enemies[i] );
        //DrawRay( getPlayerToEnemyRay( &gw->player, &gw->enemies[i] ), BLACK );
    }

    for ( int i = 0; i < gw->powerUpQuantity; i++ ) {
        drawPowerUp( &gw->powerUps[i] );
    }

    for ( int i = 0; i < gw->obstacleQuantity; i++ ) {
        drawBlock( &gw->obstacles[i] );
    }

    if ( drawWalls ) {
        drawBlock( &gw->leftWall );
        drawBlock( &gw->rightWall );
        drawBlock( &gw->farWall );
        drawBlock( &gw->nearWall );
    }

    EndMode3D();

    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        drawEnemyHpBar( &gw->enemies[i], gw->camera );
    }

    drawPlayerHud( &gw->player );
    drawReticle( gw->cameraType, gw->player.weaponState, 30 );

    // debug info
    if ( showInfo ) {
        DrawFPS( 10, 10 );
        DrawText( TextFormat( "player: x=%.1f, y=%.1f, z=%.1f", gw->player.pos.x, gw->player.pos.y, gw->player.pos.z ), 10, 30, 20, BLACK );
        DrawText( TextFormat( "active bullets: %d", gw->player.bulletQuantity ), 10, 50, 20, BLACK );
        DrawText( TextFormat( "active enemies: %d", gw->enemyQuantity ), 10, 70, 20, BLACK );
        DrawText( TextFormat( "active power-ups: %d", gw->powerUpQuantity ), 10, 90, 20, BLACK );
        showCameraInfo( &gw->camera, 10, 110 );
    }

    if ( gw->player.state == PLAYER_STATE_DEAD ) {
        DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), Fade( BLACK, 0.85f ) );
        int fontSizeDead = 60;
        int fontSizeReset = 20;
        const char *tDead = "YOU ARE DEAD!";
        const char *tReset = "Start/<R> to Reset!";
        int wDead = MeasureText( tDead, fontSizeDead );
        int wReset = MeasureText( tReset, fontSizeReset );
        DrawText( tDead, GetScreenWidth() / 2 - wDead / 2, GetScreenHeight() / 2 - fontSizeDead / 2 - 10, fontSizeDead, RED );
        DrawText( tReset, GetScreenWidth() / 2 - wReset / 2, GetScreenHeight() / 2 - fontSizeReset / 2 + 30, fontSizeReset, RED );
    }

    EndDrawing();

}

void drawReticle( CameraType cameraType, PlayerWeaponState weaponState, int reticleSize ) {

    if ( cameraType == CAMERA_TYPE_FIRST_PERSON || 
         cameraType == CAMERA_TYPE_FIRST_PERSON_MOUSE ) {
        int xCenter = GetScreenWidth() / 2;
        int yCenter = GetScreenHeight() / 2;
        Color reticleColor = weaponState == PLAYER_WEAPON_STATE_READY ? RED : BLACK;
        DrawLine( xCenter - reticleSize, yCenter, xCenter + reticleSize, yCenter, reticleColor );
        DrawLine( xCenter, yCenter - reticleSize, xCenter, yCenter + reticleSize, reticleColor );
    }

}

void setupCamera( GameWorld *gw ) {
    gw->camera = (Camera3D){ 0 };
    gw->camera.position = (Vector3){ 0.0f, 25.0f, 0.0f };   // Camera position
    gw->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    gw->camera.fovy = 45.0f;                                // Camera field-of-view Y
    gw->camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type
}

void updateCameraTarget( GameWorld *gw, Player *player ) {

    float delta = GetFrameTime();

    if ( gw->cameraType == CAMERA_TYPE_FIRST_PERSON_MOUSE ) {
        Vector2 mousePos = GetMousePosition();
        player->rotationHorizontalAngle += ( gw->previousMousePos.x - mousePos.x ) * delta * 70;
        player->rotationVerticalAngle += ( gw->previousMousePos.y - mousePos.y ) * delta * 30;
        gw->previousMousePos = mousePos;
        HideCursor();
    } else {
        ShowCursor();
    }

    float cosH = cos( DEG2RAD * player->rotationHorizontalAngle );
    float sinH = -sin( DEG2RAD * player->rotationHorizontalAngle );

    float cosV = cos( DEG2RAD * player->rotationVerticalAngle );
    //float sinV = -sin( DEG2RAD * player->rotationVerticalAngle );

    switch ( gw->cameraType ) {
        case CAMERA_TYPE_THIRD_PERSON_FIXED:
            gw->camera.target = player->pos;
            break;
        case CAMERA_TYPE_THIRD_PERSON_FIXED_SHOULDER:
        case CAMERA_TYPE_FIRST_PERSON:
        case CAMERA_TYPE_FIRST_PERSON_MOUSE:
            gw->camera.target.x = player->pos.x + cosH * firstPersonCameraTargetDist;
            gw->camera.target.y = player->pos.y + cosV * firstPersonCameraTargetDist;
            gw->camera.target.z = player->pos.z + sinH * firstPersonCameraTargetDist;
            break;
    }

}

void updateCameraPosition( GameWorld *gw, Player *player, float xOffset, float yOffset, float zOffset ) {

    float cosH = cos( DEG2RAD * player->rotationHorizontalAngle );
    float sinH = -sin( DEG2RAD * player->rotationHorizontalAngle );

    switch ( gw->cameraType ) {
        case CAMERA_TYPE_THIRD_PERSON_FIXED:
            gw->camera.position.x = player->pos.x + xOffset;
            gw->camera.position.y = yOffset;
            gw->camera.position.z = player->pos.z + zOffset;
            break;
        case CAMERA_TYPE_THIRD_PERSON_FIXED_SHOULDER:
            gw->camera.position = player->pos;
            gw->camera.position.x += cosH * ( -player->dim.x * 4 );
            gw->camera.position.y += player->dim.y;
            gw->camera.position.z += sinH * ( -player->dim.z * 4 );
            break;
        case CAMERA_TYPE_FIRST_PERSON:
        case CAMERA_TYPE_FIRST_PERSON_MOUSE:
            gw->camera.position = player->pos;
            gw->camera.position.x += cosH * ( player->dim.x / 2 );
            gw->camera.position.z += sinH * ( player->dim.z / 2 );
            break;
    }

}

void showCameraInfo( Camera3D *camera, int x, int y ) {

    const char *pos = TextFormat( 
        "camera: x=%.2f, y=%.2f, z=%.2f",
        camera->position.x,
        camera->position.y,
        camera->position.z
    );

    DrawText( pos, x, y, 20, BLACK );

}

Block createGround( float blockSize, int lines, int columns ) {

    Block ground = {
        .pos = {
            .x = -1.0f,
            .y = -1.0f,
            .z = -1.0f
        },
        .dim = {
            .x = columns * blockSize, 
            .y = blockSize,
            .z = lines * blockSize
        },
        .color = ORANGE,
        .tintColor = WHITE,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    createGroundModel( &ground );

    return ground;

}

void createGroundModel( Block *ground ) {

    ground->renderModel = true;
    ground->mesh = GenMeshCube( ground->dim.x, ground->dim.y, ground->dim.z );
    ground->model = LoadModelFromMesh( ground->mesh );

    Image img = GenImageChecked( ground->dim.x, ground->dim.z, 2, 2, ORANGE, (Color){ 192, 96, 0, 255 } );
    Texture2D texture = LoadTextureFromImage( img );
    UnloadImage( img );

    ground->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

}

void destroyGroundModel( Block *ground ) {
    UnloadTexture( ground->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
    UnloadModel( ground->model );
}

void createObstacles( GameWorld *gw, float blockSize, Color obstacleColor ) {

    gw->obstacleQuantity = 44;
    gw->obstacles = (Block*) malloc( sizeof( Block ) * gw->obstacleQuantity );

    Vector3 positions[] = {
        
        { 10, 1, 0 },
        { 12, 3, 0 },
        { 14, 5, 0 },
        { 16, 5, 0 },
        { 18, 5, 0 },
        { 20, 5, 0 },
        { 22, 3, 0 },
        { 24, 1, 0 },
        { 10, 1, -2 },
        { 12, 3, -2 },
        { 14, 5, -2 },
        { 16, 5, -2 },
        { 18, 5, -2 },
        { 20, 5, -2 },
        { 22, 3, -2 },
        { 24, 1, -2 },

        { -10, 1, 6 },
        { -10, 3, 4 },
        { -10, 5, 2 },
        { -10, 5, 0 },
        { -10, 5, -2 },
        { -10, 5, -4 },
        { -10, 3, -6 },
        { -10, 1, -8 },
        { -12, 1, 6 },
        { -12, 3, 4 },
        { -12, 5, 2 },
        { -12, 5, 0 },
        { -12, 5, -2 },
        { -12, 5, -4 },
        { -12, 3, -6 },
        { -12, 1, -8 },

        { 32, 1, 2 },
        { 34, 1, 2 },
        { 34, 1, 4 },
        { 34, 3, 0 },
        { 36, 3, 0 },
        { 36, 3, 2 },
        { 36, 5, -2 },
        { 38, 5, -2 },
        { 38, 5, 0 },
        { 38, 7, -4 },
        { 40, 7, -4 },
        { 40, 7, -2 },
    };

    for ( int i = 0; i < gw->obstacleQuantity; i++ ) {
        gw->obstacles[i] = (Block){
            .pos = positions[i],
            .dim = { blockSize, blockSize, blockSize },
            .color = obstacleColor,
            .tintColor = obstacleColor,
            .touchColor = obstacleColor,
            .visible = true,
            .renderModel = false,
            .renderTouchColor = false
        };
    }

    createObstaclesModel( gw->obstacles, gw->obstacleQuantity );

}

void createObstaclesModel( Block *obstacles, int obstaclesQuantity ) {

    Block *baseObstacle = &obstacles[0];

    Image img = GenImageChecked( 2, 2, 1, 1, WHITE, LIGHTGRAY );
    Texture2D texture = LoadTextureFromImage( img );
    UnloadImage( img );

    baseObstacle->renderModel = true;
    baseObstacle->mesh = GenMeshCube( baseObstacle->dim.x, baseObstacle->dim.y, baseObstacle->dim.z );
    baseObstacle->model = LoadModelFromMesh( baseObstacle->mesh );
    baseObstacle->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    for ( int i = 1; i < obstaclesQuantity; i++ ) {
        obstacles[i].renderModel = true;
        obstacles[i].mesh = baseObstacle->mesh;
        obstacles[i].model = baseObstacle->model;
    }

}

void destroyObstaclesModel( Block *obstacles, int obstaclesQuantity ) {
    Block *baseObstacle = &obstacles[0];
    UnloadTexture( baseObstacle->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
    UnloadModel( baseObstacle->model );
}

void createWalls( GameWorld *gw, Color wallColor ) {

    gw->leftWall = (Block){
        .pos = {
            .x = -52.0f,
            .y = 5.0f,
            .z = -1.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = 10.0f,
            .z = 20.0f
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->rightWall = (Block){
        .pos = {
            .x = 50.0f,
            .y = 5.0f,
            .z = -1.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = 10.0f,
            .z = 20.0f
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->farWall = (Block){
        .pos = {
            .x = -1.0f,
            .y = 5.0f,
            .z = -12.0f
        },
        .dim = {
            .x = 100.0f, 
            .y = 10.0f,
            .z = 2.0f
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->nearWall = (Block){
        .pos = {
            .x = -1.0f,
            .y = 5.0f,
            .z = 10.0f
        },
        .dim = {
            .x = 100.0f, 
            .y = 10.0f,
            .z = 2.0f
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

}

void processOptionsInput( Player *player, GameWorld *gw ) {

    if ( IsKeyPressed( KEY_ONE ) ) {
        showInfo = !showInfo;
    }

    if ( IsKeyPressed( KEY_TWO ) ) {
        drawWalls = !drawWalls;
    }

    if ( IsKeyPressed( KEY_THREE ) ) {
        player->showWiresOnly = !player->showWiresOnly;
    }

    if ( IsKeyPressed( KEY_FOUR ) ) {
        player->showCollisionProbes = !player->showCollisionProbes;
    }

    if ( IsKeyPressed( KEY_FIVE ) ) {
        for ( int i = 0; i < gw->obstacleQuantity; i++ ) {
            gw->obstacles[i].renderTouchColor = !gw->obstacles[i].renderTouchColor;
        }
    }

    if ( IsKeyPressed( KEY_R ) || 
         ( IsGamepadAvailable( gamepadId ) && IsGamepadButtonPressed( gamepadId, GAMEPAD_BUTTON_MIDDLE_RIGHT ) ) ) {
        resetGameWorld( gw );
    }

    if ( IsKeyPressed( KEY_F ) ) {
        int ct = gw->cameraType + 1;
        gw->cameraType = ct % CAMERA_TYPE_QUANTITY;
    }

    if ( IsKeyPressed( KEY_I ) ) {
        player->immortal = !player->immortal;
    }

}

void processCameraInput( float *xCam, float *yCam, float *zCam ) {

    if ( IsKeyDown( KEY_UP ) ) {
        *yCam += 1;
    } else if ( IsKeyDown( KEY_DOWN ) ) {
        *yCam -= 1;
    } else if ( IsKeyDown( KEY_LEFT ) ) {
        *xCam -= 1;
    } else if ( IsKeyDown( KEY_RIGHT ) ) {
        *xCam += 1;
    } else if ( IsKeyDown( KEY_KP_SUBTRACT ) ) {
        *zCam -= 1;
    } else if ( IsKeyDown( KEY_KP_ADD ) ) {
        *zCam += 1;
    }

}

void processPlayerInput( Player *player, CameraType cameraType, float delta, bool keyboard ) {

    if ( keyboard ) {

        // keyboard movement
        if ( IsKeyDown( KEY_LEFT_CONTROL ) ) {
            player->speed = player->runningSpeed;
        } else {
            player->speed = player->walkingSpeed;
        }

        if ( IsKeyDown( KEY_W ) ) {
            player->vel.x = cos( DEG2RAD * player->rotationHorizontalAngle ) * player->speed;
            player->vel.z = -sin( DEG2RAD * player->rotationHorizontalAngle ) * player->speed;
        } else if ( IsKeyDown( KEY_S ) ) {
            player->vel.x = -cos( DEG2RAD * player->rotationHorizontalAngle ) * player->speed;
            player->vel.z = sin( DEG2RAD * player->rotationHorizontalAngle ) * player->speed;
        } else {
            player->vel.x = 0.0f;
            player->vel.z = 0.0f;
        }

        if ( IsKeyDown( KEY_A ) ) {
            if ( cameraType == CAMERA_TYPE_FIRST_PERSON_MOUSE ) {
                player->vel.x = cos( DEG2RAD * ( player->rotationHorizontalAngle + 90 ) ) * player->speed;
                player->vel.z = -sin( DEG2RAD * ( player->rotationHorizontalAngle + 90 ) ) * player->speed;
            } else {
                player->rotationVel = player->rotationSpeed;
            }
        } else if ( IsKeyDown( KEY_D ) ) {
            if ( cameraType == CAMERA_TYPE_FIRST_PERSON_MOUSE ) {
                player->vel.x = cos( DEG2RAD * ( player->rotationHorizontalAngle - 90 ) ) * player->speed;
                player->vel.z = -sin( DEG2RAD * ( player->rotationHorizontalAngle - 90 ) ) * player->speed;
            } else {
                player->rotationVel = -player->rotationSpeed;
            }
        } else {
            player->rotationVel = 0.0f;
        }

        if ( IsKeyPressed( KEY_SPACE ) ) {
            jumpPlayer( player );
        }

    } else {

        // joystick
        if ( IsGamepadAvailable( gamepadId ) ) {

            // tank
            /*float gpx = -GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_RIGHT_X );
            float gpy = -GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_LEFT_Y );

            player->rotationVel = player->rotationSpeed * gpx;
            player->vel.x = cos( DEG2RAD * player->rotationAngle ) * player->speed * gpy;
            player->vel.z = -sin( DEG2RAD * player->rotationAngle ) * player->speed * gpy;*/

            // standard
            float gpxLeft = GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_LEFT_X );
            float gpyLeft = GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_LEFT_Y );

            float gpxRight = GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_RIGHT_X );
            float gpyRight = GetGamepadAxisMovement( gamepadId, GAMEPAD_AXIS_RIGHT_Y );

            if ( IsGamepadButtonPressed( gamepadId, GAMEPAD_BUTTON_LEFT_THUMB ) ) {
                player->speed = player->runningSpeed;
            }

            if ( cameraType == CAMERA_TYPE_THIRD_PERSON_FIXED ) {
                player->vel.x = player->speed * gpxLeft;
                player->vel.z = player->speed * gpyLeft;
                if ( player->vel.x != 0.0f || player->vel.z != 0.0f ) {
                    player->rotationHorizontalAngle = RAD2DEG * atan2( -player->vel.z, player->vel.x );
                }
            } else {
                player->rotationHorizontalAngle += gpxRight * delta * -player->rotationSpeed;
                player->rotationVerticalAngle += gpyRight * delta * -player->rotationSpeed;
                if ( player->rotationVerticalAngle < 0.0f ) {
                    player->rotationVerticalAngle = 0.0f;
                } else if ( player->rotationVerticalAngle > 180.0f ) {
                    player->rotationVerticalAngle = 180.0f;
                }
                float xMultiplier = ( ( -cos( DEG2RAD * player->rotationHorizontalAngle ) * gpyLeft ) +
                                    ( -cos( DEG2RAD * ( player->rotationHorizontalAngle + 90 ) ) * gpxLeft ) ) / 2.0f;
                float zMultiplier = ( ( sin( DEG2RAD * player->rotationHorizontalAngle ) * gpyLeft ) +
                                    ( sin( DEG2RAD * ( player->rotationHorizontalAngle + 90 ) ) * gpxLeft ) ) / 2.0f;
                player->vel.x = player->speed * xMultiplier;
                player->vel.z = player->speed * zMultiplier;
            }

            if ( IsGamepadButtonPressed( gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_DOWN ) ) {
                jumpPlayer( player );
            }

            if ( IsGamepadButtonDown( gamepadId, GAMEPAD_BUTTON_LEFT_TRIGGER_2 ) ) {
                player->weaponState = PLAYER_WEAPON_STATE_READY;
                if ( IsGamepadButtonDown( gamepadId, GAMEPAD_BUTTON_RIGHT_TRIGGER_2 ) ) {
                    playerShotBullet( player );
                }
            } else {
                player->weaponState = PLAYER_WEAPON_STATE_IDLE;
            }

        }

    }

}

void resolveCollisionPlayerObstacles( Player *player, GameWorld *gw ) {

    for ( int i = 0; i < gw->obstacleQuantity; i++ ) {
        Block *obs = &gw->obstacles[i];
        PlayerCollisionType coll = checkCollisionPlayerBlock( player, obs, true );
        switch ( coll ) {
            case PLAYER_COLLISION_LEFT:
                player->pos.x = obs->pos.x + obs->dim.x / 2 + player->dim.x / 2;
                obs->touchColor = Fade( player->cpLeft.color, 0.7f );
                break;
            case PLAYER_COLLISION_RIGHT:
                player->pos.x = obs->pos.x - obs->dim.x / 2 - player->dim.x / 2;
                obs->touchColor = Fade( player->cpRight.color, 0.7f );
                break;
            case PLAYER_COLLISION_BOTTOM:
                player->pos.y = obs->pos.y + obs->dim.y / 2 + player->dim.y / 2;
                player->vel.y = 0.0f;
                obs->touchColor = Fade( player->cpBottom.color, 0.7f );
                break;
            case PLAYER_COLLISION_TOP:
                player->pos.y = obs->pos.y - obs->dim.y / 2 - player->dim.y / 2 - 0.05f;
                player->vel.y = 0.0f;
                obs->touchColor = Fade( player->cpTop.color, 0.7f );
                break;
            case PLAYER_COLLISION_FAR:
                player->pos.z = obs->pos.z + obs->dim.z / 2 + player->dim.z / 2;
                obs->touchColor = Fade( player->cpFar.color, 0.7f );
                break;
            case PLAYER_COLLISION_NEAR:
                player->pos.z = obs->pos.z - obs->dim.z / 2 - player->dim.z / 2;
                obs->touchColor = Fade( player->cpNear.color, 0.7f );
                break;
            case PLAYER_COLLISION_ALL:
            case PLAYER_COLLISION_NONE:
            default:
                break;
        }
    }
    
}

void resolveCollisionEnemyObstacles( Enemy *enemy, GameWorld *gw ) {

    for ( int i = 0; i < gw->obstacleQuantity; i++ ) {
        Block *obs = &gw->obstacles[i];
        EnemyCollisionType coll = checkCollisionEnemyBlock( enemy, obs, true );
        switch ( coll ) {
            case ENEMY_COLLISION_LEFT:
                enemy->pos.x = obs->pos.x + obs->dim.x / 2 + enemy->dim.x / 2;
                break;
            case ENEMY_COLLISION_RIGHT:
                enemy->pos.x = obs->pos.x - obs->dim.x / 2 - enemy->dim.x / 2;
                break;
            case ENEMY_COLLISION_BOTTOM:
                enemy->pos.y = obs->pos.y + obs->dim.y / 2 + enemy->dim.y / 2;
                enemy->vel.y = 0.0f;
                break;
            case ENEMY_COLLISION_TOP:
                enemy->pos.y = obs->pos.y - obs->dim.y / 2 - enemy->dim.y / 2 - 0.05f;
                enemy->vel.y = 0.0f;
                break;
            case ENEMY_COLLISION_FAR:
                enemy->pos.z = obs->pos.z + obs->dim.z / 2 + enemy->dim.z / 2;
                break;
            case ENEMY_COLLISION_NEAR:
                enemy->pos.z = obs->pos.z - obs->dim.z / 2 - enemy->dim.z / 2;
                break;
            case ENEMY_COLLISION_ALL:
            case ENEMY_COLLISION_NONE:
            default:
                break;
        }
    }
    
}

void resolveCollisionPlayerGround( Player *player, Block *ground ) {
    if ( checkCollisionPlayerBlock( player, ground, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.y = ground->pos.y + ground->dim.y / 2 + player->dim.y / 2;
        player->vel.y = 0.0f;
        updatePlayerCollisionProbes( player );
    }
}

void resolveCollisionEnemyGround( Enemy *enemy, Block *ground ) {
    if ( checkCollisionEnemyBlock( enemy, ground, false ) ==  ENEMY_COLLISION_ALL ) {
        enemy->pos.y = ground->pos.y + ground->dim.y / 2 + enemy->dim.y / 2;
        enemy->vel.y = 0.0f;
        updateEnemyCollisionProbes( enemy );
    }
}

void resolveCollisionPowerUpGround( PowerUp *powerUp, Block *ground ) {
    if ( checkCollisionPowerUpBlock( powerUp, ground ) ==  POWER_UP_COLLISION_ALL ) {
        powerUp->pos.y = ground->pos.y + ground->dim.y / 2 + powerUp->radius;
        powerUp->vel.y = 0.0f;
        jumpPowerUp( powerUp );
    }
}

void resolveCollisionPlayerWalls( Player *player, Block *leftWall, Block *rightWall, Block *farWall, Block *nearWall ) {

    if ( checkCollisionPlayerBlock( player, leftWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.x = leftWall->pos.x + leftWall->dim.x / 2 + player->dim.y / 2;
        updatePlayerCollisionProbes( player );
    }
    
    if ( checkCollisionPlayerBlock( player, rightWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.x = rightWall->pos.x - rightWall->dim.x / 2 - player->dim.y / 2;
        updatePlayerCollisionProbes( player );
    }
    
    if ( checkCollisionPlayerBlock( player, farWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.z = farWall->pos.z + farWall->dim.z / 2 + player->dim.z / 2;
        updatePlayerCollisionProbes( player );
    }
    
    if ( checkCollisionPlayerBlock( player, nearWall, false ) == PLAYER_COLLISION_ALL ) {
        player->pos.z = nearWall->pos.z - nearWall->dim.z / 2 - player->dim.z / 2;
        updatePlayerCollisionProbes( player );
    }

}

void resolveCollisionEnemyWalls( Enemy *enemy, Block *leftWall, Block *rightWall, Block *farWall, Block *nearWall ) {

    if ( checkCollisionEnemyBlock( enemy, leftWall, false ) == ENEMY_COLLISION_ALL ) {
        enemy->pos.x = leftWall->pos.x + leftWall->dim.x / 2 + enemy->dim.y / 2;
        enemy->vel.x = -enemy->vel.x;
        updateEnemyCollisionProbes( enemy );
    }
    
    if ( checkCollisionEnemyBlock( enemy, rightWall, false ) == ENEMY_COLLISION_ALL ) {
        enemy->pos.x = rightWall->pos.x - rightWall->dim.x / 2 - enemy->dim.y / 2;
        enemy->vel.x = -enemy->vel.x;
        updateEnemyCollisionProbes( enemy );
    }
    
    if ( checkCollisionEnemyBlock( enemy, farWall, false ) == ENEMY_COLLISION_ALL ) {
        enemy->pos.z = farWall->pos.z + farWall->dim.z / 2 + enemy->dim.z / 2;
        enemy->vel.z = -enemy->vel.z;
        updateEnemyCollisionProbes( enemy );
    }
    
    if ( checkCollisionEnemyBlock( enemy, nearWall, false ) == ENEMY_COLLISION_ALL ) {
        enemy->pos.z = nearWall->pos.z - nearWall->dim.z / 2 - enemy->dim.z / 2;
        enemy->vel.z = -enemy->vel.z;
        updateEnemyCollisionProbes( enemy );
    }

}

void resolveCollisionPlayerEnemy( Player *player, Enemy *enemy ) {

    if ( enemy->state == ENEMY_STATE_ALIVE ) {

        PlayerCollisionType coll = checkCollisionPlayerEnemy( player, enemy, true );

        if ( !player->immortal && coll != PLAYER_COLLISION_ALL && coll != PLAYER_COLLISION_NONE ) {
            player->currentHp -= enemy->damageOnContact;
            if ( player->currentHp == 0 ) {
                player->state = PLAYER_STATE_DEAD;
            }
        }

        switch ( coll ) {
            case PLAYER_COLLISION_LEFT:
                player->pos.x = enemy->pos.x + enemy->dim.x / 2 + player->dim.x / 2;
                break;
            case PLAYER_COLLISION_RIGHT:
                player->pos.x = enemy->pos.x - enemy->dim.x / 2 - player->dim.x / 2;
                break;
            case PLAYER_COLLISION_BOTTOM:
                player->pos.y = enemy->pos.y + enemy->dim.y / 2 + player->dim.y / 2;
                player->vel.y = 0.0f;
                break;
            case PLAYER_COLLISION_TOP:
                player->pos.y = enemy->pos.y - enemy->dim.y / 2 - player->dim.y / 2 - 0.05f;
                player->vel.y = 0.0f;
                break;
            case PLAYER_COLLISION_FAR:
                player->pos.z = enemy->pos.z + enemy->dim.z / 2 + player->dim.z / 2;
                break;
            case PLAYER_COLLISION_NEAR:
                player->pos.z = enemy->pos.z - enemy->dim.z / 2 - player->dim.z / 2;
                break;
            case PLAYER_COLLISION_ALL:
            case PLAYER_COLLISION_NONE:
            default:
                break;
        }

    }
    
}

void resolveCollisionBulletWorld( GameWorld *gw ) {

    Block *ground = &gw->ground;
    Bullet *bullets = gw->player.bullets;

    for ( int i = 0; i < gw->player.bulletQuantity; i++ ) {

        Bullet *bullet = &bullets[i];

        // scenario
        if ( checkCollisionBulletBlock( bullet, ground ) || 
                checkCollisionBulletBlock( bullet, &gw->leftWall ) ||
                checkCollisionBulletBlock( bullet, &gw->rightWall ) ||
                checkCollisionBulletBlock( bullet, &gw->farWall ) ||
                checkCollisionBulletBlock( bullet, &gw->nearWall ) ) {
            bullet->collided = true;
        } else {
            for ( int j = 0; j < gw->obstacleQuantity; j++ ) {
                if ( checkCollisionBulletBlock( bullet, &gw->obstacles[j] ) ) {
                    bullet->collided = true;
                    break;
                }
            }
        }

        // enemies
        if ( !bullet->collided ) {

            for ( int j = 0; j < gw->enemyQuantity; j++ ) {

                Enemy *enemy = &gw->enemies[j];

                if ( enemy->state == ENEMY_STATE_ALIVE && 
                     checkCollisionBulletEnemy( bullet, enemy ) ) {
                    
                    bullet->collided = true;

                    if ( enemy->currentHp > 0 ) {
                        enemy->currentHp--;
                        enemy->showHpBar = true;
                        if ( enemy->currentHp == 0 ) {
                            enemy->state = ENEMY_STATE_DEAD;
                            cleanDeadEnemies( gw );
                        }
                    }

                    break;

                }

            }

        }

        // out of bounds
        if ( !bullet->collided ) {
            if ( bullet->pos.x * bullet->pos.x +
                 bullet->pos.y * bullet->pos.y +
                 bullet->pos.z * bullet->pos.z > 
                 ( gw->ground.dim.x * gw->ground.dim.x * 4 ) ) {
                bullet->collided = true;
            }
        }

        if ( bullet->collided ) {
            cleanCollidedBullets( &gw->player );
        }

    }

}

void resolveCollisionPlayerPowerUp( Player *player, PowerUp *powerUp ) {

    PlayerCollisionType coll = checkCollisionPlayerPowerUp( player, powerUp );

    if ( coll == PLAYER_COLLISION_ALL ) {
        playerAcquirePowerUp( player, powerUp );
    }
    
}

void resetGameWorld( GameWorld *gw ) {

    xCam = 0;
    yCam = 25.0f;
    zCam = 30.0f;

    gw->previousMousePos = (Vector2){0};

    destroyPlayerModel( &gw->player );
    gw->player = createPlayer();
    updatePlayerCollisionProbes( &gw->player );

    destroyEnemiesModel( gw->enemies, gw->enemyQuantity );
    free( gw->enemies );
    createEnemies( gw, enemyColor, enemyEyeColor );

    destroyPowerUpsModel( gw->powerUps, gw->powerUpQuantity );
    free( gw->powerUps );
    createPowerUps( gw );

    for ( int i = 0; i < gw->obstacleQuantity; i++ ) {
        gw->obstacles[i].touchColor = gw->obstacles[i].color;
    }
    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        gw->enemies[i].state = ENEMY_STATE_ALIVE;
    }

    gw->cameraType = CAMERA_TYPE_FIRST_PERSON;
    updateCameraTarget( gw, &gw->player );
    updateCameraPosition( gw, &gw->player, xCam, yCam, zCam );

}