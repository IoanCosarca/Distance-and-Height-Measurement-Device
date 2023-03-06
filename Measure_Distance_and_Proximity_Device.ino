#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <OneButton.h>
#include <ezButton.h>

// Switch ON/OFF
#define SWITCH_PIN 12

// Pins for Ultrasonic Sensor
#define TRIGGER_PIN 3
#define ECHO_PIN 2

// Pins for Bluetooth
#define RX_PIN 9
#define TX_PIN 8

// Pins for Buttons
#define BTN1_PIN 10
#define BTN2_PIN 11

// Pin for LED
#define LED 13

// Pin for Buzzer
#define BUZZER_PIN 7

// enter the I2C address and the dimensions of your LCD here
LiquidCrystal_I2C lcd(0x27, 16, 2);

// long duration, r;
// float distance;
int i = 0;
int delta = 0;
int measure = 0;
long duration;

int oneclick = 0;
int twoclicks = 0;

//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
SoftwareSerial BT(TX_PIN, RX_PIN);

int buttonState1 = 0;
int buttonState2 = 0;
OneButton button(BTN1_PIN, false);

ezButton toggleSwitch(SWITCH_PIN);
int isActive;

// Decide which function to execute
int enfunc1 = 0;
int enfunc2 = 0;

void setup()
{
  // Initialize Switch debounce
  toggleSwitch.setDebounceTime(50);
  
  // Initialize the buttons as input
  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);

  button.attachClick(clicka);
  
  // Initialize LED as output
  pinMode(LED, OUTPUT);

  // Initialize Buzzer as output
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  isActive = 0;
}

void initializeDevice()
{
  isActive = 1;
  Serial.begin(9600);
  BT.begin(9600);

  // Initialize LCD I2C
  lcd.init();
  lcd.clear();
  lcd.backlight();      // Make sure backlight is on
  lcd.setCursor(5, 0);
  lcd.print("Device");
  lcd.setCursor(5, 1);
  lcd.print("Ready");
}

void powerOff()
{
  isActive = 0;
  Serial.end();
  BT.end();
  lcd.clear();
  lcd.noBacklight();
  lcd.noDisplay();
  enfunc2 = 0;
  enfunc1 = 0;
  oneclick = 0;
  twoclicks = 0;
  digitalWrite(LED, LOW);
}

void rangemeter()
{
  if (oneclick == 1)
  {
    lcd.clear();
    lcd.print("Distance in cm:");
  }
  else if (twoclicks == 1)
  {
    lcd.clear();
    lcd.print("Height in cm:");
  }
  
  if (oneclick == 1 || twoclicks == 1)
  {
    lcd.setCursor(0, 1);
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    
    duration = pulseIn(ECHO_PIN, HIGH);
    delta = duration * 0.034 / 2;
    measure = i - delta;
    if (i > 0)
    {
      lcd.print(measure);
      BT.println(measure);
    }
    else {
      lcd.print(delta);
    }
    
    delay (1000);
  }
}

void clicka()
{
  if (oneclick == 1) {
    i = 0;
  }
  else if (twoclicks == 1)
  {
    BT.println("Your height:");
    i = delta;
  }
}

void alarm()
{
  lcd.clear();
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  delta = duration * 0.034 / 2;
  if (delta <= 30)
  {
    tone(BUZZER_PIN, 1200);
    delay(500);
    noTone(BUZZER_PIN);
    BT.println("A aparut un aurolac salbatic!");
  }
  else {
    noTone(BUZZER_PIN);
  }
}

void loop()
{
  toggleSwitch.loop();
  if (toggleSwitch.isPressed()) {
    if (isActive == 0) {
      initializeDevice();
    }
  }
  
  if (isActive == 1)
  {
    buttonState1 = digitalRead(BTN1_PIN);
    buttonState2 = digitalRead(BTN2_PIN);
    
    button.tick();
    if (buttonState1 == HIGH)
    {
      enfunc1 = 1;
      enfunc2 = 0;
      if (oneclick == 0 && twoclicks == 0) {
        oneclick = 1;
      }
      else
      {
        if (twoclicks == 1)
        {
          oneclick = 1;
          twoclicks = 0;
        }
        else if (oneclick == 1)
        {
          oneclick = 0;
          twoclicks = 1;
        }
      }
    }
    if (buttonState2 == HIGH)
    {
      enfunc2 = 1;
      enfunc1 = 0;
      oneclick = 0;
      twoclicks = 0;
      BT.println("Connection Made");
    }
    
    if (enfunc1 == 1)
    {
      digitalWrite(LED, LOW);
      rangemeter();
    }
    if (enfunc2 == 1)
    {
      digitalWrite(LED, HIGH);
      alarm();
    }
  }
  
  if (toggleSwitch.isReleased()) {
    powerOff();
  }
}
