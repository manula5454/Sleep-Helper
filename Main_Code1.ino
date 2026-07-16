#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define outputA 6
#define outputB 7

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 8
#define DHTTYPE DHT11 //add DHT22 if newer version

DHT dht(DHTPIN, DHTTYPE);

unsigned long interval = 60000;
unsigned long preMillis = 0;
unsigned long noiseAlertuntil = 0;
const unsigned long AlertDuration = 500;
unsigned long lastShock = 0;
const unsigned long shockTime = 1000;
unsigned long lastRead = 0;
const unsigned long readInterval = 2000;
bool ShowTime = false;
bool LastSwitchState = HIGH;
float t = 0;
float h = 0;


int soundSensor = A0;
int buttonPin = 3;
int shockSensor = 4;
int LDR = A1;
int LightLED = 9;
int pwrLED = 13;
int startButton = 2;
int relay = 10;

long int minutes_1 = 0;
long int minutes_2 = 0;
int hours_1 = 0;
int hours_2 = 0;

int aState;
int aLastState;
int counter = 0;
const int switch_b = 5; // button on rotary encoder module
int button_num = 0;

int switchState = 0;
int count = 0;
int total = 0;
int peakSound = 0;


void displayTime(){
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

}

void updateClock(){
  minutes_2++;

    if (minutes_2 >= 10){
      minutes_2 = 0;
      minutes_1++;
    }
    if (minutes_1 >= 6){
        minutes_1 = 0;
        minutes_2 = 0;
        hours_2++;
    }
    if (hours_2 >= 10){
        hours_2 = 0;
        minutes_1 = 0;
        minutes_2 = 0;
        hours_1++;
    }
    if (hours_1 >= 2 && hours_2 >=4) {
        hours_1 = 0;
        hours_2 = 0;
       minutes_1 = 0;
       minutes_2 = 0;
    }

}

void NoiseAlert(){
  Serial.println("noise");

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LOUD NOISE");

  display.display();
}

void displayShock(){

  Serial.println("Shock!");
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Shock!");

  display.display();
}

int AvrNoise(){ // t = total n = # of times
 if(count == 0) return 0;
 
 return total/count;

}

void EnvironmentShow(float t, float h){
    display.clearDisplay();
 
   // display temperature
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Temperature: ");
    display.setTextSize(2);
    display.setCursor(10,9);
    display.print(t);
    display.print(" ");
    display.setCursor(80,10);
    display.setTextSize(1);
    display.cp437(true);
    display.write(167);
    display.setTextSize(2);
    display.print("C");
  
    // display humidity
    display.setTextSize(1);
    display.setCursor(30, 25);
    display.print("Humidity: ");
    display.setTextSize(1);
    display.setCursor(85, 25);
    display.print(h);
    display.setCursor(115, 25);
    display.print(" %"); 
  
    display.display(); 
}


void setup() {                                                  //SETUP
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(switch_b, INPUT_PULLUP);
  pinMode(soundSensor, INPUT);
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);
  pinMode(pwrLED, OUTPUT);
  pinMode(LightLED, OUTPUT);
  pinMode (shockSensor, INPUT);
  pinMode(startButton, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(LDR, INPUT);
  Serial.begin(9600);
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

  Serial.println("About to init display...");
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println("SSD1306 allocation failed!");
      while(true);
    }
  Serial.println("Display init OK");

  aLastState = digitalRead(outputA);
  button_num = 0;

  while (button_num <= 4){

    aState = digitalRead(outputA);
    int buttonState = digitalRead(switch_b);
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

    displayTime();

    

    aLastState = aState;
  }
}

void loop() {                                                     //LOOP

  digitalWrite(pwrLED, HIGH);

  switchState = digitalRead(switch_b);  


  if(millis() - lastRead >= readInterval){
    t = dht.readTemperature();        // Temp & Humidity Readings
    h = dht.readHumidity();
    lastRead = millis();
  }
  

 if (millis() >= noiseAlertuntil){                                  //MAIN BLOCK


  if (switchState == LOW && LastSwitchState == HIGH){
    ShowTime = !ShowTime;     //time shown
  }
  if (ShowTime == true){
    displayTime();
  }
  
  else{

    if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    }

    EnvironmentShow(t,h);

  }

 }

  if (analogRead(LDR) > 230){
    digitalWrite(LightLED, HIGH);
  }
  if (analogRead(LDR) < 200 ){    
    Serial.println(analogRead(LDR));                   // Light
    digitalWrite(LightLED, LOW);
  }
  

  int currentSound = analogRead(soundSensor);       // Sound 
  total += currentSound;
  count++;

  if (currentSound > peakSound){
    peakSound = currentSound;   
  }

  if (count > 20){
    if ( peakSound >= AvrNoise() + 60 ){
      NoiseAlert();
      noiseAlertuntil = millis() + AlertDuration;
    }
    count = 0;
    total = 0;
    peakSound = 0;         
  }


  unsigned long curMillis = millis();
  // h1,h2,m1,m2

    if (curMillis - preMillis >= interval){
      preMillis = curMillis;
      updateClock();
    }

  

    if (t > 24){
      digitalWrite(relay, HIGH);
    }
    else{
      digitalWrite(relay, LOW);
    }


  LastSwitchState = switchState;

}
