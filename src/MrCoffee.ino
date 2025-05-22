// This sketch controls a Mr. Coffee coffee machine using an ESP8266 microcontroller.
// It connects to a WiFi network and starts a web server that allows users to control
// the coffee machine through a web interface. The web interface provides buttons to
// turn the machine on/off and to adjust the position of a servo motor, which is
// presumably used to physically interact with the coffee machine's controls.

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "YOURSSID" 
#define STAPSK  "WIFIPASSWORD"
#endif

const char* ssid     = STASSID; // SSID of the WiFi network to connect to.
const char* password = STAPSK; // Password of the WiFi network.

ESP8266WebServer server(80); // Web server object that listens on port 80.

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards


// Current Servo Position
int pos=90; // Current angular position of the servo motor (in degrees). Initialized to 90 degrees.

// HTML content for the web interface.
const String postForms = "<!DOCTYPE html>\
<html>\
<title>Mr. Coffee</title>\
<meta name='viewport' content='width=device-width, initial-scale=1'>\
<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3pro.css'>\
<link rel='stylesheet' href='https://www.w3schools.com/lib/w3-theme-brown.css'>\
<body>\
<header class='w3-top w3-bar w3-theme-d3'>\
  <h2 class='w3-bar-item'>Mr. Coffee</h2>\
</header>\
<div class='w3-container' style='margin-top:90px'>\
<iframe style='display:none' name='hiddenframe'></iframe>\
<div class='w3-cell-row'>\
  <div class='w3-cell'>\
    <a target='hiddenframe' href='/onOff/' class='w3-theme-l3 w3-col s12 w3-button w3-border w3-large w3-padding-small'>ON / OFF</a>\
    <a target='hiddenframe' href='/stepUp/' class='w3-theme-l3 w3-col s12 w3-button w3-border w3-large w3-padding-small'>UP</a>\
    <a target='hiddenframe' href='/stepDown/' class='w3-theme-l3 w3-col s12 w3-button w3-border w3-large w3-padding-small'>DOWN</a>\
  </div>\
</div>\
<footer class='w3-container w3-bottom w3-theme-d3 w3-margin-top'>\
  <h3>by Thomas Heiser</h3>\
</footer>\
</body>\
</html>";

// Handles HTTP requests to the root path ("/").
// Sends the HTML web interface to the client.
void handleRoot() {
  server.send(200, "text/html", postForms);
}

// Handles HTTP requests for paths that are not found.
// Sends a 404 Not Found error message to the client.
void handleNotFound() {
  //digitalWrite(led, 1); // This line is commented out, but could be used for debugging.
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri(); // Get the requested URI.
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST"; // Get the HTTP method.
  message += "\nArguments: ";
  message += server.args(); // Get the number of arguments.
  message += "\n";
  // Loop through all arguments and append them to the message.
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message); // Send the 404 response.
}

// Handles HTTP requests to the "/stepUp/" path.
// Increments the servo position by 1 degree, if it's less than 180 degrees.
// Updates the servo motor and sends an empty response to the client.
void handleStepUp() {
 if(pos<180) // Check if servo position is less than 180 degrees
 {
   pos++; // Increment servo position
 }
 myservo.write(pos); // Set the servo to the new position
 Serial.println(pos); // Print the new position to the serial monitor
 server.send(200, "text/html", ""); // Send an empty HTTP response
}

// Handles HTTP requests to the "/stepDown/" path.
// Decrements the servo position by 1 degree, if it's greater than 0 degrees.
// Updates the servo motor and sends an empty response to the client.
void handleStepDown() {
 if(pos>0) // Check if servo position is greater than 0 degrees
 {
   pos--; // Decrement servo position
 }
 myservo.write(pos); // Set the servo to the new position
 Serial.println(pos); // Print the new position to the serial monitor
 server.send(200, "text/html", ""); // Send an empty HTTP response
}

// Handles HTTP requests to the "/onOff/" path.
// Toggles the servo position between 61 and 90 degrees.
// This is likely used to turn the coffee machine on or off.
// Updates the servo motor and sends an empty response to the client.
void handleOnOff() {
 // This block toggles the servo position.
 // If current position is 61 (assumed "off"), move to 90 (assumed "on").
 // Otherwise, move to 61.
 if(pos==61) 
 {
   pos=90;
 }
 else
 {
   pos=61;
 }
 myservo.write(pos); // Set the servo to the new position

 delay(200); // Wait for 200 milliseconds. This might be for mechanical stability.
 
 // This block seems to repeat the toggle logic. It might be intentional for a specific 
 // mechanical interaction (e.g., a button that needs to be pressed and released),
 // or it could be a bug where the servo moves to a position and then immediately back.
 if(pos==61)
 {
   pos=90;
 }
 else
 {
   pos=61;
 }
 myservo.write(pos); // Set the servo to the new position again
 
 server.send(200, "text/html", ""); // Send an empty HTTP response
}

// Connects the ESP8266 to the WiFi network.
void connectWIFI()
{
  WiFi.begin(ssid, password); // Start WiFi connection with the given SSID and password.
  Serial.println("");

  // Wait for connection
  // This loop continues until the WiFi status is WL_CONNECTED.
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); // Wait 500 milliseconds.
    Serial.print("."); // Print a dot to the serial monitor to indicate progress.
  }
 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid); // Print the SSID of the connected network.
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // Print the IP address assigned to the ESP8266.

  // Start the mDNS responder. 
  // This allows accessing the ESP8266 using "esp8266.local" instead of its IP address.
  if (MDNS.begin("esp8266")) { 
    Serial.println("MDNS responder started");
  }
}

// This function is called once when the ESP8266 boots up.
// It initializes the serial communication, servo motor, WiFi connection, and web server.
void setup(void) {
  
  Serial.begin(115200); // Initialize serial communication at 115200 baud rate.

  myservo.attach(2);  // Attaches the servo motor to GPIO pin 2 (D4 on some ESP8266 boards).

  connectWIFI(); // Connect to the WiFi network.

  // Define the HTTP request handlers for different paths.
  server.on("/", handleRoot); // Handle requests to the root path.
  server.on("/onOff/", handleOnOff); // Handle requests to the "/onOff/" path.
  server.on("/stepUp/", handleStepUp); // Handle requests to the "/stepUp/" path.
  server.on("/stepDown/", handleStepDown); // Handle requests to the "/stepDown/" path.
  server.onNotFound(handleNotFound); // Handle requests to unknown paths.

  server.begin(); // Start the web server.
  Serial.println("HTTP server started");
}

// This function is called repeatedly in a loop after setup() has completed.
// It handles incoming client requests to the web server.
void loop(void) {
  
    server.handleClient(); // Process any incoming client requests.
 
}
