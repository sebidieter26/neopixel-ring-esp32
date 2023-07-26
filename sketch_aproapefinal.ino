#include <WiFi.h>  
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> 
#endif

#define LED_PIN    21
#define LED_ENABLE_PIN 21
#define LED_COUNT 12

Adafruit_NeoPixel ring(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASS";

WiFiServer server(80);

String header;

// Auxiliar variables to store the current output state
String output21State = "off";

// Assign output variables to GPIO pins
const int pinLed = 21;
const int numLed = 12;

int i;

// Funcția care aprinde inelul LED NeoPixel
void turnOnLed() {
  for(i=0; i< LED_COUNT; i++)
  {
    ring.setPixelColor(i, 255, 255, 255);
  }
  ring.show();
}
void turnOffLed(){
  for (i = 0; i < LED_COUNT; i++) 
  {
    ring.setPixelColor(i, 0, 0, 0); 
  }
  ring.show();
}

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, LOW);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  ring.begin();
  ring.show();
  
}

void loop(){



  output21State = digitalRead(pinLed);


  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /21/on") >= 0) 
            {
              Serial.println("RING LED on");
              output21State = "on";
              digitalWrite(pinLed,HIGH);
              turnOnLed();
            }
            else if(header.indexOf("GET /21/off") >= 0) 
            {
              Serial.println("RING LED off");
              output21State = "off";
              digitalWrite(pinLed,LOW);
              turnOffLed();
            } 
            if(header.indexOf("GET /21/alert") >= 0)
              {
                output21State = "on";
                digitalWrite(pinLed,HIGH);
                alert();
              }
            if(header.indexOf("GET /21/rainbow") >= 0)
              { 
                output21State = "on";
                digitalWrite(pinLed,HIGH);
                rainbow(10);
              }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Optima; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 { background-color: #555555;}");
            client.println(".button3{ background-color: white; color: black; border: 2px solid #f44336;}");
            client.println(".button3:hover {background-color: #f44336; color: white;}");
            client.println(".button4 { background-color: white; color: black; border: 2px solid #5d00ba;}");
            client.println(".button4:hover {background-color: #5d00ba; color: white;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ACTIONARE RING LED <b>NEOPIXEL</b></h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 21  
            client.println("<p>RING LED-UL ESTE " + output21State + "</p>");
            // If the output21State is off, it displays the ON button       
            if (output21State=="off") {
              client.println("<p><a href=\"/21/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/21/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            if (output21State == "on")
            {
            client.println("<p><a href=\"/21/alert\"><button class =\"button button3\">ALERT</button></a><a href=\"/21/rainbow\"><button class =\"button button4\">Rainbow</button></a></p>");
            }
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
void alert() {
  for (int j = 0; j < 10; j++) {  
    for (int i = 0; i < LED_COUNT; i++) {
      ring.setPixelColor(i, ring.Color(255, 0, 0));  
    }
    ring.show();  
    delay(500); 

    for (int i = 0; i < LED_COUNT; i++) {
      ring.setPixelColor(i, ring.Color(0, 0, 0));  
    }
    ring.show();  
    delay(500);  
  }
}
void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<ring.numPixels(); i++) { 

      int pixelHue = firstPixelHue + (i * 65536L / ring.numPixels());
  
      ring.setPixelColor(i, ring.gamma32(ring.ColorHSV(pixelHue)));
    }
    ring.show(); 
    delay(wait);
  }
}