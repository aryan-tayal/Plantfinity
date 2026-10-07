#include "DHT.h"
#include <arduino-timer.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>

#define SCL_PIN 2
#define SCL_PORT PORTD
#define SDA_PIN 0
#define SDA_PORT PORTC
#include <SoftI2CMaster.h>

#ifdef __AVR__
#endif


DHT dht(2, DHT11);
Adafruit_NeoPixel pixels(60, 3, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(128, 32, &Wire, -1);


auto dataTimer = timer_create_default();

unsigned long timer;

const byte soilPin = A0;
const byte pumpPin = 6 ;
const byte pingPin = 4;
const byte echoPin = 5;

const byte soilHumidity[] = {60, 80};
const byte growLamp[] = {200 , 20, 255};
byte loops = 0;
bool isLampOn = true;

void setup() {
  Serial.begin(9600);
  pinMode(soilPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, HIGH);
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);
  dht.begin();
  pixels.begin();
  dataTimer.every(1500, sendData);
  createLamp();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

int readPlantHeight() {
  long duration;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  int distance = 24 - (duration * 0.034 / 2);
  if(distance<1){
    return 0;
  }
  else{
    return distance;
  }
}

int readSoilHumidity() {
  byte soilValue = map(analogRead(soilPin), 0, 1023, 100, 0);
  return soilValue;
}
int readAtmosHumidity() {
  float h = map(dht.readHumidity(), 0, 100, 0, 80);
  if (isnan(h)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  return h;
}
int readAtmosTemp() {
  float t = dht.readTemperature();
  if ( isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  return t;
}


void writeToOLED(char *field, int value, char *unit)
{
  //clear display
  display.clearDisplay();

  // display data
  display.setTextSize(1);
  display.setCursor(0, 5);
  display.print(field);
  display.setTextSize(2);
  display.setCursor(0, 17);
  display.print(value);
  display.print(" ");
  display.print(unit);
  display.display();
}
void changeOLED() {
  int s = readSoilHumidity();
  int h = readAtmosHumidity();
  int t = readAtmosTemp();
  int g = readPlantHeight();
  if (loops % 4 == 0) {
    writeToOLED("Soil Humidity", s, "%");
  } else if (loops % 4 == 1) {
    writeToOLED("Temperature", t, "C");
  } else if (loops % 4 == 2) {
    writeToOLED("Humidity", h, "%");
  }
  else {
    writeToOLED("Plant Height", g, "cm");
  }
  return true;
}


void sendData() {
  int s = readSoilHumidity();
  int h = readAtmosHumidity();
  int t = readAtmosTemp();
  int g = readPlantHeight();
  if (loops % 4 == 0) {
    Serial.print(F("s"));
    Serial.println(s);
  } else if (loops % 4 == 1) {
    Serial.print(F("h"));
    Serial.println(h);
  } else if (loops % 4 == 2) {
    Serial.print(F("t"));
    Serial.println(t);
  }
  else  {
    Serial.print(F("g"));
    Serial.println(g);
  }
  if (s < soilHumidity[0]) {
    Serial.println("Water");
    digitalWrite(pumpPin, LOW);
    delay(10000);
    digitalWrite(pumpPin, HIGH);
    delay(10000);
  }
  return true;
}
void changeLampState() {
  if (isLampOn) {
    clearLamp();
    isLampOn = false;
  } else {
    createLamp();
    isLampOn = true;
  }
  return true;
}
void createLamp () {
  for (int j = 1; j <= 255; j++) {
    for (int i = 0; i < pixels.numPixels(); i++) {
      int r = growLamp[0]  <= j ? growLamp[0] : j  ;
      int g = growLamp[1]  <= j ? growLamp[1] : j;
      int b = growLamp[2]  <= j ? growLamp[2] : j;
      pixels.setPixelColor(i, r, g, b);
    }
    pixels.show();
    delay(10);
  }
}
void clearLamp() {
  for (int j = 1; j <= 255; j++) {
    for (int i = 0; i < pixels.numPixels(); i++) {
      int r = growLamp[0] - j >= 0 ? growLamp[0] - j : 0 ;
      int g = growLamp[1] - j >= 0 ? growLamp[1] - j : 0;
      int b = growLamp[2] - j >= 0 ? growLamp[2] - j : 0;
      pixels.setPixelColor(i, r, g, b);
    }
    pixels.show();
    delay(10);
  }
}

void loop() {
  loops += 1;
  timer = millis() / 1000;
  if (timer % 60 == 0) {
    changeLampState();
  }
  if(timer % 2 == 0){
    changeOLED();
  }
  dataTimer.tick();
  delay(1500);
}
