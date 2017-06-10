#include <VGA.h>
#include <math.h>
#include "Color.h"
#include "Ball.h"

// Uncomment to enable colour output
//#define COLORMODE

#define blueToothSerial Serial2

Ball ball = Ball();
#ifdef COLORMODE
Color color = Color(1);
#else
Color color = Color(0);
#endif

// Game variables
float p1pos = 10;
float oldp1pos = 10;
float p2pos = 10;
float oldp2pos = 10;
float pMargin = 10;
float pMaxSpeed = 2;
float p1Move = 0;

int gameState = 1;

int p1Score = 0;
int p2Score = 0;
char p1ScoreStr[2];
char p2ScoreStr[2];

int sWidth = 320;
int sHeight = 240;
int pHeight = 50;
int pWidth = 10;
int cpuBuffer = pHeight/4;
float cpuMaxSpeed = 0.97;

void setup() {
  debounce();
  Serial.begin(9600);
  attachInterrupt(5, pauseHandler, RISING);
  
  // Setup Wait for bluetooth to connect (Signal from A1 sent to Pin 8)
  pinMode(8, INPUT);
  
  // Setup Video
  int i = VGA.begin(sWidth,sHeight, VGA_COLOUR);
  
  if (i != 0) {
    Serial.print(i);
    setup();
    return;
  }

  // Uncomment to fix pixel-shifting on some monitors
  /*VGA.xstart-=4;
  VGA.synced=0;
  */

  drawTestImage();
  delay(2000);
  VGA.clear(0);
  VGA.fillRect(0,0,sWidth,sHeight,color.bg);

  // Setup scene
  setupStage();
  

  // Setup Bluetooth
  setupBlueToothConnection();
}

void loop() {
  // Check for pause before drawing other elements
  boolean justPaused=true;
  
  while (gameState == 1){
    // When paused state, print pause on the screen
    if (justPaused){
      printPaused();
      justPaused = false;
    }
    //This delay is required
    delay(50);
  }

  if(!justPaused) printPaused();

  // Draw score first up
  drawScore();
  
  if (moveBall()) {
    // A Player has scored
    scorePoint();
  } else {
    // Normal game
    movePlayer1();
    movePlayer2();
  }
  
  // Causes issues on some displays, disabling for now
  /*Draws the net*/
  //drawNet();

  VGA.waitSync();
  VGA.waitBeam(); 
}

/***
 * Move Methods for players and ball
 * 
 * 
 */

// Returns true if ball out of bounds (score), otherwise false for just moved normally
 boolean moveBall() {
  // Calculate angle & coords
  float sine = sin(ball.angle);
  float cosine = cos(ball.angle);
  float denom = abs(sine)+abs(cosine);
  ball.xpos += ball.speed*(sine/denom);
  ball.ypos += ball.speed*(cosine/denom);

  // Check if hitting ceiling or floor & correct it
  if (ball.ypos - ball.radius < 0 || ball.ypos + ball.radius > sHeight) {
    if (ball.angle > 0) {
      ball.angle = M_PI-ball.angle;
    } else {
      ball.angle = -M_PI-ball.angle;
    }

    // Don't need to recalc angle & is just opposite
    ball.ypos -= 2*ball.speed*(cosine/denom);
  }

  // Now check if intersect player side
  if (ball.xpos-ball.radius<pMargin+pWidth || ball.xpos+ball.radius>sWidth-pMargin-pWidth+1) {
    //Check here if not intersecting player
    if (ball.xpos < sWidth/2) {
      // Player 1 Check
      if (ball.ypos+ball.radius<p1pos-1 || ball.ypos-ball.radius>p1pos+pHeight+2) {
        p2Score++;
        return true;
      }
    } else {
      // Player 2 Check
      if (ball.ypos+ball.radius<p2pos-1 || ball.ypos-ball.radius>p2pos+pHeight+2) {
        p1Score++;
        return true;
      }
    }
    
    // Hit player, now bouncing
    ball.xpos -= 2*ball.speed*(sine/denom);

    // Now generate a random angle
    float randAngle = (float)random(0, M_PI/3*200)/200.0;//(float)rand()/((float)RAND_MAX/(M_PI/3));

    if (ball.angle > 0) {
      if (ball.angle < M_PI/2) {
        ball.angle = -M_PI/2+randAngle;
      } else {
        ball.angle = -M_PI/2-randAngle;
      }
    } else {
      if (ball.angle > -M_PI/2) {
        ball.angle = M_PI/2-randAngle;
      } else {
        ball.angle = M_PI/2+randAngle;
      }
    }
  }

  // 1 means moving positive (up & right)
  int dirXBall = (ball.angle>0) ? 1 : -1;
  int dirYBall = (abs(ball.angle)<M_PI/2) ? 1 : -1;
  
  // Remove trail colors
  VGA.fillRect(ball.xpos-dirXBall*(ball.radius+5), ball.ypos-ball.radius-5, ball.xpos-dirXBall*(ball.radius), ball.ypos+ball.radius+3, color.bg);
  VGA.fillRect(ball.xpos-ball.radius-3, ball.ypos-dirYBall*(ball.radius+8), ball.xpos+ball.radius+3, ball.ypos-dirYBall*(ball.radius), color.bg);

  // Print ball
  VGA.fillRect(ball.xpos-ball.radius, ball.ypos-ball.radius-ball.radius, ball.xpos+ball.radius, ball.ypos+ball.radius, color.ball);

  return false;
 }

 void movePlayer1() {
  // Note - position adjustment for player 1 is set outside this method in the bluetooth section
  p1pos += p1Move;
  p1Move = 0;

  if (p1pos < 0) p1pos = 0;
  if (p1pos > sHeight - pHeight) p1pos = sHeight - pHeight;
  
  if ((int)oldp1pos - (int)p1pos != 0) {
    VGA.fillRect(pMargin, p1pos-12,pMargin+pWidth,p1pos,color.bg);
    VGA.fillRect(pMargin,pHeight+p1pos,pMargin+pWidth,pHeight+p1pos+12,color.bg);
  }
  oldp1pos = p1pos;
  
  int p1posInt = (int)p1pos;
  VGA.fillRect(pMargin,p1posInt,pMargin + pWidth,p1posInt + pHeight,color.player1);
 }

  void movePlayer2() {
  // Note - all movement logic for P2 CPU is performed here
  // Performs a slow follow

  if (ball.ypos-ball.radius<p2pos+cpuBuffer) p2pos -= cpuMaxSpeed;
  if (ball.ypos+ball.radius>p2pos+pHeight-cpuBuffer) p2pos += cpuMaxSpeed;

  if (p2pos < 0) p2pos = 0;
  if (p2pos > sHeight - pHeight) p2pos = sHeight - pHeight;

  if ((int)oldp2pos - (int)p2pos != 0) {
    VGA.fillRect(sWidth-pWidth-pMargin, p2pos-12,sWidth-pMargin,p2pos,color.bg);
    VGA.fillRect(sWidth-pWidth-pMargin, pHeight+p2pos,sWidth-pMargin,pHeight+p2pos+12,color.bg);
  }
  oldp2pos = p2pos;
  
  int p2posInt = (int)p2pos;
  VGA.fillRect(sWidth - pMargin - pWidth,p2posInt,sWidth - pMargin,p2posInt + pHeight,color.player2);
 }

/***
 * Draw Methods for fixed on-screen elements
 * 
 * i.e. Score & Net
 * 
 */

void drawScore() {
  int startX = sWidth/2 - 12;
  int startX2 = sWidth/2 + 6;
  int startY = 4;

  VGA.drawText(p1ScoreStr, startX, startY, color.score, color.bg, 0);
  VGA.drawText(p2ScoreStr, startX2, startY, color.score, color.bg, 0);
}

void drawNet() {
  int nWidth = 4;
  int nHeight = 4;

  int startX = sWidth/2 - nWidth/2;
  int endX = sWidth/2 + nWidth/2;

  for (int i = 1; i*nHeight < sHeight; i+=3) {
    VGA.fillRect(startX,i*nHeight,endX,(i+1)*nHeight,color.net);
  }
}

void drawTestImage() {
  VGA.fillRect(10,10,20,60,3);
  VGA.fillRect(300,10,310,60,224);
  int width = sWidth/6;
  int x = 0;
  int y = sHeight/2;
  int bot = sHeight;
  VGA.fillRect(x,y,x+width,bot,255);
  x += width;
  VGA.fillRect(x,y,x+width,bot,224);
  x += width;
  VGA.fillRect(x,y,x+width,bot,28);
  x += width;
  VGA.fillRect(x,y,x+width,bot,3);
  x += width;
  VGA.fillRect(x,y,x+width,bot,240);
  x += width;
  VGA.fillRect(x,y,x+width,bot,227);
  x += width;
}

/***
 * Game win/score area & reset value
 * 
 * 
 */

 void scorePoint() {
  // Update
  if (p1Score > 9) {
    winScreen("Player 1 Wins!");
  } else if (p2Score > 9) {
    winScreen("Player 2 Wins!");
  } else {
    setupStage();
    
    // Print screen and delay for 1 second
    movePlayer1();
    movePlayer2();
    drawScore();
    delay(1000);
  }
 }

 void winScreen(char *winStr) {
  setupStage();

  movePlayer1();
  movePlayer2();
  drawScore();

  int startX = sWidth/3;
  int startY = sHeight/2-4;

  VGA.drawText(winStr, startX, startY, color.win, color.bg, 0);
  
  delay(3000);
  p1Score = 0;
  p2Score = 0;
  setupStage();
 }

// Setup stage to initial ball in middle & players zeroed
 void setupStage() {
  // Clear Screen
  VGA.fillRect(0,0,sWidth,sHeight,color.bg);
  
  // Score setup & checking
  sprintf(p1ScoreStr, "%d", p1Score);
  sprintf(p2ScoreStr, "%d", p2Score);

  if (p1Score > 9) sprintf(p1ScoreStr, "W");
  if (p2Score > 9) sprintf(p2ScoreStr, "W");
  
  
  // Ball setup
  ball.xpos = sWidth/2;
  ball.ypos = sHeight/2;
  ball.angle = M_PI/2 + random(-1,1)*0.001;

  // Player Setup
  p1pos = sHeight/2-pHeight/2;
  p2pos = sHeight/2-pHeight/2;

  //Variables
  randomSeed(analogRead(5));
 }

/***
 * Bluetooth setup area
 * 
 * 
 */

// Display waiting for bluetooth on screen
void printBlueToothWaiting() {
  //VGA.clear(color.bg);

  int startX = sWidth/6.8;
  int startY = sHeight/2-4;
  char *btStr = "Waiting for P1 to connect...";
  VGA.drawText(btStr, startX, startY, color.xorr, color.clr, 0);
}
void printBlueToothConnected() {
  VGA.clear(color.bg);

  int startX = sWidth/3;
  int startY = sHeight/2-4;
  char *btStr = "P1 Connected!";
  VGA.drawText(btStr, startX, startY, color.xorr, color.clr, 0);

  delay(1000);
  // This should XOR itself off the screen
  VGA.drawText(btStr, startX, startY, color.xorr, color.clr, 0);
}

void printPaused(){
  //VGA.clear(color.bg);

  int startX = sWidth/2.4;
  int startY = sHeight/2-4;
  char *btStr = "Paused!";
  VGA.drawText(btStr, startX, startY, color.xorr, color.clr, 0);
}

void setupBlueToothConnection()
{
    printBlueToothWaiting();
    blueToothSerial.begin(38400);                           // Set BluetoothBee BaudRate to default baud rate 38400
    blueToothSerial.print("\r\n+STWMOD=0\r\n");             // set the bluetooth work in slave mode
    blueToothSerial.print("\r\n+STNA=GitGud\r\n");    // set the bluetooth name as "SeeedBTSlave"
    blueToothSerial.print("\r\n+STOAUT=1\r\n");             // Permit Paired device to connect me
    blueToothSerial.print("\r\n+STAUTO=0\r\n");             // Auto-connection should be forbidden here

        while(blueToothSerial.available())
        {   
            //check if there's any data sent from the remote bluetooth shield
            char recvChar = blueToothSerial.read();
            Serial.write(recvChar);
        }
    delay(2000);                                            // This delay is required.
  
    blueToothSerial.print("\r\n+INQ=1\r\n");                // make the slave bluetooth inquirable
    
    delay(2000);                                            // This delay is required.
       
    blueToothSerial.flush();

    // Wait for bluetooth to connect (Signal from A1 sent to Pin 8)
    while (digitalRead(8) != 1) {
    }
    printBlueToothConnected();
}

/***
 * Serial Event area triggered by serial received
 * 
 */

 void serialEventRun(void) {
  while (blueToothSerial.available()) {
    bluetoothReceived();
  }
 }

 void bluetoothReceived() {
  // data sent from the remote bluetooth shield
  char recvChar = blueToothSerial.read();

  // calculate paddle speed
  float toMove = ((int)recvChar-128)*1.0/8.0;

  Serial.println(toMove);
  
  if (toMove > 2) toMove = 2;
  if (toMove < -2) toMove = -2;

  //corrects roll direction
  p1Move = -toMove;
 }

/* IRQ for button on digital pin 5 */
 void pauseHandler(){
  //if global int gameState = 0 -> Unpaused state, gameState =1 -> Paused state
  gameState = (gameState+1) % 2;
 }

/* Debouncing setup function*/
 void debounce(){
  REG_PIOC_IFER = (1<<25);
  REG_PIOC_DIFSR = (1<<25);
  REG_PIOC_IFDGSR = (1<<25);
  REG_PIOC_SCDR = 255;
 }

