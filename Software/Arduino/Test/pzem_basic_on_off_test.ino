/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.
  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port
  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include <ModbusMaster.h>
#include <HardwareSerial.h>

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#define RELAY       26
#define SIM_EN      27

// Set these to your desired credentials.
const char *ssid = "yourAP";
const char *password = "yourPassword";

WiFiServer server(80);

HardwareSerial SerialPort(2); // use UART2
ModbusMaster node;

double U_PR, I_PR, P_PR, PPR, PR_F, PR_PF, PR_alarm;
uint8_t result; uint16_t data[6];

int counter = 0;

char buffer[200];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(SIM_EN, OUTPUT);
  digitalWrite(RELAY, HIGH);
  digitalWrite(SIM_EN, HIGH);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
  node.begin(1, SerialPort);
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
            client.print(buffer);

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
          digitalWrite(RELAY, LOW);
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
          digitalWrite(RELAY, HIGH);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }

  if (counter == 50000) {
    counter = 0;
    result = node.readInputRegisters(0x0000, 10);
    if (result == node.ku8MBSuccess) {
    U_PR = (node.getResponseBuffer(0x00)/10.0f);
    I_PR = (node.getResponseBuffer(0x01)/1000.000f);
    P_PR = (node.getResponseBuffer(0x03)/10.0f);
    PPR = (node.getResponseBuffer(0x05)/1000.0f);
    PR_F = (node.getResponseBuffer(0x07)/10.0f);
    PR_PF = (node.getResponseBuffer(0x08)/100.0f);
    PR_alarm = (node.getResponseBuffer(0x09));
    }
    Serial.print("U_PR: "); Serial.println(U_PR); // V
    Serial.print("I_PR: "); Serial.println(I_PR,3); // A
    Serial.print("P_PR: "); Serial.println(P_PR); // W
    Serial.print("PPR: "); Serial.println(PPR,3); // kWh
    Serial.print("PR_F: "); Serial.println(PR_F); // Hz
    Serial.print("PR_PF: "); Serial.println(PR_PF);
    Serial.print("PR_alarm: "); Serial.println(PR_alarm,0);
    Serial.println("====================================================");
  }
  sprintf(buffer, "U_PR: %f\nI_PR: %f\nP_PR: %f\nPPR: %f\nPR_F: %f\nPR_PF: %f\nPR_alarm: %f\n", U_PR, I_PR, P_PR, PPR, PR_F, PR_PF, PR_alarm);

  counter++;
}
