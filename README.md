# SLEEP HELPER!! 
Sleep Helper is a box that sits by your night stand and provides you data about your sleep environment when you wake up the next day!

![Hardware]()
### This project uses an ESP32 with BLE (Bluetooth Low Energy) with hardware including an OLED display (SSD1306), rotary encoder module, DHT11 module, big sound sensor module and LDR. Data collected from these sensors is sent to a webpage which displays it in graphs to evaluate your environment in the morning. It can also act as an alarm. 

* ESP32
* BLE
* Arduino
* Modules
* DHT11
* Sleep sensor
* Alarm

## Using this repo:
1. Code is located in [Arduino]() and can easily be uploaded to ESP32 with the ESP32 board library 
2. Other code can be used to test some sensors and see early iterations of my code for hardware [Other Code]()
3. Get a breadboard and start wiring according to the schematic [schematic]()
4. Download the webpage and copy the path for the .html file and copy link onto web browser of your choice.
5. Connect your board to the webpage (make sure bluetooth on your device is enabled)
6. Edit Arduino code for your sensors sensitivity.
7. If you have a 3D printer download the CAD files and enhance your nightstand experience! [CAD]()
8. Place it on your night stand and good night!

## heres how I made the project :)
1. Started by displaying an OLED screen with data from DHT11 module (temp & humidity)
2. Added other modules to add sound and light sensing and some LEDs
3. Created some alarm clock functionality with code and displayed on OLED (this includes current time and alarm)
4. Polished code up for every function
5. Started building a webpage which displays data in a graph
6. Designed CAD exterior for project.

## Roadblocks
* Trying to use Arduino UNO as it had too little SRAM for my program
* Connecting faulty sensors from unverified sources (e.g. my DHT11 did not work and needed another)
* Wires loosening on motor 

## Code - Arduino IDE
Libraries : 

```
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
```
(can be found in IDE)

```void EnvironmentShow(float t, float h) ``` -- used to position temp and humidity data on OLED
```void displayTime()``` -- Displays time on OLED screen
```vvoid NoiseAlert()``` -- Displays a noise alert if sound sensor detects noise over a specific threshold
```void updateClock()``` -- updates current time on OLED 

## Hardware 

This is the schematic [Schematic]()

Parts List: 

* ESP32
* DHT11 Module
* Big sound sensor module
* Rotary encoder module w/ switch
* LDR
* LEDs
* DC motor
* 10uF Capacitor
* Resistors
* Active Buzzer (can be module)
* Pushbutton 


## Webpage

To connect your ESP32 to the webpage:

1. Click Connect to device
2. Select your device
3. The text on the top left should say Connected
4. To double check wiring and connection is good toggle the fan button on the webpage and see if the fan turns on

Clear Saved Data --> deletes all data so far

A **baseline** exists for each graph to help understand what your environment should be, can be changed in code.

**IMPORTANT: As the webpage is local only it can store data continously BUT if the site is refreshed it will disconnect from device and thus not continue reading data (but still keep the data collected so far)**
