/*
BluetoothShield Demo Code Master.pde.This sketch could be used with
Slave.pde to establish connection between two Arduino.
2011 Copyright (c) Seeed Technology Inc.  All right reserved.

Author: Steve Chang

Modify: Loovee
2013-10-29

This demo code is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/


#include <SoftwareSerial.h>                         // Software Serial Port

#define RxD         7
#define TxD         6

#define PINBUTTON   9                               // pin of button

#define DEBUG_ENABLED  1

//Accelerometer
#define ystill 335
#define speeed 10
#define stillerror 10

String retSymb   = "+RTINQ=";                       // start symble when there's any return
String slaveName = ";GitGud";                 // caution that ';'must be included, and make sure the slave name is right.

int nameIndex = 0;
int addrIndex = 0;

String recvBuf;
String slaveAddr;

String connectCmd = "\r\n+CONN=";

const int groundpin = A0;             // analog input pin 0 -- ground
const int powerpin = A4;              // analog input pin 4 -- voltage
const int xpin = A3;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis (roll left: y < 290 -- print '0' , roll right: y > 380 -- print '2', idle ~ 333 -- print '1')
const int zpin = A1;                  // z-axis (only on 3-axis models)

SoftwareSerial blueToothSerial(RxD,TxD);

void setup()
{
    Serial.begin(9600);
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
    pinMode(PINBUTTON, INPUT);
    
    setupBlueToothConnection();
    //wait 1s and flush the serial buffer
    delay(1000);
    Serial.flush();
    blueToothSerial.flush();

    pinMode(groundpin, OUTPUT);
    pinMode(powerpin, OUTPUT);
  
    digitalWrite(groundpin, LOW);
    digitalWrite(powerpin, HIGH);
}

void loop()
{
  double y = analogRead(ypin);
  int sent = encode(y);
  blueToothSerial.print(sent);
  //delay(20);
}


void setupBlueToothConnection()
{
    blueToothSerial.begin(38400);                               // Set BluetoothBee BaudRate to default baud rate 38400
    blueToothSerial.print("\r\n+STWMOD=1\r\n");                 // set the bluetooth work in master mode
    blueToothSerial.print("\r\n+STNA=SeeedBTMaster\r\n");       // set the bluetooth name as "SeeedBTMaster"
    blueToothSerial.print("\r\n+STAUTO=0\r\n");                 // Auto-connection is forbidden here
    delay(2000);                                                // This delay is required.
    blueToothSerial.flush();
    blueToothSerial.print("\r\n+INQ=1\r\n");                    //make the master inquire
    Serial.println("Master is inquiring!");
    delay(2000); // This delay is required.

    //find the target slave
    char recvChar;
    while(1)
    {
        if(blueToothSerial.available())
        {
            recvChar = blueToothSerial.read();
            Serial.write(recvChar);
            recvBuf += recvChar;
            nameIndex = recvBuf.indexOf(slaveName);             //get the position of slave name
            
                                                                //nameIndex -= 1;
                                                                //decrease the ';' in front of the slave name, to get the position of the end of the slave address
            if ( nameIndex != -1 )
            {
                //Serial.print(recvBuf);
                addrIndex = (recvBuf.indexOf(retSymb,(nameIndex - retSymb.length()- 18) ) + retSymb.length());//get the start position of slave address
                slaveAddr = recvBuf.substring(addrIndex, nameIndex);//get the string of slave address
                break;
            }
        }
    }
    
    //form the full connection command
    connectCmd += slaveAddr;
    connectCmd += "\r\n";
    int connectOK = 0;
    Serial.print("Connecting to slave:");
    Serial.print(slaveAddr);
    Serial.println(slaveName);
    //connecting the slave till they are connected
    
    do
    {
        blueToothSerial.print(connectCmd);//send connection command
        blueToothSerial.print("\r\n+RTPIN=0000\r\n");//send connection command
        recvBuf = "";
        while(1)
        {
            if(blueToothSerial.available()){
                recvChar = blueToothSerial.read();
                Serial.write(recvChar);
                recvBuf += recvChar;
                if(recvBuf.indexOf("CONNECT:OK") != -1)
                {
                    connectOK = 1;
                    Serial.println("Connected!");
                    blueToothSerial.print("Connected!");
                    break;
                }
                else if(recvBuf.indexOf("CONNECT:FAIL") != -1)
                {
                    Serial.println("Connect again!");
                    break;
                }
            }
        }
    }while(0 == connectOK);
}

int encode (double y){
  if (int(y)-ystill < -stillerror){
    y = int(1000*((y+stillerror)/ystill));
  }
  else if (int(y) - stillerror > ystill){
    y = int(1000*((y-stillerror)/ystill));
    }
  else{
    y = 1000;
  }
  return y;
}

