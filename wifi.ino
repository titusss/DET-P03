/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

unsigned long startMillis = millis();

// Set these to your desired credentials.
const char *ssid = "Somnus";
const char *password = "password";

WiFiServer server(80);

// Setting pin for inflatables
const int InPin = 25;
const int OutPin = 26;

//Setting pin for Peltier pads
const int heatPin = 14;
const int coolPin = 32;

bool manualOverride = false;
bool firstTime = true;






// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        17 // On Trinket or Gemma, suggest changing this to 1
#define PIN2        16

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 29 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUMPIXELS, PIN2, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 25 // Time (in milliseconds) to pause between pixels

bool startupSequence = true;

void(* resetFunc) (void) = 0;//declare reset function at address 0


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

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
  // Setting the pump pins
  pinMode(InPin, OUTPUT);
  pinMode(OutPin, OUTPUT);
  pinMode(heatPin, OUTPUT);
  pinMode(coolPin, OUTPUT);

  //Setting the LED pins
  pinMode(PIN, OUTPUT);
  pinMode(PIN2, OUTPUT);

  pixels.begin();
  pixels2.begin();
}



bool DREAM1 = false;
bool DREAM2 = false;
bool DREAM3 = false;
bool DREAM4 = false;
bool DREAM5 = false;

int DREAM1_IN = 5000;
int DREAM1_OUT = 3000;

int DREAM2_IN = 3000;
int DREAM2_OUT = 1000;

int DREAM3_IN = 4000;
int DREAM3_OUT = 2000;

bool reset = false;
bool getStarted = false;

void resetDream(){
  DREAM1 = false;
  DREAM2 = false;
  DREAM3 = false;
  DREAM4 = false;
  DREAM5 = false;
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
            client.print("Click <a href=\"/RESET\">here</a> to reset.<br>");

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



        // Check to see what is the client request:

        if (currentLine.endsWith("GET /START")) {
          resetDream();
          getStarted = true;
        }
        
        if (currentLine.endsWith("GET /DREAM1")) {
          resetDream();
          DREAM1 = true;
          firstTime = false;
        }

        if (currentLine.endsWith("GET /DREAM2")) {
          resetDream();
          DREAM2 = true;
          firstTime = false;
        }

        if (currentLine.endsWith("GET /DREAM3")) {
          resetDream();
          DREAM3 = true;
          firstTime = false;
        }

        if (currentLine.endsWith("GET /RESET")) {
          resetDream();
          reset = true;
        }

        if (currentLine.endsWith("GET /RESETALL")) {
          digitalWrite(PIN, LOW);
          digitalWrite(PIN2, LOW);
          digitalWrite(heatPin, LOW);
          digitalWrite(coolPin, LOW);
          pixels.clear(); // Set all pixel colors to 'off'
          pixels2.clear(); // Set all pixel colors to 'off'
          resetFunc(); //call reset 
        }
        
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }

  if (!getStarted){
    digitalWrite(PIN, HIGH);
    digitalWrite(PIN2, HIGH);
    digitalWrite(heatPin, LOW);
    digitalWrite(coolPin, LOW);
    pixels.clear(); // Set all pixel colors to 'off'
    pixels2.clear(); // Set all pixel colors to 'off'

    for(int i=0; i<NUMPIXELS; i++) { // For each pixel..n.

      // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
      // Here we're using a moderately bright green color:
      pixels.setPixelColor(i, pixels.Color(96, 48, 217));
      pixels2.setPixelColor(i, pixels2.Color(96, 48, 217));
    }

    pixels.show();   // Send the updated pixel colors to the hardware.
    pixels2.show();   // Send the updated pixel colors to the hardware.
  }
  
  if (getStarted){
    if (startupSequence){
      digitalWrite(InPin, HIGH);
      digitalWrite(OutPin, LOW);
      delay(25000); //Startup sequence
    
      digitalWrite(InPin, LOW);
      digitalWrite(OutPin, LOW);
      startupSequence = false;
    }
    
    if (!DREAM1 and !DREAM2 and !DREAM3 and !DREAM4 and !DREAM5){
      breathe(5500, 1500);
      //Looping the LED strips
      digitalWrite(PIN, HIGH);
      digitalWrite(PIN2, HIGH);
      pixels.clear(); // Set all pixel colors to 'off'
      pixels2.clear(); // Set all pixel colors to 'off'
      reset = false;
  
      for(int i=0; i<NUMPIXELS; i++) { // For each pixel..n.
  
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        pixels.setPixelColor(i, pixels.Color(int(millis()%255/6), int((255-millis()%255)/6), int((int(millis()%255/2)+100)/6)));
        pixels2.setPixelColor(i, pixels2.Color(int(255-millis()%255/6), int((int(millis()%255/2)+100)/6), int(millis()%255/6)));
      }
  
      pixels.show();   // Send the updated pixel colors to the hardware.
      pixels2.show();   // Send the updated pixel colors to the hardware.
    }
  
    if (reset){
      breathe(5500, 1500);
      //Looping the LED strips
      digitalWrite(PIN, HIGH);
      digitalWrite(PIN2, HIGH);
      pixels.clear(); // Set all pixel colors to 'off'
      pixels2.clear(); // Set all pixel colors to 'off'
      Serial.println("resetted");
      reset = false;
  
      for(int i=0; i<NUMPIXELS; i++) { // For each pixel..n.
  
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        pixels.setPixelColor(i, pixels.Color(int(millis()%255/6), int((255-millis()%255)/6), int((int(millis()%255/2)+100)/6)));
        pixels2.setPixelColor(i, pixels2.Color(int(255-millis()%255/6), int((int(millis()%255/2)+100)/6), int(millis()%255/6)));
      }
  
      pixels.show();   // Send the updated pixel colors to the hardware.
      pixels2.show();   // Send the updated pixel colors to the hardware.
    }
  
    if (DREAM1){
      breathe(DREAM1_IN, DREAM1_OUT);
      //Looping the LED strips
      digitalWrite(PIN, HIGH);
      digitalWrite(PIN2, HIGH);
      digitalWrite(heatPin, HIGH);
      digitalWrite(coolPin, LOW);
      pixels.clear(); // Set all pixel colors to 'off'
      pixels2.clear(); // Set all pixel colors to 'off'
      Serial.println("DREAM1");
  
  
      for(int i=0; i<NUMPIXELS; i++) { // For each pixel..n.
  
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        pixels.setPixelColor(i, pixels.Color(201, 165, 32));
        pixels2.setPixelColor(i, pixels2.Color(201, 165, 32));
      }
  
      pixels.show();   // Send the updated pixel colors to the hardware.
      pixels2.show();   // Send the updated pixel colors to the hardware.
      
    }
  
    if (DREAM2){
      breathe(DREAM2_IN, DREAM2_OUT);
      //Looping the LED strips
      digitalWrite(PIN, HIGH);
      digitalWrite(PIN2, HIGH);
      digitalWrite(heatPin, LOW);
      digitalWrite(coolPin, LOW);
      pixels.clear(); // Set all pixel colors to 'off'
      pixels2.clear(); // Set all pixel colors to 'off'
      Serial.println("DREAM2");
      // The first NeoPixel in a strand is #0, second is 1, all the way up
      // to the count of pixels minus one.
      for(int i=0; i<NUMPIXELS; i++) { // For each pixel..n.
    
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        pixels.setPixelColor(i, pixels.Color(50, 50, 50));
        pixels2.setPixelColor(i, pixels2.Color(50, 50, 50));
    
        pixels.show();   // Send the updated pixel colors to the hardware.
        pixels2.show();   // Send the updated pixel colors to the hardware.
    
        delay(DELAYVAL); // Pause before next pass through loop
      }
    }
  
    if (DREAM3){
      breathe(DREAM3_IN, DREAM3_OUT);
      //Looping the LED strips
      digitalWrite(PIN, HIGH);
      digitalWrite(PIN2, HIGH);
      digitalWrite(heatPin, LOW);
      digitalWrite(coolPin, HIGH);
      pixels.clear(); // Set all pixel colors to 'off'
      pixels2.clear(); // Set all pixel colors to 'off'
      Serial.println("DREAM3");
  
      for(int i=0; i<NUMPIXELS; i++) { // For each pixel..n.
    
        // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
        // Here we're using a moderately bright green color:
        pixels.setPixelColor(i, pixels.Color(32, 154, 201));
        pixels2.setPixelColor(i, pixels2.Color(32, 154, 201));
      }
  
      pixels.show();   // Send the updated pixel colors to the hardware.
      pixels2.show();   // Send the updated pixel colors to the hardware.
    }
  }
}



void breathe(int IN, int OUT){
  if ((millis() - startMillis) % (IN + OUT) < IN) {
    digitalWrite(InPin, HIGH);
    digitalWrite(OutPin, LOW);
  }
  else{
    digitalWrite(InPin, LOW);
    digitalWrite(OutPin, HIGH);
  }
}
