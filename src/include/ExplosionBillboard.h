#pragma once

#include "Types.h"
#include "raylib.h"

ExplosionBillboard createExplosionBillboard( Vector3 pos );
void updateExplosionBillboard( ExplosionBillboard *eb, float delta );
void drawExplosionBillboard( ExplosionBillboard *eb, Camera camera );