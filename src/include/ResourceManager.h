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

    bool playerModelCreated;
    bool powerUpModelCreated;
    bool enemyModelCreated;
    bool obstacleModelCreated;
    bool groundModelCreated;

    /*Texture2D textureExample;
    Sound soundExample;
    Music musicExample;*/

    Sound handgunSound;
    Sound submachinegunSound;
    Sound shotgunSound;

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