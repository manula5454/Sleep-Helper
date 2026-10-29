int sensor = 2; // Connected to digital output of KY-038 sound sensor module
int led = 13; // Connected to postive of led
boolean is_on = false; //To determine/track if led is on or off

void setup() {
  pinMode(sensor, INPUT); //Setting the pin to input for reading data //Setting the pin to output for turning the led on/off
  Serial.begin(9600);
}

void loop() {

  int data = digitalRead(sensor); //Reading data from sensor and storing in variable

  if (data == 1) { // 1 is sent from sensor when loud noise is detected
    Serial.println("noise");
  }
}
