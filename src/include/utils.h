/**
 * @file utils.h
 * @author Prof. Dr. David Buzatto
 * @brief Utilitary function and constant declarations.
 * 
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "raylib.h"

Color interpolate2Color( Color c1, Color c2, float t );
Color interpolate3Color( Color c1, Color c2, Color c3, float t );
bool colorEqualsIgnoreAlpha( Color c1, Color c2 );