#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
//#include <SPI.h>
//#include <SD.h>

#define DHTPIN 4
#define DHTTYPE DHT11 //add DHT22 if newer version

DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define outputA 32
#define outputB 33

//const int chipSelect = 4;

unsigned long interval = 60000;
unsigned long preMillis = 0;
unsigned long noiseAlertuntil = 0;
const unsigned long AlertDuration = 500;
unsigned long lastRead = 0;
const unsigned long readInterval = 2000;
bool ShowTime = false;
bool LastSwitchState = HIGH;
bool setAlarm = false;
bool alarmChoice = false;
bool alarmRinging = false;
bool alarmOff = false;
float t = 0;
float h = 0;


int soundSensor = 36;
int LDR = 39;
int LightLED = 13;
int buttonPin = 5;
int relay = 19;
int buzzerPin = 18;

int minutes_1 = 0;
int minutes_2 = 0;
int hours_1 = 0;
int hours_2 = 0;
int Aminutes_1 = 0;
int Aminutes_2 = 0;
int Ahours_1 = 0;
int Ahours_2 = 0;


int aState;
int aLastState;
int counter = 0;
const int switch_b = 23; // button on rotary encoder module
int button_num = 0;
int currentSound;
int lightAmt;

int switchState = 0;
int count = 0;
int total = 0;
int peakSound = 0;

void displayTime(int h1, int h2, int m1, int m2){
  display.clearDisplay();

  display.setTextSize(3);
  display.setCursor(10, 8);
  display.print(h1);
  display.setTextSize(3);
  display.setCursor(35, 8);
  display.print(h2);
  display.setTextSize(3);
  display.setCursor(53, 8);
  display.print(":");
  display.setTextSize(3);
  display.setCursor(70, 8);
  display.print(m1);
  display.setTextSize(3);
  display.setCursor(95, 8);
  display.print(m2);
    
  display.display();

}

void setTime(int &h1, int &h2, int &m1, int &m2){
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
        h1 = counter;
      }
      if (button_num == 2){ //HOUR 2

        if (h1 == 2){
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

        h2 = counter;

    
      }
      if (button_num == 3){ //MINUTE 1
        if (counter > 5){
          counter = 0;
        }
        if (counter < 0){
         counter = 5;
        }
        m1 = counter;
      }
      if(button_num == 4){ //MINUTE 2
        if (counter > 9){
          counter = 0;
        }
        if (counter < 0){
          counter = 9;
        }
        m2 = counter;
      }

      displayTime( h1, h2, m1, m2);

      aLastState = aState;
    }

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

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("LOUD NOISE"));

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
    display.print(F("Temperature: "));
    display.setTextSize(2);
    display.setCursor(10,9);
    display.print(t);
    display.print(F(" "));
    display.setCursor(80,10);
    display.setTextSize(1);
    display.cp437(true);
    display.write(167);
    display.setTextSize(2);
    display.print(F("C"));
  
    // display humidity
    display.setTextSize(1);
    display.setCursor(30, 25);
    display.print(F("Humidity: "));
    display.setTextSize(1);
    display.setCursor(85, 25);
    display.print(h);
    display.setCursor(115, 25);
    display.print(" %"); 
  
    display.display(); 
}


void setup() {                                                   //SETUP
  

  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED DISPLAY FAILED"));
    while(true);
  }
  Serial.println(F("DISPLAY initialised"));

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(switch_b, INPUT_PULLUP);
  pinMode(soundSensor, INPUT);
  pinMode(outputA, INPUT);
  pinMode(outputB, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(LightLED, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(LDR, INPUT);
  dht.begin();
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

  digitalWrite(LightLED, LOW);


  setTime( hours_1, hours_2, minutes_1, minutes_2);
  delay(500);

  while(!alarmChoice){
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("Do you want to set an Alarm?"));
    delay(500);
    display.display();

    if(digitalRead(switch_b) == LOW){
      alarmChoice = true;
      setAlarm = false;
      Serial.println(F("ENCODER PRESS"));
      Ahours_1 = -1;
      Ahours_2 = -1;
      Aminutes_1 = -1;
      Aminutes_2 = -1;
      
    }
    if(digitalRead(buttonPin) == LOW){
      Serial.println(F("buttonpin"));
      alarmChoice = true;
      setAlarm = true;
    }

  if (setAlarm){
    setTime( Ahours_1, Ahours_2, Aminutes_1, Aminutes_2);
  }


  }
}

void loop() {                                                     //LOOP

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
    displayTime( hours_1, hours_2, minutes_1, minutes_2);
  }
  
  else{

    if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    }

    EnvironmentShow(t,h);

  }
 }

 

  lightAmt = analogRead(LDR);
  Serial.println(lightAmt);

  if (lightAmt > 130 ){
    digitalWrite(LightLED, LOW);
  }
  if ( lightAmt < 120 ){                    // Light
    digitalWrite(LightLED, HIGH);
  }

  currentSound = analogRead(soundSensor);       // Sound 
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

    if (Aminutes_1 == minutes_1 && Aminutes_2 == minutes_2 && Ahours_1 == hours_1 && Ahours_2 == hours_2 && alarmOff == false){
     alarmRinging = true;
    }
    else if(!(Aminutes_1 == minutes_1 && Aminutes_2 == minutes_2 && Ahours_1 == hours_1 && Ahours_2 == hours_2)){
      alarmOff = false;
    }

    if(alarmRinging){
      digitalWrite(buzzerPin, HIGH);
      if (digitalRead(buttonPin) == LOW){
        alarmRinging = false;
        alarmOff = true;
        digitalWrite(buzzerPin, HIGH);
      }
   }
    else{
      digitalWrite(buzzerPin, LOW);
    }

  

    if (t > 19.5){
      digitalWrite(relay, HIGH);
    }
    else{
     digitalWrite(relay, LOW);
    }

  LastSwitchState = switchState;

}
  
