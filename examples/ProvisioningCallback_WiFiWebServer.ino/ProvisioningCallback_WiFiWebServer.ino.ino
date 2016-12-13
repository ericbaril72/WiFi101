/*
  WiFi Web Server

  A simple web server that shows the value of the analog input pins.
  using a WiFi shield.

  This example is written to configure the WiFi settings using provisioning mode.

  Circuit:
   WiFi shield attached
   Analog inputs attached to pins A0 through A5 (optional)

  created 13 July 2010
  by dlf (Metodo2 srl)
  modified 31 May 2012
  by Tom Igoe

*/

#include <SPI.h>
#include <WiFi101.h>

const int ledPin = 6; // LED pin for connectivity status indicator

WiFiServer server(80);

// SSID to use in provisioning mode
//  - you must connect to this to configure the WiFi settings for your access point
const char provisionSsid[] = "wifi101-provisioning";
// Hostname to use for the provision mode
//  - once connected to the SSID above, you need to visit http://wifi101.io to configure WiFi settings
const char provisionHost[] = "wifi101.io";

void setup() {
  //Initialize serial:
  Serial.begin(9600);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // configure the LED pin for output mode
  pinMode(ledPin, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.beginOrProvision(provisionSsid, provisionHost, onProvisionEnter, onProvisioning);
  }

  // connected, make the LED stay on
  digitalWrite(ledPin, HIGH);

  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}

void onProvisionEnter() {
  Serial.println("Failed to connect with provisioned settings!");
  Serial.println();
  Serial.println("Now in provisioning mode");

  Serial.print("Please connect to SSID '");
  Serial.print(provisionSsid);
  Serial.print("' and then visit http://");
  Serial.print(provisionHost);
  Serial.println(" to configure WiFi settings");
}

void onProvisioning() {
  // we are waiting for someone to provision the settings, blink the LED
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

