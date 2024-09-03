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
#include <string.h>
#include <ctype.h>

#include "Types.h"
#include "GameWorld.h"
#include "PowerUp.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Block.h"
#include "raylib.h"
#include "raymath.h"

//#include "raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raygui.h"              // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

#define MAX_HITS 100

// extern from GameWorld.h
const float GRAVITY = 50.0f;

// id generation - extern from Types.h
int entityIdCounter = 1;

const int GAMEPAD_ID = 0;
const int CAMERA_TYPE_QUANTITY = 2;
const int WEAPON_TYPE_QUANTITY = 3;
const float FIRST_PERSON_CAMERA_TARGET_DIST = 30.0f;

bool loadTestMap = true;
const char *TEST_MAP_FILENAME = "testMap.txt";

const CameraType DEFAULT_CAMERA_TYPE = CAMERA_TYPE_FIRST_PERSON;
//const CameraType DEFAULT_CAMERA_TYPE = CAMERA_TYPE_THIRD_PERSON_FIXED;
const GameWorldPlayerInputType DEFAULT_INPUT_TYPE = GAME_WORLD_PLAYER_INPUT_TYPE_GAMEPAD;
//const GameWorldPlayerInputType DEFAULT_INPUT_TYPE = GAME_WORLD_PLAYER_INPUT_TYPE_KEYBOARD;

// globals
bool showDebugInfo = true;
bool drawWalls = true;

IdentifiedRayCollision hits[MAX_HITS];
IdentifiedRayCollision currentHit = {0};
MultipleIdentifiedRayCollision currentMultipleHit = {0};
int hitCounter = 0;

float xCam;
float yCam;
float zCam;

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {
    GameWorld *gw = (GameWorld*) calloc( 1, sizeof( GameWorld ) );
    configureGameWorld( gw );
    return gw;
}

void configureGameWorld( GameWorld *gw ) {

    float blockSize = 2.0f;
    
    //Color wallColor = Fade( DARKGREEN, 0.5f );
    Color wallColor = DARKBLUE;
    Color obstacleColor = LIME;
    //Color obstacleColor = Fade( LIME, 0.8f );
    Color enemyColor = RED;
    Color enemyEyeColor = (Color){ 38, 0, 82, 255 };
    Color bulletColor = WHITE;

    xCam = 0.0f;
    yCam = 25.0f;
    zCam = 1.0f;

    gw->maxCollidedBullets = 50;
    gw->collidedBulletCount = 0;
    gw->bulletColor = bulletColor;

    if ( loadTestMap ) {
        processMapFile( TextFormat( "resources/maps/%s", TEST_MAP_FILENAME ), gw, blockSize, wallColor, obstacleColor, enemyColor, enemyEyeColor );
    } else {
        processMapFile( "resources/maps/map1.txt", gw, blockSize, wallColor, obstacleColor, enemyColor, enemyEyeColor );
    }

    gw->cameraType = DEFAULT_CAMERA_TYPE;
    setupCamera( gw );
    updateCameraTarget( gw, &gw->player );
    updateCameraPosition( gw, &gw->player, xCam, yCam, zCam );

    gw->playerInputType = DEFAULT_INPUT_TYPE;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    free( gw->enemies );
    free( gw->powerUps );
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

        if ( gw->playerInputType == GAME_WORLD_PLAYER_INPUT_TYPE_KEYBOARD ) {
            processPlayerInputByKeyboard( gw, player, gw->cameraType, delta );
        } else if ( gw->playerInputType == GAME_WORLD_PLAYER_INPUT_TYPE_GAMEPAD ) {
            processPlayerInputByGamepad( gw, player, gw->cameraType, delta );
        }
        
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

        updateCameraTarget( gw, &gw->player );
        updateCameraPosition( gw, &gw->player, xCam, yCam, zCam );

        /*if ( player->currentWeapon->type == WEAPON_TYPE_SHOTGUN ) {
            currentMultipleHit = resolveMultipleHitsWorld( gw );
        } else {
            currentHit = resolveHitsWorld( gw );
        }*/

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

    int collidedBullets = gw->collidedBulletCount < gw->maxCollidedBullets ? gw->collidedBulletCount : gw->maxCollidedBullets;
    for ( int i = 0; i < collidedBullets; i++ ) {
        drawBullet( &gw->collidedBullets[i] );
    }

    drawBlock( &gw->ground );
    drawPlayer( &gw->player );
    
    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        drawEnemy( &gw->enemies[i] );
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

    /*Player *player = &gw->player;
    if ( player->currentWeapon->type == WEAPON_TYPE_SHOTGUN ) {
        currentMultipleHit = resolveMultipleHitsWorld( gw );
    } else {
        currentHit = resolveHitsWorld( gw );
    }*/

    EndMode3D();

    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        drawEnemyHpBar( &gw->enemies[i], gw->camera );
    }

    drawPlayerHud( &gw->player );
    drawReticle( gw, gw->cameraType, gw->player.weaponState, 30 );

    if ( showDebugInfo ) {
        drawDebugInfo( gw );
    }

    if ( gw->player.state == PLAYER_STATE_DEAD ) {
        drawGameoverOverlay();
    }

    EndDrawing();

}

void drawReticle( GameWorld *gw, CameraType cameraType, PlayerWeaponState weaponState, int reticleSize ) {

    if ( cameraType == CAMERA_TYPE_FIRST_PERSON ) {

        Vector2 v = {0};
        IdentifiedRayCollision reticleHit = resolveHitsWorld( gw );

        if ( reticleHit.entityType == ENTITY_TYPE_NONE ) {
            v = GetWorldToScreen( gw->camera.target, gw->camera );
        } else {
            v = GetWorldToScreen( reticleHit.collision.point, gw->camera );
        }

        Color reticleColor = weaponState == PLAYER_WEAPON_STATE_READY ? RED : BLACK;
        DrawLine( v.x - reticleSize, v.y, v.x + reticleSize, v.y, reticleColor );
        DrawLine( v.x, v.y - reticleSize, v.x, v.y + reticleSize, reticleColor );

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

    float cosH = cos( DEG2RAD * player->rotationHorizontalAngle );
    float sinH = -sin( DEG2RAD * player->rotationHorizontalAngle );
    float cosV = cos( DEG2RAD * player->rotationVerticalAngle );
    //float sinV = -sin( DEG2RAD * player->rotationVerticalAngle );

    switch ( gw->cameraType ) {
        case CAMERA_TYPE_THIRD_PERSON_FIXED:
            gw->camera.target = player->pos;
            break;
        case CAMERA_TYPE_FIRST_PERSON:
            if ( gw->playerInputType == GAME_WORLD_PLAYER_INPUT_TYPE_KEYBOARD ) {
                Vector2 mouseDelta = GetMouseDelta();
                player->rotationHVel = mouseDelta.x * delta * -player->rotationSpeed * 10;
                player->rotationVVel = mouseDelta.y * delta * player->rotationSpeed * 10;
                HideCursor();
            } else {
                ShowCursor();
            }
            gw->camera.target.x = player->pos.x + cosH * FIRST_PERSON_CAMERA_TARGET_DIST;
            gw->camera.target.y = player->pos.y + cosV * FIRST_PERSON_CAMERA_TARGET_DIST;
            gw->camera.target.z = player->pos.z + sinH * FIRST_PERSON_CAMERA_TARGET_DIST;
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
        case CAMERA_TYPE_FIRST_PERSON:
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

Block createGround( float thickness, int lines, int columns ) {

    Block ground = {
        .id = entityIdCounter++,
        .pos = {
            .x = -1.0f,
            .y = -1.0f,
            .z = -1.0f
        },
        .dim = {
            .x = columns, 
            .y = thickness,
            .z = lines
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

void createObstacles( GameWorld *gw, Vector3 *positions, int obstacleQuantity, float blockSize, Color obstacleColor ) {

    gw->obstacleQuantity = obstacleQuantity;
    gw->obstacles = (Block*) malloc( sizeof( Block ) * gw->obstacleQuantity );

    for ( int i = 0; i < gw->obstacleQuantity; i++ ) {
        gw->obstacles[i] = (Block){
            .id = entityIdCounter++,
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

void createGroundModel( Block *ground ) {

    if ( !rm.groundModelCreated ) {

        Mesh mesh = GenMeshCube( ground->dim.x, ground->dim.y, ground->dim.z );
        Model model = LoadModelFromMesh( mesh );

        Image img = GenImageChecked( ground->dim.x, ground->dim.z, 2, 2, ORANGE, (Color){ 192, 96, 0, 255 } );
        Texture2D texture = LoadTextureFromImage( img );
        UnloadImage( img );

        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
        rm.groundModel = model;
        rm.groundModelCreated = true;

    }

    ground->renderModel = true;
    ground->model = rm.groundModel;

}

void createObstaclesModel( Block *obstacles, int obstaclesQuantity ) {

    if ( !rm.obstacleModelCreated ) {

        Block *baseObstacle = &obstacles[0];

        Mesh mesh = GenMeshCube( baseObstacle->dim.x, baseObstacle->dim.y, baseObstacle->dim.z );
        Model model = LoadModelFromMesh( mesh );

        Image img = GenImageChecked( 2, 2, 1, 1, WHITE, LIGHTGRAY );
        Texture2D texture = LoadTextureFromImage( img );
        UnloadImage( img );

        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

        rm.obstacleModel = model;
        rm.obstacleModelCreated = true;

    }

    for ( int i = 0; i < obstaclesQuantity; i++ ) {
        obstacles[i].renderModel = true;
        obstacles[i].model = rm.obstacleModel;
    }

}

void createWalls( GameWorld *gw, Color wallColor, int groundLines, int groundColumns, int wallHeight ) {

    gw->leftWall = (Block){
        .id = entityIdCounter++,
        .pos = {
            .x = - ( groundColumns / 2 + 2 ),
            .y = wallHeight / 2,
            .z = -1.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = wallHeight,
            .z = groundLines
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->rightWall = (Block){
        .id = entityIdCounter++,
        .pos = {
            .x = groundColumns / 2,
            .y = wallHeight / 2,
            .z = -1.0f
        },
        .dim = {
            .x = 2.0f, 
            .y = wallHeight,
            .z = groundLines
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->farWall = (Block){
        .id = entityIdCounter++,
        .pos = {
            .x = -1.0f,
            .y = wallHeight / 2,
            .z = - ( groundLines / 2 + 2 )
        },
        .dim = {
            .x = groundColumns, 
            .y = wallHeight,
            .z = 2.0f
        },
        .color = wallColor,
        .visible = true,
        .renderModel = false,
        .renderTouchColor = false
    };

    gw->nearWall = (Block){
        .id = entityIdCounter++,
        .pos = {
            .x = -1.0f,
            .y = wallHeight / 2,
            .z = groundLines / 2
        },
        .dim = {
            .x = groundColumns, 
            .y = wallHeight,
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
        showDebugInfo = !showDebugInfo;
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
         ( IsGamepadAvailable( GAMEPAD_ID ) && IsGamepadButtonPressed( GAMEPAD_ID, GAMEPAD_BUTTON_MIDDLE_RIGHT ) ) ) {
        resetGameWorld( gw );
    }

    if ( IsKeyPressed( KEY_F ) ) {
        int ct = gw->cameraType + 1;
        gw->cameraType = ct % CAMERA_TYPE_QUANTITY;
    }

    if ( IsKeyPressed( KEY_I ) ) {
        player->immortal = !player->immortal;
    }

    if ( IsKeyPressed( KEY_L ) ) {
        loadTestMap = !loadTestMap;
        resetGameWorld( gw );
    }

    if ( IsKeyPressed( KEY_K ) ) {
        if ( gw->playerInputType == GAME_WORLD_PLAYER_INPUT_TYPE_GAMEPAD ) {
            gw->playerInputType = GAME_WORLD_PLAYER_INPUT_TYPE_KEYBOARD;
        } else {
            gw->playerInputType = GAME_WORLD_PLAYER_INPUT_TYPE_GAMEPAD;
        }
    }

}

void processPlayerInputByKeyboard( GameWorld *gw, Player *player, CameraType cameraType, float delta ) {

    if ( cameraType == CAMERA_TYPE_THIRD_PERSON_FIXED ) {
        if ( IsKeyDown( KEY_UP ) ) {
            yCam += 1;
        } else if ( IsKeyDown( KEY_DOWN ) ) {
            yCam -= 1;
        } else if ( IsKeyDown( KEY_LEFT ) ) {
            xCam -= 1;
        } else if ( IsKeyDown( KEY_RIGHT ) ) {
            xCam += 1;
        } else if ( IsKeyDown( KEY_KP_SUBTRACT ) ) {
            zCam -= 1;
        } else if ( IsKeyDown( KEY_KP_ADD ) ) {
            zCam += 1;
        }
    }

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
        if ( cameraType == CAMERA_TYPE_FIRST_PERSON ) {
            player->vel.x = cos( DEG2RAD * ( player->rotationHorizontalAngle + 90 ) ) * player->speed;
            player->vel.z = -sin( DEG2RAD * ( player->rotationHorizontalAngle + 90 ) ) * player->speed;
        } else {
            player->rotationHVel = player->rotationSpeed;
        }
    } else if ( IsKeyDown( KEY_D ) ) {
        if ( cameraType == CAMERA_TYPE_FIRST_PERSON ) {
            player->vel.x = cos( DEG2RAD * ( player->rotationHorizontalAngle - 90 ) ) * player->speed;
            player->vel.z = -sin( DEG2RAD * ( player->rotationHorizontalAngle - 90 ) ) * player->speed;
        } else {
            player->rotationHVel = -player->rotationSpeed;
        }
    } else {
        if ( cameraType == CAMERA_TYPE_THIRD_PERSON_FIXED ) {
            player->rotationHVel = 0.0f;
            player->rotationVVel = 0.0f;
        }
    }
            
    if ( IsKeyPressed( KEY_SPACE ) ) {
        jumpPlayer( player );
    }

    if ( cameraType == CAMERA_TYPE_FIRST_PERSON ) {
        if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {
            player->weaponState = PLAYER_WEAPON_STATE_READY;
            currentHit = resolveHitsWorld( gw );
            playerShotMachinegun( gw, player, &currentHit );
        } else {
            player->weaponState = PLAYER_WEAPON_STATE_IDLE;
        }
    } else {
        if ( IsKeyDown( KEY_LEFT_ALT ) ) {
            player->weaponState = PLAYER_WEAPON_STATE_READY;
            currentHit = resolveHitsWorld( gw );
            playerShotMachinegun( gw, player, &currentHit );
        } else {
            player->weaponState = PLAYER_WEAPON_STATE_IDLE;
        }
    }

}

void processPlayerInputByGamepad( GameWorld *gw, Player *player, CameraType cameraType, float delta ) {

    if ( IsGamepadAvailable( GAMEPAD_ID ) ) {

        // standard
        float gpxLeft = GetGamepadAxisMovement( GAMEPAD_ID, GAMEPAD_AXIS_LEFT_X );
        float gpyLeft = GetGamepadAxisMovement( GAMEPAD_ID, GAMEPAD_AXIS_LEFT_Y );

        float gpxRight = GetGamepadAxisMovement( GAMEPAD_ID, GAMEPAD_AXIS_RIGHT_X );
        float gpyRight = GetGamepadAxisMovement( GAMEPAD_ID, GAMEPAD_AXIS_RIGHT_Y );

        if ( IsGamepadButtonPressed( GAMEPAD_ID, GAMEPAD_BUTTON_LEFT_THUMB ) ) {
            player->speed = player->runningSpeed;
        }

        if ( cameraType == CAMERA_TYPE_THIRD_PERSON_FIXED ) {
            player->vel.x = player->speed * gpxLeft;
            player->vel.z = player->speed * gpyLeft;
            if ( player->vel.x != 0.0f || player->vel.z != 0.0f ) {
                player->rotationHorizontalAngle = RAD2DEG * atan2( -player->vel.z, player->vel.x );
            }
        } else {
            player->rotationHVel = gpxRight * delta * -player->rotationSpeed * 100;
            player->rotationVVel = gpyRight * delta * -player->rotationSpeed * 100;
            float xMultiplier = ( ( -cos( DEG2RAD * player->rotationHorizontalAngle ) * gpyLeft ) +
                                ( -cos( DEG2RAD * ( player->rotationHorizontalAngle + 90 ) ) * gpxLeft ) ) / 2.0f;
            float zMultiplier = ( ( sin( DEG2RAD * player->rotationHorizontalAngle ) * gpyLeft ) +
                                ( sin( DEG2RAD * ( player->rotationHorizontalAngle + 90 ) ) * gpxLeft ) ) / 2.0f;
            player->vel.x = player->speed * xMultiplier;
            player->vel.z = player->speed * zMultiplier;
        }

        if ( IsGamepadButtonPressed( GAMEPAD_ID, GAMEPAD_BUTTON_RIGHT_FACE_DOWN ) ) {
            jumpPlayer( player );
        }

        if ( IsGamepadButtonDown( GAMEPAD_ID, GAMEPAD_BUTTON_LEFT_TRIGGER_2 ) ) {

            player->weaponState = PLAYER_WEAPON_STATE_READY;

            switch ( player->currentWeapon->type ) {
                case WEAPON_TYPE_HANDGUN:
                    if ( IsGamepadButtonPressed( GAMEPAD_ID, GAMEPAD_BUTTON_RIGHT_TRIGGER_2 ) ) {
                        currentHit = resolveHitsWorld( gw );
                        playerShotHandgun( gw, player, &currentHit );
                    }
                    break;
                case WEAPON_TYPE_SUBMACHINEGUN:
                    if ( IsGamepadButtonDown( GAMEPAD_ID, GAMEPAD_BUTTON_RIGHT_TRIGGER_2 ) ) {
                        currentHit = resolveHitsWorld( gw );
                        playerShotMachinegun( gw, player, &currentHit );
                    }
                    break;
                case WEAPON_TYPE_SHOTGUN:
                    if ( IsGamepadButtonPressed( GAMEPAD_ID, GAMEPAD_BUTTON_RIGHT_TRIGGER_2 ) ) {
                        currentMultipleHit = resolveMultipleHitsWorld( gw );
                        playerShotShotgun( gw, player, &currentMultipleHit );
                    }
                    break;
            }

        } else {
            player->weaponState = PLAYER_WEAPON_STATE_IDLE;
        }

        if ( IsGamepadButtonPressed( GAMEPAD_ID, GAMEPAD_BUTTON_RIGHT_FACE_UP ) ) {
            int currentWeaponType = player->currentWeapon->type;
            currentWeaponType++;
            currentWeaponType %= WEAPON_TYPE_QUANTITY;
            switch ( currentWeaponType ) {
                case WEAPON_TYPE_HANDGUN:
                    player->currentWeapon = &player->handgun;
                    break;
                case WEAPON_TYPE_SUBMACHINEGUN:
                    player->currentWeapon = &player->submachinegun;
                    break;
                case WEAPON_TYPE_SHOTGUN:
                    player->currentWeapon = &player->shotgun;
                    break;
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

void resolveCollisionPlayerPowerUp( Player *player, PowerUp *powerUp ) {

    PlayerCollisionType coll = checkCollisionPlayerPowerUp( player, powerUp );

    if ( coll == PLAYER_COLLISION_ALL ) {
        playerAcquirePowerUp( player, powerUp );
    }
    
}

// returns the identifier of the closest enemy or zero if the collisions
// are not against enemies or there was no collision
IdentifiedRayCollision resolveHitsWorld( GameWorld *gw ) {

    Ray ray = getPlayerToVector3Ray( &gw->player, gw->camera.target );
    hitCounter = 0;

    Block *b[5] = {
        &gw->ground,
        &gw->leftWall,
        &gw->rightWall,
        &gw->farWall,
        &gw->nearWall
    };

    for ( int i = 0; i < 5; i++ ) {
        RayCollision rc = GetRayCollisionBox( ray, getBlockBoundingBox( b[i] ) );  
        if ( rc.hit && hitCounter < MAX_HITS ) {
            hits[hitCounter++] = (IdentifiedRayCollision) {
                .entityId = b[i]->id,
                .entityType = ENTITY_TYPE_BLOCK,
                .collision = rc
            };
        }
    }

    for ( int i = 0; i < gw->obstacleQuantity; i++ ) {
        RayCollision rc = GetRayCollisionBox( ray, getBlockBoundingBox( &gw->obstacles[i] ) );  
        if ( rc.hit && hitCounter < MAX_HITS ) {
            hits[hitCounter++] = (IdentifiedRayCollision) {
                .entityId = gw->obstacles[i].id,
                .entityType = ENTITY_TYPE_OBSTACLE,
                .collision = rc
            };
        }
    }

    for ( int i = 0; i < gw->enemyQuantity; i++ ) {
        Enemy *e = &gw->enemies[i];
        RayCollision rc = GetRayCollisionMesh( ray, e->model.meshes[0], getEnemyTransformMatrix( e ) );
        if ( rc.hit && hitCounter < MAX_HITS ) {
            hits[hitCounter++] = (IdentifiedRayCollision) {
                .entityId = gw->enemies[i].id,
                .entityType = ENTITY_TYPE_ENEMY,
                .collision = rc
            };
        }
    }

    qsort( hits, hitCounter, sizeof( IdentifiedRayCollision ), compareRaycollision );
    if ( hitCounter > 0 ) {
        return hits[0];
    }

    return (IdentifiedRayCollision){0};

}

MultipleIdentifiedRayCollision resolveMultipleHitsWorld( GameWorld *gw ) {

    int pelletQuantity = 20;
    Player *player = &gw->player;
    Vector3 centralTraget = gw->camera.target;

    MultipleIdentifiedRayCollision mirc = {0};
    //DrawSphere( centralTraget, 0.5, BLACK );

    for ( int i = 0; i < pelletQuantity; i++ ) {

        Vector3 cPos = centralTraget;

        /*float spreadRadius = 2.0f;
        float spreadAngle = 30 * i;*/

        float spreadRadius = (float) GetRandomValue( 1, 5 ) / 2.0f;
        float spreadAngle = (float) GetRandomValue( 0, 360 ) * i;

        // first rotation
        cPos.y += sin( DEG2RAD * spreadAngle ) * spreadRadius;
        cPos.z += cos( DEG2RAD * spreadAngle ) * spreadRadius;

        // second rotation
        float c1 = cPos.x - centralTraget.x;
        float c2 = cPos.z - centralTraget.z;
        float centerRadius = sqrt( c1 * c1 + c2 * c2 );
        float centerAngle = RAD2DEG * atan2( cPos.z - centralTraget.z, cPos.x - centralTraget.x );
        cPos.x = centralTraget.x + cos( DEG2RAD * ( centerAngle - player->rotationHorizontalAngle ) ) * centerRadius;
        cPos.z = centralTraget.z + sin( DEG2RAD * ( centerAngle - player->rotationHorizontalAngle ) ) * centerRadius;

        //DrawSphere( cPos, 0.5, BLACK );

        Ray ray = getPlayerToVector3Ray( &gw->player, cPos );
        hitCounter = 0;

        Block *b[5] = {
            &gw->ground,
            &gw->leftWall,
            &gw->rightWall,
            &gw->farWall,
            &gw->nearWall
        };

        for ( int i = 0; i < 5; i++ ) {
            RayCollision rc = GetRayCollisionBox( ray, getBlockBoundingBox( b[i] ) );  
            if ( rc.hit && hitCounter < MAX_HITS ) {
                hits[hitCounter++] = (IdentifiedRayCollision) {
                    .entityId = b[i]->id,
                    .entityType = ENTITY_TYPE_BLOCK,
                    .collision = rc
                };
            }
        }

        for ( int i = 0; i < gw->obstacleQuantity; i++ ) {
            RayCollision rc = GetRayCollisionBox( ray, getBlockBoundingBox( &gw->obstacles[i] ) );  
            if ( rc.hit && hitCounter < MAX_HITS ) {
                hits[hitCounter++] = (IdentifiedRayCollision) {
                    .entityId = gw->obstacles[i].id,
                    .entityType = ENTITY_TYPE_OBSTACLE,
                    .collision = rc
                };
            }
        }

        for ( int i = 0; i < gw->enemyQuantity; i++ ) {
            Enemy *e = &gw->enemies[i];
            RayCollision rc = GetRayCollisionMesh( ray, e->model.meshes[0], getEnemyTransformMatrix( e ) );
            if ( rc.hit && hitCounter < MAX_HITS ) {
                hits[hitCounter++] = (IdentifiedRayCollision) {
                    .entityId = gw->enemies[i].id,
                    .entityType = ENTITY_TYPE_ENEMY,
                    .collision = rc
                };
            }
        }

        qsort( hits, hitCounter, sizeof( IdentifiedRayCollision ), compareRaycollision );
        if ( hitCounter > 0 ) {
            IdentifiedRayCollision rc = hits[0];
            mirc.irCollisions[mirc.quantity] = rc;
            mirc.quantity++;
        }

    }

    return mirc;

}

int compareRaycollision( const void *pr1, const void *pr2 ) {
    IdentifiedRayCollision *r1 = (IdentifiedRayCollision*) pr1;
    IdentifiedRayCollision *r2 = (IdentifiedRayCollision*) pr2;
    if ( r1->collision.distance < r2->collision.distance ) {
        return -1;
    } else if ( r1->collision.distance > r2->collision.distance ) {
        return 1;
    }
    return 0;
}

void resetGameWorld( GameWorld *gw ) {
    free( gw->enemies );
    free( gw->powerUps );
    free( gw->obstacles );
    unloadModelsResourceManager();
    configureGameWorld( gw );
}

void drawDebugInfo( GameWorld *gw ) {

    DrawFPS( 10, 10 );
    DrawText( TextFormat( "player: x=%.1f, y=%.1f, z=%.1f", gw->player.pos.x, gw->player.pos.y, gw->player.pos.z ), 10, 30, 20, BLACK );
    DrawText( TextFormat( "active enemies: %d", gw->enemyQuantity ), 10, 50, 20, BLACK );
    DrawText( TextFormat( "active power-ups: %d", gw->powerUpQuantity ), 10, 70, 20, BLACK );
    DrawText( TextFormat( "input type: %s", gw->playerInputType == GAME_WORLD_PLAYER_INPUT_TYPE_GAMEPAD ? "gamepad" : "keyboard" ), 10, 90, 20, BLACK );
    DrawText( TextFormat( "weapon type: %s", gw->player.currentWeapon->name ), 10, 110, 20, BLACK );
    showCameraInfo( &gw->camera, 10, 130 );

    // draw collision points with raycast (debug)
    if ( gw->cameraType == CAMERA_TYPE_FIRST_PERSON ) {

        Color colors[] = { BLACK, WHITE, DARKPURPLE, GREEN, BLUE, YELLOW, ORANGE, DARKGRAY };

        for ( int i = 0; i < hitCounter; i++ ) {

            Vector2 v = GetWorldToScreen( hits[i].collision.point, gw->camera );
            Color c = i < 8 ? colors[i] : BLACK;
            float d = i == 0 ? 2 : hits[i].collision.distance * 100;
            
            DrawCircleLinesV( v, d, c );
            DrawText( TextFormat( "%d", hits[i].entityId ), v.x + d + 10, v.y, 20, c );
            DrawText( TextFormat( "%.2f", hits[i].collision.distance ), v.x, v.y + d + 10, 20, c );

        }

    }

}

void drawGameoverOverlay( void ) {
    DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), Fade( BLACK, 0.85f ) );
    int fontSizeDead = 60;
    int fontSizeReset = 20;
    const char *tDead = "YOU DIED!";
    const char *tReset = "Start/<R> to Reset!";
    int wDead = MeasureText( tDead, fontSizeDead );
    int wReset = MeasureText( tReset, fontSizeReset );
    DrawText( tDead, GetScreenWidth() / 2 - wDead / 2, GetScreenHeight() / 2 - fontSizeDead / 2 - 10, fontSizeDead, RED );
    DrawText( tReset, GetScreenWidth() / 2 - wReset / 2, GetScreenHeight() / 2 - fontSizeReset / 2 + 30, fontSizeReset, RED );
}

void processMapFile( const char *filePath, GameWorld *gw, float blockSize, Color wallColor, Color obstacleColor, Color enemyColor, Color enemyEyeColor ) {

    char *data = LoadFileText( filePath );
    int line = 0;
    int column = 0;
    int currentY = -1;

    char intBuffer[20];
    int bufferPos = 0;

    char parsedData[4][20];
    int parsedDataLimit = 4;
    int parsedDataPos = 0;

    int playerLine = 0;
    int playerColumn = 0;
    int playerY = 0;

    int oCounter = 0;
    int eCounter = 0;
    int pCounter = 0;

    Vector3 obstaclePositions[1000];
    Vector3 enemyPositions[100];
    Vector3 powerUpPositions[100];
    PowerUpType powerUpTypes[100];

    while ( *data != '\0' ) {

        char c = *data;

        if ( c == '\n' ) {
            line++;
            column = 0;
            intBuffer[bufferPos] = '\0';
            if ( bufferPos > 0 ) {
                if ( parsedDataPos < parsedDataLimit ) {
                    strcpy( parsedData[parsedDataPos++], intBuffer );
                }
            }
            bufferPos = 0;
        } else {

            if ( isdigit( c ) ) {
                intBuffer[bufferPos++] = c;
            } else {

                intBuffer[bufferPos] = '\0';
                if ( bufferPos > 0 ) {
                    if ( parsedDataPos < parsedDataLimit ) {
                        strcpy( parsedData[parsedDataPos++], intBuffer );
                    }
                }
                intBuffer[0] = '\0';
                bufferPos = 0;

                switch ( c ) {
                    case 'P':
                        playerLine = line;
                        playerColumn = column;
                        playerY = currentY;
                        break;
                    case 'Y':
                        currentY += 2;
                        line = 0;
                        break;
                    case 'O':
                        obstaclePositions[oCounter++] = (Vector3) { 
                            column, 
                            currentY, 
                            line
                        };
                        break;
                    case 'E':
                        enemyPositions[eCounter++] = (Vector3) { column, currentY, line };
                        break;
                    case 'H':
                        powerUpPositions[pCounter] = (Vector3) { column, currentY, line };
                        powerUpTypes[pCounter++] = POWER_UP_TYPE_HP;
                        break;
                    case 'A':
                        powerUpPositions[pCounter] = (Vector3) { column, currentY, line };
                        powerUpTypes[pCounter++] = POWER_UP_TYPE_AMMO;
                        break;
                }

            }

            column++;

        }

        data++;

    }

    int groundLines = atoi( parsedData[0] );
    int groundColumns = atoi( parsedData[1] );
    int wallHeight = atoi( parsedData[2] );
    int playerStartAngle = atoi( parsedData[3] );

    gw->ground = createGround( 2.0f, groundLines, groundColumns );
    createWalls( gw, wallColor, groundLines, groundColumns, wallHeight );

    int columnAdjust = -groundColumns/2 - 1;
    int lineAdjust = -groundLines/2 - 2;

    float playerX = (float) (columnAdjust + playerColumn);
    float playerZ = (float) (lineAdjust + playerLine);

    gw->player = createPlayer( (Vector3){
        .x = playerX,
        .y = (float) playerY,
        .z = playerZ
    });
    gw->player.currentWeapon = &gw->player.handgun;
    gw->player.rotationHorizontalAngle = playerStartAngle;

    for ( int i = 0; i < eCounter; i++ ) {
        enemyPositions[i].x += columnAdjust;
        enemyPositions[i].z += lineAdjust;
    }

    for ( int i = 0; i < pCounter; i++ ) {
        powerUpPositions[i].x += columnAdjust;
        powerUpPositions[i].z += lineAdjust;
    }

    for ( int i = 0; i < oCounter; i++ ) {
        obstaclePositions[i].x += columnAdjust;
        obstaclePositions[i].z += lineAdjust;
    }

    createEnemies( gw, enemyPositions, eCounter, enemyColor, enemyEyeColor );
    createPowerUps( gw, powerUpPositions, powerUpTypes, pCounter );
    createObstacles( gw, obstaclePositions, oCounter, blockSize, obstacleColor );

}