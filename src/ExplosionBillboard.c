#include "Types.h"
#include "ExplosionBillboard.h"
#include "ResourceManager.h"
#include "raylib/raylib.h"

ExplosionBillboard createExplosionBillboard( Vector3 pos ) {

    return (ExplosionBillboard) {
        .pos = pos,
        .textures = { rm.explosion0, rm.explosion1, rm.explosion2 },
        .frameCount = 3,
        .currentFrame = 0,
        .frameTimeCounter = 0.0f,
        .timeToNextFrame = 0.1f,
        .finished = false
    };

}

void updateExplosionBillboard( ExplosionBillboard *eb, float delta ) {

    if ( !eb->finished ) {
        if ( eb->currentFrame < eb->frameCount ) {
            eb->frameTimeCounter += delta;
            if ( eb->frameTimeCounter >= eb->timeToNextFrame ) {
                eb->frameTimeCounter = 0.0f;
                eb->currentFrame++;
            }
        }
        if ( eb->currentFrame == eb->frameCount ) {
            eb->finished = true;
        }
    }
    

}

void drawExplosionBillboard( ExplosionBillboard *eb, Camera camera ) {
    if ( !eb->finished ) {
        DrawBillboard( camera, eb->textures[eb->currentFrame], eb->pos, 5.0f, WHITE );
    }
}