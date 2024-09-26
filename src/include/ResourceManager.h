/**
 * @file ResourceManager.h
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager struct and function declarations.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include <stdlib.h>

#include "raylib.h"

typedef struct ResourceManager {

    Model playerModel;
    Model powerUpModel;
    Model enemyModel;
    Model obstacleModel;
    Model groundModel;
    Model lrWallModel;
    Model fnWallModel;

    bool playerModelCreated;
    bool powerUpModelCreated;
    bool enemyModelCreated;
    bool obstacleModelCreated;
    bool groundModelCreated;
    bool lrWallModelCreated;
    bool fnWallModelCreated;

    Texture2D explosion0;
    Texture2D explosion1;
    Texture2D explosion2;

    Shader lightShader;

    Sound handgunSound;
    Sound submachinegunSound;
    Sound shotgunSound;

    Sound enemyDeathSound01;
    Sound enemyDeathSound02;
    Sound enemyDeathSound03;

    Sound noAmmoWarningSound;

    Sound hpPowerUpSound;
    Sound ammoPowerUpSound;

    Sound playerJumpSound;
    Sound playerStepSound;

    Music bgMusicTestMap;
    Music bgMusicMap1;

} ResourceManager;

/**
 * @brief Global ResourceManager instance.
 */
extern ResourceManager rm;

/**
 * @brief Load global game resources, linking them in the global instance of
 * ResourceManager called rm.
 */
void loadResourcesResourceManager( void );

/**
 * @brief Unload global game resources.
 */
void unloadResourcesResourceManager( void );

void unloadModelsResourceManager( void );