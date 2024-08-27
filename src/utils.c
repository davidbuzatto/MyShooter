/**
 * @file utils.c
 * @author Prof. Dr. David Buzatto
 * @brief Utilitary functions implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include "raylib.h"
#include "utils.h"

Color interpolate2Color( Color c1, Color c2, float t ) {
    return (Color){
        .r = (int) ( c1.r + ( c2.r - c1.r ) * t ),
        .g = (int) ( c1.g + ( c2.g - c1.g ) * t ),
        .b = (int) ( c1.b + ( c2.b - c1.b ) * t ),
        .a = 255
    };
}

Color interpolate3Color( Color c1, Color c2, Color c3, float t ) {
    
    if ( t <= 0.5f ) {
        return interpolate2Color( c1, c2, t / 0.5f );
    } else {
        return interpolate2Color( c2, c3, ( t - 0.5f ) / 0.5f );
    }

}