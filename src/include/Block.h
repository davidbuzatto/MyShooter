#pragma once

#include "raylib/raylib.h"

typedef struct Block {

    int id;

    Vector3 pos;
    Vector3 dim;
    Color color;
    Color tintColor;
    Color touchColor;
    bool visible;

    Model model;
    bool renderModel;
    bool renderTouchColor;

} Block;

void drawBlock( Block *block );
BoundingBox getBlockBoundingBox( Block *block );
