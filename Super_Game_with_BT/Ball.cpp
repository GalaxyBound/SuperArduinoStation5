/* Ball.cpp
 *
 * Implementation of ball methods
 */

#include "Ball.h"

Ball::Ball()
{
    xpos = 100;
    ypos = 100;
    angle = 1.58;
    radius = 5;
    speed = 3;
}

void Ball::setPosition(int x, int y)
{
    xpos = x;
    ypos = y;
}

void Ball::setAngle(float newAngle)
{
    angle = newAngle;
}

/*
 float ballXPos = 100;
 float ballYPos = 100;
 float ballAngle = M_PI/2;
 int ballRad = 5;
 int ballSpeed = 3;
 */
