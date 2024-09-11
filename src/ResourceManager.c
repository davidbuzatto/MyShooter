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
    //rm.musicExample = LoadMusicStream( "resources/musics/overworld1.ogg" );

    rm.lightShader = LoadShader( "resources/shaders/glsl330/lighting.vs", "resources/shaders/glsl330/lighting.fs" );
    rm.lightShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation( rm.lightShader, "viewPos" );

    rm.handgunSound = LoadSound( "resources/sfx/handgun.wav" );
    rm.submachinegunSound = LoadSound( "resources/sfx/submachinegun.wav" );
    rm.shotgunSound = LoadSound( "resources/sfx/shotgun.wav" );

    rm.enemyDeathSound01 = LoadSound( "resources/sfx/enemyDeath01.wav" );
    rm.enemyDeathSound02 = LoadSound( "resources/sfx/enemyDeath02.wav" );
    rm.enemyDeathSound03 = LoadSound( "resources/sfx/enemyDeath03.wav" );
    rm.noAmmoWarningSound = LoadSound( "resources/sfx/noAmmoWarning.wav" );
    rm.hpPowerUpSound = LoadSound( "resources/sfx/hpPowerUp.wav" );
    rm.ammoPowerUpSound = LoadSound( "resources/sfx/ammoPowerUp.wav" );
    rm.playerJumpSound = LoadSound( "resources/sfx/playerJump.wav" );
    rm.playerStepSound = LoadSound( "resources/sfx/playerStep.wav" );

}

void unloadResourcesResourceManager( void ) {

    //UnloadTexture( rm.textureExample );
    //UnloadMusicStream( rm.musicExample );

    UnloadShader( rm.lightShader );

    UnloadSound( rm.handgunSound );
    UnloadSound( rm.submachinegunSound );
    UnloadSound( rm.shotgunSound );
    UnloadSound( rm.enemyDeathSound01 );
    UnloadSound( rm.enemyDeathSound02 );
    UnloadSound( rm.enemyDeathSound03 );
    UnloadSound( rm.noAmmoWarningSound );
    UnloadSound( rm.hpPowerUpSound );
    UnloadSound( rm.ammoPowerUpSound );
    UnloadSound( rm.playerJumpSound );
    UnloadSound( rm.playerStepSound );
    
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

    if ( rm.lrWallModelCreated ) {
        UnloadTexture( rm.lrWallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
        UnloadModel( rm.lrWallModel );
        rm.lrWallModelCreated = false;
    }

    if ( rm.fnWallModelCreated ) {
        UnloadTexture( rm.fnWallModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture );
        UnloadModel( rm.fnWallModel );
        rm.fnWallModelCreated = false;
    }

}