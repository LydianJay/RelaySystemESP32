#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <WiFi.h>

#define RELAY_OPEN LOW
#define RELAY_CLOSE HIGH


#define RXD2 16
#define TXD2 17

struct PZEMReading {
  float voltage;
  float power;
  float current;
  float pf;
  float energy;
};


struct SendData {
  float voltage;
  float power;
  float energy;
};

PZEMReading reading;

LiquidCrystal_I2C lcd(0x27,16,2); 
PZEM004Tv30 pzem(Serial2, RXD2, TXD2);
uint8_t pinRelay = 19;

WiFiServer server(1322);

uint32_t powerLimit = 1500;

void setup() {
  
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, RELAY_CLOSE);
  WiFi.mode(WIFI_AP); 
  WiFi.softAP("RelaySystem", "admin123");
  server.begin();
  delay(2000);
  lcd.init();                      
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  Power Limit");
  lcd.setCursor(0,1);
  lcd.print("  Consumption");
  lcd.print(WiFi.softAPIP());
  delay(2000);
  digitalWrite(pinRelay, RELAY_OPEN);
}



void getReadings() {
  reading.voltage = pzem.voltage();
  reading.current = pzem.current();
  reading.power = pzem.power();
  reading.energy = pzem.energy();
  reading.pf = pzem.pf();
  
}


void displayToLCD() {
  String voltz = String(reading.voltage) + "V";
  String amp = String(reading.current) + "A";
  String pow = String(reading.power) + "W";
  String powerF = String(reading.pf) + "pf";

  lcd.clear();
  lcd.print(voltz + " " + amp);
  lcd.setCursor(0, 1);
  lcd.print(pow + " " + powerF);
}


void handlePowerLimit() {
  if(reading.power >= powerLimit) {
    digitalWrite(pinRelay, RELAY_CLOSE);
    lcd.clear();
    lcd.print("Power Limit");
    lcd.setCursor(0,1);
    lcd.print("Has Been Reach!");
    delay(8000);
    lcd.clear();
    lcd.print("Resetting Relay");
    lcd.setCursor(0,1);
    lcd.print("Please Wait...");
    delay(15000);
    digitalWrite(pinRelay, RELAY_OPEN);
    pzem.resetEnergy();
  }
}

void loop() {
  WiFiClient client = server.available();

 
  getReadings();
  displayToLCD();


  if(client) {
    while (client.connected()) {
      getReadings();
      displayToLCD();
      if( client.available() ) {
        String limitStr = client.readString();
        powerLimit = limitStr.toFloat();
      }

      SendData sData;
      sData.energy  = reading.energy;
      sData.power   = reading.power;
      sData.voltage = reading.voltage;
      String s = String(reading.energy) + "," + String(reading.power) + "," + String(reading.voltage);
      client.write(s.c_str());
      handlePowerLimit();
      delay(1000);
    } 
    
  }

  handlePowerLimit();
  

  // Serial.println("-------------------------------------");
  // Serial.println(voltz);
  // Serial.println(amp);
  // Serial.println(pow);
  // Serial.println();
  // Serial.println("Freq: " + String(frequency));
  // Serial.println(powerF);
  // Serial.println("-------------------------------------");
  delay(1000);
}

