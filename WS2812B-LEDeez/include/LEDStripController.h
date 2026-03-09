#pragma once
#include <Arduino.h>
#include <FastLED.h>

#define MAX_LEDS 300
#define LED_PIN 5
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

class LEDController {
private:
    uint16_t numLeds;      // Active LEDs
    uint8_t brightness;
    char currentState;
    CRGB leds[MAX_LEDS];

public:
    LEDController();

    void begin(uint16_t count);
    void setBrightness(uint8_t b);
    void setState(char state, uint16_t num_leds = 0);
    void run();

    // Effects
    void clear();
    void solid(CRGB color);
    void breathe(CRGB color, uint8_t bpm = 10);
    void sineWave(CRGB color, uint8_t speed = 2);
    void dualBreathe(CRGB color1, CRGB color2);
    void twinkle(CRGB color, uint8_t chance = 20, uint8_t fadeAmount = 10);

    CRGB* getLeds();
    uint16_t size() { return numLeds; }
};