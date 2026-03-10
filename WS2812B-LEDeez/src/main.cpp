// ICS_LEDs v1.0
// Written by: Mike Slemko
// Date: 16FEB2026
// Description: Controls the F&F underglow.

#include <Arduino.h>
#include "LEDStripController.h"

// LED Configuration
#define LED_PIN     5
uint8_t NUM_LEDS = 150;
CRGB* leds;
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define BAUD_RATE 115200

uint8_t brightness = 50; // Brightness level variable (0-255)
char command;
int value = -1;

boolean newData = false;
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
char payload[numChars] = {0};

char currentState = 'o';

unsigned long lastUpdate = 0;
const unsigned long frameInterval = 1; 

// LED array
//CRGB leds[NUM_LEDS];

CRGB colorList[] = {
  CRGB::Purple,
  CRGB::Green,
  CRGB::Blue,
  CRGB::Red,
  CRGB(255, 255, 100)   // Custom yellowish
};

CRGB errorStateColor = colorList[3];  // Color to indicate an error state
CRGB readyStateColor = colorList[1];  // Color to indicate a ready state
CRGB worklistingColor1 = colorList[1];  // Color to indicate a busy state
CRGB worklistingColor2 = colorList[0]; 
CRGB idleStateColor = colorList[2];  // Color to indicate an idle state

LEDController strip;

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {

        rc = Serial.read();

        if (recvInProgress == true) {

            if (rc != endMarker) {
                receivedChars[ndx++] = rc;

                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0';
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void parseCommand() {

    char *token;

    token = strtok(receivedChars, ":");
    if (token != NULL) {
        command = token[0];
    }

    token = strtok(NULL, ":");
    if (token != NULL) {
        value = atoi(token);
    } else {
        value = -1;  // no parameter supplied
    }
}

void setup() {
  //Serial setdfup
  Serial.begin(BAUD_RATE);
  strip.begin(NUM_LEDS);
  delay(50);
}

void loop() {

    EVERY_N_MILLISECONDS(frameInterval) {
        strip.run();
    }

    recvWithStartEndMarkers(); // parse serial commands

    if (newData) {
        parseCommand(); // sets command & value
        strip.setState(command, value);
        newData = false;
        delay(10);
    }

}