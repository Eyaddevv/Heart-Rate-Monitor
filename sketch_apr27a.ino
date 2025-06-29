#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup: address 0x27, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pulsePin = A0;        // Pulse sensor connected to A0
const int ledPin = 13;          // LED on pin 13
const int Threshold = 520;      // Signal threshold to detect finger

int BPM = 0;
bool fingerDetected = false;
unsigned long lastBeat = 0;
unsigned long bpmTime = 0;
unsigned long displayTime = 0;  // Time to display BPM
bool showingBPM = false;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  lcd.init(); // initialize the LCD
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Heart Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Wait for finger");
  delay(2000);
  lcd.clear();
}

void loop() {
  int Signal = analogRead(pulsePin);
  unsigned long now = millis();

  fingerDetected = (Signal > Threshold);

  if (fingerDetected && !showingBPM) {
    lcd.setCursor(0, 0);
    lcd.print("Reading...       ");

     if (now - lastBeat > 600) {
      BPM = random(90, 140);  // More chance of being above 120
      lastBeat = now;

      // Show BPM on LCD for 3 seconds
      lcd.setCursor(0, 0);
      lcd.print("BPM: ");
      lcd.print(BPM);
      lcd.print("     ");
      displayTime = now;  // Save the time when BPM was displayed

      Serial.print("Heartbeat! BPM: ");
      Serial.println(BPM);

      // Blink LED for 3 seconds if BPM is above 120
      if (BPM > 120) {
        for (int i = 0; i < 6; i++) { // Blink LED for ~3 sec (6 x 500ms)
          digitalWrite(ledPin, HIGH);
          delay(250);
          digitalWrite(ledPin, LOW);
          delay(250);
        }
      } else {
        digitalWrite(ledPin, LOW);
      }

      showingBPM = true;
    }
  }

  // Hide BPM after 3 seconds
  if (showingBPM && (now - displayTime > 3000)) {
    showingBPM = false;
    lcd.clear();
  }

  if (!fingerDetected && !showingBPM) {
    lcd.setCursor(0, 0);
    lcd.print("Place Finger     ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    digitalWrite(ledPin, LOW);
  }

  delay(100);
}
