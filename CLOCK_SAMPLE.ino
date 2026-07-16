#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

#define outputA 6 //CLK
#define outputB 7 // DT 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long interval = 1000; // 1 seconds
unsigned long preMillis = 0;


int minutes_1 = 0;
int minutes_2 = 0;
int hours_1 = 0;
int hours_2 = 0;

int aState;
int aLastState;
int counter = 0;
const int buttonPin = 2; // button on rotary encoder module
int button_num = 0;
// h1,h2,m1,m2
void setup() {        // SET THE TIME HERE
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(9600);
  display.clearDisplay();
  display.setTextColor(WHITE);

  aLastState = digitalRead(outputA);
  button_num = 0;

  while (button_num <= 5){

    aState = digitalRead(outputA);
    int buttonState = digitalRead(buttonPin);
    if (buttonState == LOW){
      button_num++;
      counter = 0;
      delay(500);
    }

    if (aState != aLastState){
      if (digitalRead(outputB) != aState){
        counter++;
      }
      else{
        counter--;
      }
    }
    if (button_num == 1){ //HOUR 1        H1, H2, M1, M2
      if(counter > 2){
        counter = 0;
      }
      if (counter < 0){
        counter = 2;
      }
      hours_1 = counter;
    }
    if (button_num == 2){ //HOUR 2

      if (hours_1 == 2){
        if (counter > 3){ 
          counter = 0;
        }
        if (counter < 0){
          counter = 3;
        }
      }
      else{
        if (counter > 9){
        counter = 0;
        }
        if (counter < 0){
        counter = 9;
        }
      }

      hours_2 = counter;

    
    }
    if (button_num == 3){ //MINUTE 1
      if (counter > 5){
        counter = 0;
      }
      if (counter < 0){
        counter = 5;
      }
      minutes_1 = counter;
    }
    if(button_num == 4){ //MINUTE 2
      if (counter > 9){
        counter = 0;
      }
      if (counter < 0){
        counter = 9;
      }
      minutes_2 = counter;
    }

    display.clearDisplay();

    display.setTextSize(3);
    display.setCursor(10, 8);
    display.print(hours_1);
    display.setTextSize(3);
    display.setCursor(35, 8);
    display.print(hours_2);
    display.setTextSize(3);
    display.setCursor(53, 8);
    display.print(":");
    display.setTextSize(3);
    display.setCursor(70, 8);
    display.print(minutes_1);
    display.setTextSize(3);
    display.setCursor(95, 8);
    display.print(minutes_2);
    
    display.display();

    Serial.print("|");
    Serial.print(hours_1);
    Serial.print(hours_2);
    Serial.print(":");
    Serial.print(minutes_1);
    Serial.print(minutes_2);
    Serial.println("|");

    aLastState = aState;
  }

}

void loop() {       // CONTINUE CURRENT TIME HERE
    display.clearDisplay();

    display.setTextSize(3);
    display.setCursor(10, 8);
    display.print(hours_1);
    display.setTextSize(3);
    display.setCursor(35, 8);
    display.print(hours_2);
    display.setTextSize(3);
    display.setCursor(53, 8);
    display.print(":");
    display.setTextSize(3);
    display.setCursor(70, 8);
    display.print(minutes_1);
    display.setTextSize(3);
    display.setCursor(95, 8);
    display.print(minutes_2);
    
    display.display();

    Serial.print("|");
    Serial.print(hours_1);
    Serial.print(hours_2);
    Serial.print(":");
    Serial.print(minutes_1);
    Serial.print(minutes_2);
    Serial.println("|");
  
}
