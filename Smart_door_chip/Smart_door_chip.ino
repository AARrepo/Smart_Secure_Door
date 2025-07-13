#include <SoftwareSerial.h>

SoftwareSerial btSerial(D7, D6); // RX = D7, TX = D6

void setup() {
  Serial.begin(115200);        // PC Serial Monitor
  delay(5000);               // A little delay so that serial monitor can open

  btSerial.begin(9600);      // HC-05 default baud
  

  Serial.println(">>> Starting test with SofwareSerial on D7 (GPIO13)");
}

void loop() {
  // From mobile device to PC 
 if (btSerial.available()) {
    delay(100); // Delay to make sure the whole message gets recieved in one line
    Serial.print("Fra HC-05: ");
    while (btSerial.available()) {
      char c = btSerial.read();
      Serial.print(c);
    }
    Serial.println();
  }

  // From PC to mobile device 
  if (Serial.available()){
    Serial.print("Fra serial monitor: ");
  
  while (Serial.available()) {
    char c = Serial.read();
    Serial.print(c); 
    btSerial.print(c);
    }
  }
}
