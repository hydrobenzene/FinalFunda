  
SYSTEM_THREAD(ENABLED);
#include <Wire.h>
#include <blynk.h>
#include "SparkFun_VCNL4040_Arduino_Library.h"
#include "oled-wing-adafruit.h"

VCNL4040 vc;
OledWingAdafruit oled = oledSetup;

// Values can be modified!
int lowLight;
int highLight;

String ILOT = "null";

void setup() {
  // LED Pins
  pinMode(D3, OUTPUT); // HIGH
  pinMode(D2, OUTPUT); // NORMAL
  pinMode(D1, OUTPUT); // LOW

  // Input Pins
  pinMode(A2, INPUT);
  pinMode(A1, INPUT);
  pinMode(D1, INPUT);

  // Turn off Pins
  digitalWrite(D6, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D4, LOW);

  Blynk.begin("{TOKEN}", IPAddress(000, 000, 000, 000), 0000);
  Blynk.run();

  // Start I2C Connection
  Wire.begin();

  calibrate();
}


void loop() {
 oled.loop();

  // Trigger either LIGHT or TEMPERATURE based on response.
  if(oled.pressedA()) ILOT = "L";
  if(oled.pressedB()) ILOT = "T";

  // LIGHT response.
  if(ILOT = "L"){
    vc.powerOnAmbient();
    
    // Check Light Status
    if(vc.getAmbient() < lowLight){
      digitalWrite(D3, LOW);
      digitalWrite(D2, LOW);
      digitalWrite(D1, HIGH);
      Blynk.notify("Light levels are lower than normal.");
    }
    if(vc.getAmbient() < highLight){
      digitalWrite(D3, LOW);
      digitalWrite(D2, HIGH);
      digitalWrite(D1, LOW);
      Blynk.notify("Light levels are normal.");
    }
    else {
      digitalWrite(D3, HIGH);
      digitalWrite(D2, LOW);
      digitalWrite(D1, LOW);
      Blynk.notify("Light levels are higher than normal.");
    }

    oled.println("LuxVal: " + vc.getAmbient())
  }
  if(ILOT = "T"){
    digitalWrite(D6, LOW);
    digitalWrite(D5, LOW);
    digitalWrite(D4, LOW);
    
    oled.println("C: " + (((analogRead(A1) * 3.3) / 4095.0)- 0.5) * 100 + "\nF: " + (((((analogRead(A1) * 3.3) / 4095.0)- 0.5) * 100) * 9 / 5) + 32)
    Blynk.virtualWrite(V0, "C: " + (((analogRead(A1) * 3.3) / 4095.0)- 0.5) * 100 + "\nF: " + (((((analogRead(A1) * 3.3) / 4095.0)- 0.5) * 100) * 9 / 5) + 32)
    ;
  }

}

void calibrate(){
  // Two lights being on signals calibration
  digitalWrite(D3, LOW);
  digitalWrite(D2, HIGH);
  digitalWrite(D1, HIGH);
  lowLight = analogRead(A5);
  delay(5000)
  digitalWrite(D3, HIGH);
  digitalWrite(D2, HIGH); // All three lights signals change in calibration
  digitalWrite(D1, HIGH);
  delay(1000)
  digitalWrite(D3, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D1, LOW);
  highLight = analogRead(A5);
}

oled oledSetup(){
  oled.setup(); // Initialize OLED
  oled.clearDisplay(); // Clear OLED Screen
  oled.setCursor(0,0); // Reset cursor
  oled.setTextSize(0); // Set text size to 0
	oled.display(); // Update OLED
}