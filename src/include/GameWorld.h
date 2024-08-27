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
extern const int CAMERA_TYPE_QUANTITY;

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void );

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
void drawReticle( CameraType cameraType, PlayerWeaponState weaponState, int reticleSize );

void setupCamera( GameWorld *gw );
void updateCameraTarget( GameWorld *gw, Player *player );
void updateCameraPosition( GameWorld *gw, Player *player, float xOffset, float yOffset, float zOffset );
void showCameraInfo( Camera3D *camera, int x, int y );

Block createGround( float blockSize, int lines, int columns );
void createGroundModel( Block *ground );
void destroyGroundModel( Block *ground );

void createObstacles( GameWorld *gw, float blockSize, Color obstacleColor );
void createObstaclesModel( Block *obstacles, int obstaclesQuantity );
void destroyObstaclesModel( Block *obstacles, int obstaclesQuantity );

void createWalls( GameWorld *gw, Color wallColor );

void processOptionsInput( Player *player, GameWorld *gw );
void processCameraInput( float *xCam, float *yCam, float *zCam );
void processPlayerInput( Player *player, CameraType cameraType, float delta, bool keyboard );

void resolveCollisionPlayerObstacles( Player *player, GameWorld *gw );
void resolveCollisionEnemyObstacles( Enemy *enemy, GameWorld *gw );
void resolveCollisionPlayerGround( Player *player, Block *ground );
void resolveCollisionEnemyGround( Enemy *enemy, Block *ground );
void resolveCollisionPowerUpGround( PowerUp *powerUp, Block *ground );
void resolveCollisionPlayerWalls( Player *player, Block *leftWall, Block *rightWall, Block *farWall, Block *nearWall );
void resolveCollisionEnemyWalls( Enemy *enemy, Block *leftWall, Block *rightWall, Block *farWall, Block *nearWall );
void resolveCollisionPlayerEnemy( Player *player, Enemy *enemy );
void resolveCollisionBulletWorld( GameWorld *gw );
void resolveCollisionPlayerPowerUp( Player *player, PowerUp *powerUp );

void resetGameWorld( GameWorld *gw );