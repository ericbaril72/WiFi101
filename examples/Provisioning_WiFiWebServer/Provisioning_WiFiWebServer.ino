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

int status = WL_IDLE_STATUS;

WiFiServer server(80);

// SSID to use in provisioning mode
//  - you must connect to this to configure the WiFi settings for your access point
const char provisionSsid[] = "wifi101-provisioning";
// Hostname to use for the provision mode
//  - once connected to the SSID above, you need to visit http://wifi101.io to configure WiFi settings
const char provisionHost[] = "wifi101.io";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  while (WiFi.status() != WL_CONNECTED) {
    switch (WiFi.status()) {
      case WL_IDLE_STATUS:
      default:
        // Try to connect to a previously provisioned access point
        Serial.println("Attempting to connect with provisioned settings");
        WiFi.begin();
        break;

      case WL_DISCONNECTED:
        // WiFi.begin() failed, enter provision mode
        Serial.println("Failed to connect with provisioned settings!");
        Serial.println();
        Serial.println("Entering provisioning mode");
        WiFi.beginProvision(provisionSsid, provisionHost);

        Serial.print("Please connect to SSID '");
        Serial.print(provisionSsid);
        Serial.print("' and then visit http://");
        Serial.print(provisionHost);
        Serial.println(" to configure WiFi settings");
        break;

      case WL_PROVISIONING:
        // do nothing, we are waiting for someone to provision the settings
        break;
    }
  }

  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
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

