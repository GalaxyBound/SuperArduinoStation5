# SuperArduinoStation5

## About
Super Arduino Station 5 is a project that connects an Arduino Uno acting as an game controller with an accelerometer to a Arduino Due Console over bluetooth. Game logic and rendering is performed on the Arduino Due with on-screen elements rendered with help from the [DueVGA](https://github.com/stimmer/DueVGA) library.

## Requirements

- Arduino Uno
- Arduino Due
- ADXL335 Accelerometer
- 2 x Seeeduino Bluetooth Shields
- [DueVGA Library by Stimmer](https://github.com/stimmer/DueVGA)
- Asorted Resistors (See schematic)

## Schematics

### Arduino Due Console
![alt text](https://github.com/GalaxyBound/SuperArduinoStation5/raw/master/Schematics/SAS5_console_schem.png)

### Arduino Uno Controller
![alt text](https://github.com/GalaxyBound/SuperArduinoStation5/raw/master/Schematics/SAS5_controller_schem.png)

Note: The Seeeduino Bluetooth shield is ommitted from the above schematics.
