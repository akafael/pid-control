// Libraries --------------------------------------------------------------------------------
#include <LiquidCrystal.h>

// Constants --------------------------------------------------------------------------------

// PID Control Pins
#define PIN_INPUT_P     A0
#define PIN_INPUT_I     A1
#define PIN_INPUT_D     A2
#define PIN_INPUT_REF   A3

// Motor Pins
#define PWM_PIN         5
#define MOTOR_EN_PIN    4

// Encoder Pins
#define encoder0PinA    3
#define encoder0PinB    2

// Global Variables -------------------------------------------------------------------------

// LDC PINs Setup
LiquidCrystal lcd(1, 0, 9, 8, 7, 6);

volatile unsigned long  encoderCount = 0;
unsigned long  encoderCountTotal = 0;
int motorSpeed = 0;

// Main Functions ---------------------------------------------------------------------------

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
  	pinMode(MOTOR_EN_PIN, OUTPUT);

    // LCD Setup
	  lcd.begin(16, 2);       //Definindo o LCD com 16 colunas e 2 linhas	
	  lcd.clear();            // Cla
  	lcd.setCursor(0, 0);  //Definindo o cursor na posição inicial do LCD
}

void loop(){
    // Read Input (Range: 0-1023)
    int inputRead = analogRead(PIN_INPUT_REF);
    
    // Read Encoder
    noInterrupts();
    encoderCountTotal += encoderCount;
    encoderCount = 0;
    interrupts();

    // Control Action
    motorSpeed = inputRead >> 2; // Divide by 4
  
    // LCD - Print Variables
    lcd.clear();
	  lcd.setCursor(0, 0);
	  lcd.print("V: ");
  	lcd.print(motorSpeed);
  	lcd.setCursor(0, 1);
	  lcd.print("E: ");
	  lcd.print(encoderCountTotal);
  
    // Control Motor
    digitalWrite(MOTOR_EN_PIN, HIGH);
    analogWrite(PWM_PIN, motorSpeed); // Range 0-255

	  delay(100); // Just a Random Delay to slow down everything
}

// Aux Functions ----------------------------------------------------------------------------

/**
 * Called at Each Interruption
 */
void isrCount()
{
  encoderCount++;
}
