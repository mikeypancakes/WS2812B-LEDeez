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
const unsigned long frameInterval = 10; // ~20 FPS

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

void sineWave(CRGB wave_color, uint8_t wave_speed) {
    static uint8_t phase = 0;

    for (int i = 0; i < NUM_LEDS; i++) {
        // Calculate sine wave brightness for each LED
        uint8_t brightness = sin8(phase + (i * 255 / NUM_LEDS));
        leds[i] = wave_color;
        leds[i].fadeToBlackBy(255 - brightness);  // Adjust brightness based on sine wave
    }
    phase += 4;  // Wave speed
    FastLED.show();
    delay(wave_speed);
}

void dualBreathe(CRGB color1_in, CRGB color2_in) {
    uint8_t brightness1 = beatsin8(8);
    uint8_t brightness2 = 255 - brightness1;  // Inverse

    for (int i = 0; i < NUM_LEDS; i++) {
        CRGB color1 = color1_in;
        CRGB color2 = color2_in;

        color1.fadeToBlackBy(255 - brightness1);
        color2.fadeToBlackBy(255 - brightness2);

        leds[i] = color1 + color2;  // Blend both colors
    }
    FastLED.show();
    delay(20);
}

void fireflies(CRGB fireflyColor, uint8_t flash_frequency) {
    fadeToBlackBy(leds, NUM_LEDS, 10);  // Very slow fade
    if (random8() < flash_frequency) {
        leds[random16(NUM_LEDS)] = fireflyColor; 
    }
    FastLED.show();
    delay(10);
}

void travelingPeaks() {
    static uint8_t peak1 = 0;
    static uint8_t peak2 = NUM_LEDS / 2;

    fadeToBlackBy(leds, NUM_LEDS, 20);

    // Draw peaks with falloff
    for (int i = 0; i < NUM_LEDS; i++) {
        int dist1 = abs(i - peak1);
        int dist2 = abs(i - peak2);

        uint8_t brightness = qadd8(
            qsub8(255, dist1 * 20),
            qsub8(255, dist2 * 20)
        );

        leds[i] += CHSV(160, 255, brightness);
    }

    peak1 = (peak1 + 1) % NUM_LEDS;
    peak2 = (peak2 + 1) % NUM_LEDS;
    FastLED.show();
    delay(20);
}

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
    recvWithStartEndMarkers(); // parse serial commands

    if (newData) {
        parseCommand(); // sets command & value
        strip.setState(command, value);
        newData = false;
        delay(10);
    }

    // Non-blocking LED update
    unsigned long now = millis();
    if (now - lastUpdate >= frameInterval) {
        strip.run();
        lastUpdate = now;
    }
}