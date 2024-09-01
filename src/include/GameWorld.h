/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld struct and function declarations.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "Types.h"
#include "raylib.h"

extern const float GRAVITY;

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void );
void configureGameWorld( GameWorld *gw );

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw );

/**
 * @brief Reads user input and updates the state of the game.
 */
void inputAndUpdateGameWorld( GameWorld *gw );

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw );
void drawReticle( GameWorld *gw, CameraType cameraType, PlayerWeaponState weaponState, int reticleSize );

void setupCamera( GameWorld *gw );
void updateCameraTarget( GameWorld *gw, Player *player );
void updateCameraPosition( GameWorld *gw, Player *player, float xOffset, float yOffset, float zOffset );
void showCameraInfo( Camera3D *camera, int x, int y );

Block createGround( float thickness, int lines, int columns );
void createObstacles( GameWorld *gw, Vector3 *positions, int obstacleQuantity, float blockSize, Color obstacleColor );

void createGroundModel( Block *ground );
void createObstaclesModel( Block *obstacles, int obstaclesQuantity );

void createWalls( GameWorld *gw, Color wallColor, int groundLines, int groundColumns, int wallHeight );

void processOptionsInput( Player *player, GameWorld *gw );
void processPlayerInputByKeyboard( GameWorld *gw, Player *player, CameraType cameraType, float delta );
void processPlayerInputByGamepad( GameWorld *gw, Player *player, CameraType cameraType, float delta );

void resolveCollisionPlayerObstacles( Player *player, GameWorld *gw );
void resolveCollisionEnemyObstacles( Enemy *enemy, GameWorld *gw );
void resolveCollisionPlayerGround( Player *player, Block *ground );
void resolveCollisionEnemyGround( Enemy *enemy, Block *ground );
void resolveCollisionPowerUpGround( PowerUp *powerUp, Block *ground );
void resolveCollisionPlayerWalls( Player *player, Block *leftWall, Block *rightWall, Block *farWall, Block *nearWall );
void resolveCollisionEnemyWalls( Enemy *enemy, Block *leftWall, Block *rightWall, Block *farWall, Block *nearWall );
void resolveCollisionPlayerEnemy( Player *player, Enemy *enemy );
void resolveCollisionPlayerPowerUp( Player *player, PowerUp *powerUp );
IdentifiedRayCollision resolveHitsWorld( GameWorld *gw );

void resetGameWorld( GameWorld *gw );
void drawDebugInfo( GameWorld *gw );
void drawGameoverOverlay( void );

void processMapFile( const char *filePath, GameWorld *gw, float blockSize, Color wallColor, Color obstacleColor, Color enemyColor, Color enemyEyeColor );
int compareRaycollision( const void *pr1, const void *pr2 );