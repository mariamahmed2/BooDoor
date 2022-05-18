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
// Set these to your desired credentials.
const char *ssid = "Door Lock";
const char *password = "12345678";
WiFiServer server(80);


#define LED_BUILTIN 4  // Ligth Sys. Led


#include <Keypad.h>

#define LED   2  // Door Indecation
#define ROW_NUM     4  // four rows
#define COLUMN_NUM  4  // four columns


char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {13, 12, 14, 27};    // GIOP19, GIOP18, GIOP5, GIOP17 connect to the row pins
byte pin_column[COLUMN_NUM] = {26, 25, 33, 32}; // GIOP16, GIOP4, GIOP0, GIOP2 connect to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password_1 = "123"; // change your password here
const String password_2 = "1234";  // change your password here
const String password_3 = "12345";  // change your password here
String input_password;

#include <Servo.h>
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int DoorOpen = 0; //idicatour

void setup() {
 
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("Hello, Mo3ed!");
  Serial.println();
  Serial.println("Configuring access point...");
  input_password.reserve(32); // maximum input characters is 32
  pinMode(LED, OUTPUT); 
  digitalWrite(LED, LOW ); // lock the door

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

   myservo.attach(18);  // attaches the servo on pin 18 to the servo object
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
        if (currentLine.endsWith("Led/1")) {
          digitalWrite(LED_BUILTIN, HIGH);               // GET Led/1 turns the LED on
        }
        if (currentLine.endsWith("Led/0")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET Led/1 turns the LED off
        }
        if (currentLine.endsWith("door/1")) {
          digitalWrite(2, HIGH);                // GET door/1 turns the door LED on
        if(DoorOpen == 0){
        for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees // in steps of 1 degree
             myservo.write(pos);              // tell servo to go to position in variable 'pos'
             delay(10);                   }
             
             DoorOpen = 1;
        }
        }
        if (currentLine.endsWith("door/0")) {
          if(DoorOpen == 1){
      digitalWrite(LED, LOW );
       for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }
      Serial.println("DOOR LOOKED");
      DoorOpen = 0;}
        }
        
 
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
  // keypad

  char key = keypad.getKey();

  if (key) {
    Serial.println(key);

    if (key == 'D') {
      if(DoorOpen == 1){
      digitalWrite(LED, LOW );
       for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(10);                       // waits 15ms for the servo to reach the position
  }
      Serial.println("DOOR LOOKED");
      DoorOpen = 0;}
    }
    else if (key == '*') {
      input_password = ""; // reset the input password
      Serial.println("reset the input password");
    } else if (key == '#') {
      if (input_password == password_1 || input_password == password_2 || input_password == password_3) {
        digitalWrite(2, HIGH);  
        Serial.println("Valid Password => unlock the door");
        if(DoorOpen == 0){
        for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees // in steps of 1 degree
             myservo.write(pos);              // tell servo to go to position in variable 'pos'
             delay(10);                   }
             
             DoorOpen = 1;
        }
        
      } else {
        Serial.println("Invalid Password => Try again");
      }

      input_password = ""; // reset the input password
    } else {
      input_password += key; // append new character to input password string
    }
  }

  
}
