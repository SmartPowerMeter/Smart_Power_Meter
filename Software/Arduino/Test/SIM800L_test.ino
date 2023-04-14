#include <HardwareSerial.h>

HardwareSerial SIM800(1); // use UART1

// After running this code, SIM800L is available from serial port
// AT Commands can be given to SIM800L module through esp module

void setup() {
  // put your setup code here, to run once:
  SIM800.begin(115200, SERIAL_8N1, 13, 14); 
  Serial.begin(115200);
  delay(3000);
  Serial.println("SIM800L Ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  while (Serial.available())
  {
    SIM800.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (SIM800.available())
  {
    Serial.write(SIM800.read());//Forward what Software Serial received to Serial Port
  }
}
