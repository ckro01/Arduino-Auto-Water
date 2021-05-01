#include "DHT.h"
#include "pitches.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define POWER_PIN  13 
#define soil_pin A2
#define DHTPIN 8    
#define DHTTYPE DHT11   
#define debounce 50 
#define holdTime 2000 
#define toggleTime 5000 
DHT dht(DHTPIN, DHTTYPE);
int melody[] = { //nokia
  NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4, 
  NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4, 
  NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
  NOTE_A4, 2, 
};
int value = 0,waterValve = 20,redLed = 4,greenLed = 5,tempo = 180,buzzer = 3,notes = sizeof(melody) / sizeof(melody[0]) / 2,wholenote = (60000 * 4) / tempo,divider = 0, noteDuration = 0,outPin = A0,x=0,val=0,buttonVal = 0,buttonLast = 0,modePin = 2,trial=0;
volatile byte mState = LOW;  
long hour = 0, minute = 0, second = 0,current=0,countup_time = (hour*3600) + (minute * 60) + second,btnDnTime,btnUpTime;
boolean ignoreUp = false;
void setup(){
  redSTATE(1);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Check case for display errors
  }
  display.clearDisplay();
  startUpDisplay();
  testscrolltext();    
  pinMode(outPin,OUTPUT);
  pinMode(modePin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(modePin),isr,CHANGE);
  pinMode(POWER_PIN, OUTPUT);   
  digitalWrite(POWER_PIN, LOW); 
  pinMode(buzzer, OUTPUT); 
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("DHT11 INITIALIZED"));
  Serial.println("LEDS INITIALIZED");
  Serial.println("BUZZER INITIALIZED");
  dht.begin();
  delay(1000);
  greenSTATE(1);
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.print("  Initialization\n");
  display.setCursor(15,10);
  display.print("    Completed");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
  display.print("MODE: TEMP\n");
  display.setCursor(10,17);
  display.print("NO ERRORS");
  display.display();
  delay(3000);
  redSTATE(0);
  display.clearDisplay();
  display.display();
}
void startUpDisplay(void) {
   display.clearDisplay();
  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, INVERSE);
    display.display();
    delay(1);
  }
  delay(500);
  display.clearDisplay();
  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, WHITE);
    display.display();
    delay(1);
  }
  delay(500);
}
void testscrolltext(void) {
  display.clearDisplay();
  display.setTextSize(2); 
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(F("Loading.."));
  display.display();     
  delay(100);
  display.startscrollright(0x00, 0x0F);
  delay(500);
  chime();
  display.stopscroll();
  display.startscrollleft(0x00, 0x0F);
  delay(250);
  display.stopscroll();
  delay(500);
}
void redSTATE(bool STATE) {if (STATE == 1) { digitalWrite(redLed,HIGH);}else {digitalWrite(redLed,LOW);}}
void greenSTATE(bool STATE) {if (STATE == 1) {digitalWrite(greenLed,HIGH);} else {digitalWrite(greenLed,LOW);}}
void chime(){ //nokia
  redSTATE(0);
  greenSTATE(1);
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }
    val++;
    if (val > 1) val=0;
    redSTATE(val);
    greenSTATE(val);
    tone(buzzer, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(buzzer);
  }
}
void getTemp() {
  isr();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true)-7;
  if (isnan(h) || isnan(t) || isnan(f)) {
    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(10,0);
    display.print(" ERROR READING\n");
    display.setCursor(15,10);
    display.print("   TEMP SENSOR");
    display.display();
    delay(5000);
    Serial.println(F("Failed to read from DHT sensor!"));
    alert();
    isr();
    return;
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Temp: ");
  display.print(int(f));
  display.print(" F");
  display.setCursor(0,17);
  display.print("Hum:  ");
  display.print(int(h));
  display.print(" %");
  display.display();
  isr();
  delay(500);
  isr();
 
}
void alert(){
  tone(buzzer,1000);
  delay(500);
  redSTATE(1);
  greenSTATE(0);
  noTone(buzzer);
  delay(1000);
  redSTATE(0);
  greenSTATE(1);
}
void water(){
  x=2;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(F("Watering."));
  display.display();     
  delay(100);
  display.startscrollright(0x00, 0x0F);
  delay(500);
  display.stopscroll();
  display.startscrollleft(0x00, 0x0F);
  delay(250);
  display.stopscroll();
  delay(500);
  display.clearDisplay();
  Serial.print("Watering");
  redSTATE(1);
  tone(buzzer,1000);
  delay(100);
  noTone(buzzer);
  redSTATE(0);
  delay(200);
  redSTATE(1);
  tone(buzzer,1000);
  delay(100);
  noTone(buzzer);
  redSTATE(0);
  digitalWrite(outPin,HIGH);
  delay(3000);
  alert();
  digitalWrite(outPin,LOW);
  current=millis()/1000;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10,0);
  display.println(F(" Reading \n          Soil \n              Sat"));
  display.display();     
  delay(100);
  display.startscrollleft(0x00, 0x0F);
  delay(4000);
  display.stopscroll();
  display.clearDisplay();
  display.setCursor(10,10);
  delay(450);
  display.setTextSize(2);
  display.println(F(" Reading"));
  display.setCursor(40,10);
  display.display();
  delay(450);
  display.clearDisplay();
  display.println(F("Soil"));
  display.setCursor(50,10);
  display.display();
  delay(450);
  display.clearDisplay();
  display.println(F("Sat"));
  delay(450);
  display.display();
  display.clearDisplay();
  delay(5000);
}
void mode(int choice){
  if (choice==1) {Time();}
  if (choice==2) {Soil();}
  if (choice==0) {getTemp();} }
void Time(){
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2); 
  display.setTextColor(WHITE);
  display.setCursor(5, 0);
  display.println(F("Last Water"));
  delay(100);
  long countuptime_seconds = countup_time + (millis() / 1000)-current;
  if (countuptime_seconds >= 0) {
    long countup_hour = countuptime_seconds / 3600;
    long countup_minute = ((countuptime_seconds / 60)%60);
    long countup_sec = countuptime_seconds % 60;
    display.setCursor(13, 17);
    if (countup_hour < 10) {
    display.print(F("0"));
    }
    display.print((countup_hour));
    display.print(F(":"));;
    if (countup_minute < 10) {
    display.print(F("0"));
    }
    display.print((countup_minute));
    display.print(F(":"));;
    if (countup_sec < 10) {
    display.print(F("0"));
    }
    display.print((countup_sec));
    display.print(F(""));;
  } display.display();
}
void isr() {
  buttonVal = digitalRead(modePin);
  if (buttonVal == HIGH && buttonLast == LOW && (millis() - btnUpTime) > long(debounce))
  {
  btnDnTime = millis();
  }
  if (buttonVal == LOW && buttonLast == HIGH && (millis() - btnDnTime) > long(debounce))
  {
  if (ignoreUp == false) event1();
  else ignoreUp = false;
  btnUpTime = millis();
  }
  if (buttonVal == HIGH && (millis() - btnDnTime) > long(holdTime))
  {
  event2();
  ignoreUp = true;
  btnDnTime = millis();
  }
  buttonLast = buttonVal;
  }
void Soil(){
  Serial.println("Val: ");
  Serial.println(int(trial));
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(5,0);
  display.print(" Soil Sat:");
  display.setCursor(35,17);
  display.print("->");
  display.print(trial);
  display.print("<-");
  display.display();
  delay(500);
}
void event1()
{
    x++;
    redSTATE(1);
    delay(100);
    Serial.print(x);
    delay(20);
    redSTATE(0);
    greenSTATE(1);
    if (x>2) {
      x=0;
    }
}
void event2()
{
  water();
}
void loop(){
  mode(x);
  trial = analogRead(soil_pin);
  trial = map(trial,0,1023,0,100);
  trial = (trial*-1 + 100);
  if (trial < 20) {
    water();
  }
}
