#pragma once

#include "raylib.h"

typedef enum EntityType {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_BLOCK,
    ENTITY_TYPE_OBSTACLE,
    ENTITY_TYPE_ENEMY
} EntityType;

typedef struct IdentifiedRayCollision {
    int entityId;
    EntityType entityType;
    RayCollision collision;
} IdentifiedRayCollision;

typedef struct MultipleIdentifiedRayCollision {
    IdentifiedRayCollision irCollisions[20];
    int quantity;
} MultipleIdentifiedRayCollision;

extern int entityIdCounter;
