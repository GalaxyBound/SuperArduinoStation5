/* Color.h
 *
 * Class that provides the values of colors for games
 */

#ifndef color_h
#define color_h

class Color
{
public:
    // Methods
    Color(int colorMode);
    void BWEnable(void);
    void ColorEnable(void);
    
    // Variables
    int bg;
    int player1;
    int player2;
    int ball;
    int net;
    int score;
    int win;
    int clr;
    int xorr;
    
};


#endif
