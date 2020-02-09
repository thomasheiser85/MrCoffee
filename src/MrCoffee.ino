#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "YOURSSID" 
#define STAPSK  "WIFIPASSWORD"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards


// Current Servo Position
int pos=90;

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

void handleRoot() {
  server.send(200, "text/html", postForms);
}

void handleNotFound() {
  //digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void handleStepUp() {

 if(pos<180)
 {
   pos++;
 }
 myservo.write(pos);
 Serial.println(pos);
 server.send(200, "text/html", "");
}


void handleStepDown() {

 if(pos>0)
 {
   pos--;
 }
 myservo.write(pos);
 Serial.println(pos);
 server.send(200, "text/html", "");
}

void handleOnOff() {

 if(pos==61)
 {
   pos=90;
 }
 else
 {
   pos=61;
 }
 myservo.write(pos);

 delay(200);
 
 if(pos==61)
 {
   pos=90;
 }
 else
 {
   pos=61;
 }
 myservo.write(pos);
 
 server.send(200, "text/html", "");
}


void connectWIFI()
{
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
}

void setup(void) {
  
  Serial.begin(115200);

  myservo.attach(2);  // attaches the servo on GIO2 to the servo object (or D4?)

  connectWIFI();

  server.on("/", handleRoot);

  server.on("/onOff/", handleOnOff);
  
  server.on("/stepUp/", handleStepUp);
  server.on("/stepDown/", handleStepDown);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  
    server.handleClient();
 
}
