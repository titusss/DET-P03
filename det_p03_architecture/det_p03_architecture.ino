////////////////////////////PINS///////////////////////////////
#define PIN_NEOPIXEL_1        17
#define PIN_NEOPIXEL_2        16

#define PELTIER_PIN_HEATING   22
#define PELTIER_PIN_COOLING   23

#define PUMP_OUT_PIN          -1
#define PUMP_IN_PIN            -1

////////////////////////////DREAMS///////////////////////////////
// LED delays
// LED_STATES = ["fallAsleep", "flashing", "wakeUp", "off"]
const int LED_TIMINGS[4] = {500, 25, 500, 0};
const int LED_RGB[4][3] = {{50, 30, 0}, {50, 50, 50}, {20, 10, 50}, {0, 0, 0}};

// DREAM 1
const int DREAM1_KEYFRAMES[5] = {0, 20, 30, 40, 60};
const int DREAM1_HEARTRATES[5] = {90, 70, 90, 90, 50};
const int DREAM1_RESPRATES[5] = {16, 10, 20, 10, 8};
const int DREAM1_LEDMODES[5] = {0, 3, 1, 3, 2};
const char *DREAM1_AMBIENTSOUND = "dream01.wav";
const char *DREAM1_PELTIERMODES[5] = {"cold", "cold", "warm", "warm", "warm"};

// DREAM 2
const int DREAM2_KEYFRAMES[5] = {0, 20, 30, 40, 60};
const int DREAM2_HEARTRATES[5] = {90, 70, 90, 90, 50};
const int DREAM2_RESPRATES[5] = {16, 10, 20, 10, 8};
const int DREAM2_LEDMODES[5] = {0, 3, 1, 3, 2};
const char *DREAM2_AMBIENTSOUND = "dream01.wav";
const char *DREAM2_PELTIERMODES[5] = {"cold", "cold", "warm", "warm", "warm"};

// DREAM 3
const int DREAM3_KEYFRAMES[5] = {0, 20, 30, 40, 60};
const int DREAM3_HEARTRATES[5] = {90, 70, 90, 90, 50};
const int DREAM3_RESPRATES[5] = {16, 10, 20, 10, 8};
const int DREAM3_LEDMODES[5] = {0, 3, 1, 3, 2};
const char *DREAM3_AMBIENTSOUND = "dream01.wav";
const char *DREAM3_PELTIERMODES[5] = {"cold", "cold", "warm", "warm", "warm"};

////////////////////////////WIFI///////////////////////////////
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
const char *ssid = "DET_Dreamweaver";
const char *password = "password";
WiFiServer server(80);

// SETUP FUNCTION
void wifiSetup() {
  Serial.println("SETUP: WiFi Access Point...");
  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("SUCCESS: WiFi Access Point Setup.");
}
////////////////////////////NEOPIXEL///////////////////////////////
#define NUM_PIXELS 29

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// SETUP FUNCTION
void neopixelSetup() {
  
  Adafruit_NeoPixel pixels1(NUM_PIXELS, PIN_NEOPIXEL_1, NEO_GRB + NEO_KHZ800);
  Adafruit_NeoPixel pixels2(NUM_PIXELS, PIN_NEOPIXEL_2, NEO_GRB + NEO_KHZ800);
  Serial.println("SETUP: Neopixel...");
  pinMode(PIN_NEOPIXEL_1, OUTPUT);
  pinMode(PIN_NEOPIXEL_2, OUTPUT);
  digitalWrite(PIN_NEOPIXEL_1, HIGH);
  digitalWrite(PIN_NEOPIXEL_2, HIGH);
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  pixels1.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels2.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels1.clear(); // Set all pixel colors to 'off'
  pixels2.clear(); // Set all pixel colors to 'off'
  Serial.println("SUCCESS: Neopixel Setup.");
}


void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.setDebugOutput(true);
  Serial.println("Serial connection established successfully.");
  
  wifiSetup();
  neopixelSetup();
}

void loop() {

  // Setup WiFi Server
  
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
            client.print("Click <a href=\"/DREAM1\">here</a> to turn Dream 1.<br>");
            client.print("Click <a href=\"/DREAM2\">here</a> to turn Dream 2.<br>");
            client.print("Click <a href=\"/DREAM3\">here</a> to turn Dream 3.<br>");
            client.print("Click <a href=\"/STOP\">here</a> to turn OFF the pump.<br>");

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

        
        // MAIN LOGIC FOR DECIDING THE SCENARIO
        
        if (currentLine.endsWith("GET /DREAM1")) {
          // manualOverride = true;
          for (int i = 0; i<sizeof(DREAM1_KEYFRAMES); i++) {
            int last_keyframe_time = DREAM1_KEYFRAMES[i - 1];
            if (i==0) {
              last_keyframe_time = 0;
            }
            setDream(*DREAM1_PELTIERMODES[i], DREAM1_RESPRATES[i], DREAM1_LEDMODES[i], DREAM1_KEYFRAMES[i], last_keyframe_time); 
          }
        }
        else if (currentLine.endsWith("GET /DREAM2")) {
          // manualOverride = true;
          for (int i = 0; i<sizeof(DREAM2_KEYFRAMES); i++) {
            int last_keyframe_time = DREAM2_KEYFRAMES[i - 1];
            if (i==0) {
              last_keyframe_time = 0;
            }
            setDream(*DREAM2_PELTIERMODES[i], DREAM2_RESPRATES[i], DREAM2_LEDMODES[i], DREAM2_KEYFRAMES[i], last_keyframe_time); 
          }
        } else if(currentLine.endsWith("GET /DREAM3")) {
          // manualOverride = true;
          for (int i = 0; i<sizeof(DREAM3_KEYFRAMES); i++) {
            int last_keyframe_time = DREAM3_KEYFRAMES[i - 1];
            if (i==0) {
              last_keyframe_time = 0;
            }
            setDream(*DREAM3_PELTIERMODES[i], DREAM3_RESPRATES[i], DREAM3_LEDMODES[i], DREAM3_KEYFRAMES[i], last_keyframe_time);   
          }
        } else if (currentLine.endsWith("GET /STOP")) {
          // manualOverride = false;
          // Not sure if this makes sense. Please edit for pump stopping.
          digitalWrite(PELTIER_PIN_COOLING, LOW);
          digitalWrite(PELTIER_PIN_HEATING, HIGH);
          digitalWrite(PUMP_OUT_PIN, LOW);
          digitalWrite(PUMP_IN_PIN, LOW);
          pixels1.clear();
          pixels2.clear();
          Serial.println("Manual control stopped");
        }
      }
    }
    
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void setDream(char peltierMode, int respRate, int ledMode, int current_keyframe_time, int last_keyframe_time) {
  // Heartrate
  // Play PCR Future Work
  
  // Ambient Sound
  // Play PCR Future Work
  
  // Peltier Heating and Cooling
  if (peltierMode == "cold") {
    digitalWrite(PELTIER_PIN_COOLING, HIGH);
    digitalWrite(PELTIER_PIN_HEATING, LOW);
  } else {
    digitalWrite(PELTIER_PIN_COOLING, LOW);
    digitalWrite(PELTIER_PIN_HEATING, HIGH);
  }

  // PSEUDCODE TO-DO IMPLEMENT TIMER STARTING FROM 0 BELOW
  time = new.time;
  // END OF PSEUDO CODE
  
  int ledDelay = 60; / LED_TIMINGS[led_mode] * 1000;
  int respDelay = 60; / respRate * 1000;

  int ledIndex = 0;
  int respInhaling = true;
  
  // PSEUDO CODE TO-DO IMPLEMENT TIMER CHECK HERE
  while (time.now <= current_keyframe_time - last_keyframe_time * 1000) {
  // END OF PSEUDO CODE
    
    if (int(time.now / 10) % ledDelay == 0) {
      ledIndex = setLED(LED_RGB[led_mode][0], LED_RGB[led_mode][1], LED_RGB[led_mode][2], ledIndex);
    }
    if (int(time.now / 10) % * int(respDelay * / 2) == 0) {
      if (respInhaling == true) {
        digitalWrite(PUMP_IN_PIN, HIGH);
        digitalWrite(PUMP_OUT_PIN, LOW);
      } else {  
        digitalWrite(PUMP_IN_PIN, LOW);
        digitalWrite(PUMP_OUT_PIN, HIGH);
      }
      respInhaling != respInhaling;
    }
  }
}

int setLED(int r, int g, int b, int ledIndex) {
  if (ledIndex == NUM_PIXELS) {
    pixels1.clear();
    pixels2.clear();
    ledIndex = 0;
  }
  pixels1.setPixelColor(ledIndex, pixels.Color(r, g, b));
  pixels2.setPixelColor(ledIndex, pixels2.Color(r, g, b));
  pixels2.show();   // Send the updated pixel colors to the hardware.
  pixels2.show();   // Send the updated pixel colors to the hardware.
  ledIndex++;
  return ledIndex;
}
