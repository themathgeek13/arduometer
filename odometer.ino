 #include <EEPROMVar.h>
#include <EEPROMex.h>

// set pin numbers:
#include<LiquidCrystal.h>
#define BUTTON_PIN 13
#define LCD_LIGHT_PIN A4
#define LCD_LIGHT_ON_TIME 3000
#define PI 3.141592653589793438

unsigned int currentLcdLightOnTime=0;
unsigned long lcdLightOn_StartMillis;

boolean isLcdLightOn;

int pushState=0;
int address=0;

const int magneticSwitch = 7;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin
const int pushButton=8;
LiquidCrystal lcd(12,11,5,4,3,2);

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int count=0;
double distance=0.00000000000000;
double overallDist = 0.000000000000000;
double savedDist=0.00000000000000;
float velocity;
long currentTime=10;
/*long current;
int count1;
int count2; */
long lastTime=0;
double radius=0.75;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay =5;    // the debounce time; increase if the output flickers

void setup() {
  Serial.begin(9600);
  pinMode(magneticSwitch, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LCD_LIGHT_PIN, OUTPUT);
  pinMode(pushButton, INPUT);
  
  //EEPROM.writeDouble(0.0000000000,0);
  Serial.print(address);
  Serial.print("\t");
  Serial.print(distance);
  Serial.println();
  
  savedDist=EEPROM.readDouble(address);
  overallDist=savedDist;

  lcd.begin(16,2);
  //lcd.print("Count: 0");
  lcd.print("V:0.00 | n=0");
  lcd.setCursor(0,1);
  lcd.print("C:0.00|T:");
  lcd.print(overallDist);

  // set initial LED state
  digitalWrite(ledPin, ledState);
  digitalWrite(LCD_LIGHT_PIN, LOW);
  isLcdLightOn=false;
 
}

void loop() {
  //current=millis();
  // read the state of the switch into a local variable:
  int reading = digitalRead(magneticSwitch);
  pushState=digitalRead(BUTTON_PIN);
  int memRead=digitalRead(pushButton);
  
  if(memRead==HIGH)
  {
    savedDist = EEPROM.readDouble(address);
  Serial.println("\Saved distance as per EEPROM: ");
  Serial.println(savedDist);
  if(overallDist!=savedDist && distance>0)
  {
    //overallDist+=distance;
    EEPROM.writeDouble(address, overallDist);
    savedDist=overallDist;
    Serial.println("\nWritten to memory!");
  }
}

double initial;
double anyDist;
  
  if(pushState==HIGH){
   Serial.println("Button pressed!");
   if(isLcdLightOn==false && currentLcdLightOnTime>50)
   {  
     lcdLightOn_StartMillis = millis();
     currentLcdLightOnTime = 0;
     isLcdLightOn = true;
     digitalWrite(LCD_LIGHT_PIN, HIGH);
     ledState=!ledState;
   }
   else if(isLcdLightOn==true && currentLcdLightOnTime>50)
   {
     isLcdLightOn=false;
     digitalWrite(LCD_LIGHT_PIN, LOW); 
     currentLcdLightOnTime=0;
     ledState=!ledState;
   }
  }
  else{
    // Button not pressed.
    //Serial.println("Button not pressed - LOW");
 
   
      currentLcdLightOnTime = millis() - lcdLightOn_StartMillis;
      if(currentLcdLightOnTime > LCD_LIGHT_ON_TIME && isLcdLightOn){
        isLcdLightOn = false;
        digitalWrite(LCD_LIGHT_PIN, LOW);
      }
  }
 
    Serial.print("\nMagnetic: ");
    Serial.print(reading);
  //Serial.print("Lcd light on time: ");
  //Serial.println(currentLcdLightOnTime);
  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
     
      
      lcd.setCursor(0,0);
      lcd.print("V:");
      if(currentTime-lastTime<20)
      velocity=0.000000000;
      else
      velocity=2*PI*radius*1000/(currentTime-lastTime);
      if(millis()-lastTime>5000)
      velocity=0.0000000000;
      lcd.print(velocity);

    // if the button state has changed:
   if (reading != buttonState) {
      buttonState = reading;
      count++;
      lcd.print(" | n = ");
      lcd.print(count/2);
     // lcd.setCursor(0,1);
     // lcd.print("C:");
      if(count%2==0){
      initial=distance;
      anyDist=overallDist;
      while(distance<=initial+2*PI*radius && overallDist<=anyDist+2*PI*radius)
      {
        distance+=0.2;
        overallDist+=0.2;
        lcd.setCursor(0,1);
        lcd.print("C:");
        lcd.print(distance);
        lcd.print(" T:");
        lcd.print(overallDist);
        delay(2);
      }
      distance=initial+2*PI*radius;
      overallDist=anyDist+2*PI*radius;
      
      }
      lcd.setCursor(0,1);
      lcd.print("C:");
      lcd.print(distance);
      lcd.print("T:");
      lcd.print(overallDist);
      
      
      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        lastTime=currentTime;
        currentTime=millis();
      }
   }
 }
  
  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}
