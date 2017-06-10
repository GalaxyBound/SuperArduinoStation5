/* Ball.h
 *
 * Class that provides logic for the ball & its movement
 */

#ifndef ball_h
#define ball_h

class Ball
{
public:
    // Methods
    // Initialises the ball
    Ball(void);
    
    // Set ball position
    void setPosition(int x, int y);
    
    // Set ball angle
    void setAngle(float newAngle);
    
    // Variables
    float xpos;
    float ypos;
    float angle;
    int radius;
    int speed;
    
};


#endif
