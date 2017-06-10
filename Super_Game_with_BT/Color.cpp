/* Color.cpp
 *
 * Implementation of color methods
 */

#include "Color.h"

Color::Color(int colorMode)
{
    if (colorMode == 0) {
        // Black and white mode
        BWEnable();
    } else {
        // Default to color mode
        ColorEnable();
    }
}

void Color::BWEnable()
{
    // Sets as black and white
    bg = 0;
    player1 = 255;
    player2 = 255;
    ball = 255;
    net = 255;
    score = 255;
    win = 255;
    clr = -256;
    xorr = -255;
}

void Color::ColorEnable()
{
    // Sets as color output
    bg = 37;
    player1 = 224;
    player2 = 8;
    ball = 28;
    net = 8;
    score = 227;
    win = 227;
    clr = -256;
    xorr = -255;
}

/*
 #ifdef COLORMODE
 int colorBg = 37;
 int colorPlayer = 224;
 int colorOpp = 8;
 int colorBall = 28;
 int colorNet = 8;
 int colorScore = 227;
 int colorWin = 227;
 #else
 int colorBg = 0;
 int colorPlayer = 255;
 int colorOpp = 255;
 int colorBall = 255;
 int colorNet = 255;
 int colorScore = 255;
 int colorWin = 255;
 #endif
 */
