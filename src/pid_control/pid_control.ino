#include <LiquidCrystal.h>

LiquidCrystal lcd(1, 0, 9, 8, 7, 6);

#define PWM_PIN  5
#define MOTOR_EN1_PIN  12
#define MOTOR_EN2_PIN  11
#define encoder0PinA 3
#define encoder0PinB 2

#define PIN_INPUT_P A0
#define PIN_INPUT_I A1
#define PIN_INPUT_D A2

#define PIN_INPUT_REF A3
#define PIN_OUTPUT_CTRL A4

// Encoder
volatile long  encoderCount = 42;
volatile bool isRotatingCCW = true;
long  encoderCountTotal = 0;

// Motor
int motorSpeed = 0;
int rotateCCW;

// Control Internal
int cntrlP = 0;
int cntrlI = 0;
int cntrlD = 0;
int cntrlSignal = 0;
int error,lastError = 0;

// Timers (ms)
unsigned long currentTime;
unsigned long lastTime;
const unsigned long periodTime = 100;

void setup() {
  
    // PID - Input
    pinMode(PIN_INPUT_P, INPUT_PULLUP);
    pinMode(PIN_INPUT_I, INPUT_PULLUP);
	pinMode(PIN_INPUT_D, INPUT_PULLUP);
  
    // Reference - Input
    pinMode(PIN_INPUT_REF, INPUT_PULLUP);
  
    // Encoder
  	pinMode(encoder0PinA, INPUT_PULLUP);
  	pinMode(encoder0PinB, INPUT_PULLUP);

	// interrupt signal to Encoder PIN
    attachInterrupt(digitalPinToInterrupt(encoder0PinA), isrCount, RISING); 
  
    // Motor
	pinMode(PWM_PIN, OUTPUT);
  	pinMode(MOTOR_EN1_PIN, OUTPUT);
  	pinMode(MOTOR_EN2_PIN, OUTPUT);
  
  	// Signal Display
  	pinMode(PIN_OUTPUT_CTRL, OUTPUT);

    // LCD Setup
	lcd.begin(16, 2);    //Definindo o LCD com 16 colunas e 2 linhas	
	lcd.clear();         // Limpa LCD
  	lcd.setCursor(0, 0); //Definindo o cursor na posição inicial do LCD
}

void loop(){
  	if(millis() > lastTime + periodTime)
    {
      lastTime = millis(); // Reset Timer
      
      // Read Input (Range: 0-1023)
      int inputRead = analogRead(PIN_INPUT_REF);
      int inputP = analogRead(PIN_INPUT_P);
      int inputI = analogRead(PIN_INPUT_I);
      int inputD = analogRead(PIN_INPUT_D);

      // Read Encoder
      noInterrupts();
      encoderCountTotal += encoderCount;
      encoderCount = 0;
      interrupts();
      
      // PID Control
      lastError = error;
      error = inputRead - encoderCount;
      cntrlP = inputP*error;
      cntrlI = cntrlI + inputI*error*periodTime;
      cntrlD = inputD*(error-lastError)/periodTime;
      cntrlSignal = (cntrlP + cntrlI + cntrlD) >> 5;
      if(cntrlSignal >= 0)
      {
        rotateCCW = LOW;
        motorSpeed = cntrlSignal;
      }
      else
      {
        rotateCCW = HIGH;
        motorSpeed = -cntrlSignal;
      }

      // LCD - Print Variables
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SP ");
      lcd.print(motorSpeed);
      lcd.setCursor(8, 0);
      lcd.print("SE ");
      lcd.print(inputRead);
      lcd.setCursor(0, 1);
      lcd.print("C ");
      lcd.print(inputP);
      lcd.setCursor(6, 1);
      lcd.print(" ");
      lcd.print(inputI);
      lcd.setCursor(11, 1);
      lcd.print(" ");
      lcd.print(inputD);
      
      // Control Motor
      digitalWrite(MOTOR_EN2_PIN, rotateCCW);
      digitalWrite(MOTOR_EN1_PIN, !rotateCCW);
      analogWrite(PWM_PIN, motorSpeed); // Range 0-255
    }
}

/**
 * Called at Each Interruption
 */
void isrCount()
{
  isRotatingCCW = digitalRead(encoder0PinB);
  if(isRotatingCCW == HIGH)
  {
    encoderCount--;
  }
  else
  {
    encoderCount++;
  }
}
