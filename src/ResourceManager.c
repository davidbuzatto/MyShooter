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
}