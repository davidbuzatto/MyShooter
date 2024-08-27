/**
 * @file ResourceManager.c
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "ResourceManager.h"

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {
    //rm.textureExample = LoadTexture( "resources/images/mario.png" );
    //rm.soundExample = LoadSound( "resources/sfx/powerUp.wav" );
    //rm.musicExample = LoadMusicStream( "resources/musics/overworld1.ogg" );
}

void unloadResourcesResourceManager( void ) {
    //UnloadTexture( rm.textureExample );
    //UnloadSound( rm.soundExample );
    //UnloadMusicStream( rm.musicExample );
    unloadModelsResourceManager();
}

void unloadModelsResourceManager( void ) {

    if ( rm.playerModelCreated ) {
        UnloadTexture( rm.playerModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
        UnloadModel( rm.playerModel );
        rm.playerModelCreated = false;
    }

    if ( rm.powerUpModelCreated ) {
        UnloadTexture( rm.powerUpModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
        UnloadModel( rm.powerUpModel );
        rm.powerUpModelCreated = false;
    }

    if ( rm.enemyModelCreated ) {
        UnloadTexture( rm.enemyModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
        UnloadModel( rm.enemyModel );
        rm.enemyModelCreated = false;
    }

    if ( rm.obstacleModelCreated ) {
        UnloadTexture( rm.obstacleModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
        UnloadModel( rm.obstacleModel );
        rm.obstacleModelCreated = false;
    }

    if ( rm.groundModelCreated ) {
        UnloadTexture( rm.groundModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
        UnloadModel( rm.groundModel );
        rm.groundModelCreated = false;
    }

}