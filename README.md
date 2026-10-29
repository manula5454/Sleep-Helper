# SLEEP HELPER!! 
Sleep Helper is a box that sits by your night stand and provides you data about your sleep environment when you wake up the next day!

![Hardware]()
## This project uses an ESP32 with BLE (Bluetooth Low Energy) with hardware including an OLED display (SSD1306), rotary encoder module, DHT11 module, big sound sensor module and LDR. Data collected from these sensors is sent to a webpage which displays it in graphs to evaluate your environment in the morning. It can also act as an alarm. 

* ESP32
* BLE
* Arduino
* Modules
* DHT11
* Sleep sensor
* Alarm
* .html
* .css
* .js

## Using this repo:
1. Code is located in [Arduino]()
2. Other code can be used to test some sensors and see early iterations of my code for hardware [Other Code]()
3. I will upload the schematic/wiring soon!!
4. If you have a 3D printer download the CAD files and enhance your nightstand experience! [CAD]()

## heres how I made the project :)
1. Started by displaying an OLED screen with data from DHT11 module (temp & humidity)
2. Added a 

## Code - Arduino IDE
Libraries : 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
(can be found in IDE)

void EnvironmentShow(float t, float h) -- used to position temp and humidity data on OLED
void displayTime() -- Displays time on OLED screen



buttonPin -- 1 press = pause, long press = reset {rp.2040 reboot();}

LED functions - builds up (e.g. LED5 lights up, then LED4....)

## Hardware


## Webpage
