#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <PZEM004Tv30.h>

#define RXD2 16
#define TXD2 17

LiquidCrystal_I2C lcd(0x27,16,2); 
PZEM004Tv30 pzem(Serial2, RXD2, TXD2);
uint8_t pinRelay = 19;

#define RELAY_OPEN LOW
#define RELAY_CLOSE HIGH

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, RELAY_OPEN);


  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);

  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();
  lcd.print("Made In China");
  lcd.setCursor(0,1);
  lcd.print("XD XD XD");
  delay(3000);

}

void loop() {

  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();



  String voltz = String(voltage) + "V";
  String amp = String(current) + "A";
  String pow = String(power) + "W";
  String powerF = String(pf) + "pf";

  lcd.clear();
  lcd.print(voltz + " " + amp);
  lcd.setCursor(0, 1);
  lcd.print(pow + " " + powerF);


  if(power > 1000.0f) {
    digitalWrite(pinRelay, RELAY_CLOSE);
    lcd.clear();
    lcd.print("Power Limit");
    lcd.setCursor(0,1);
    lcd.print("Has Been Reach!");
    delay(8000);
    lcd.clear();
    lcd.print("  Reset Relay  ");
    lcd.setCursor(0,1);
    lcd.print("Please Wait...");
    delay(15000);
    digitalWrite(pinRelay, RELAY_OPEN);
    pzem.resetEnergy();
  }

  // Serial.println("-------------------------------------");
  // Serial.println(voltz);
  // Serial.println(amp);
  // Serial.println(pow);
  // Serial.println();
  // Serial.println("Freq: " + String(frequency));
  // Serial.println(powerF);
  // Serial.println("-------------------------------------");
  delay(1750);
}

