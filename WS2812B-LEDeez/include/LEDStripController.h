#pragma once
#include <Arduino.h>
#include <FastLED.h>
#define MAX_LEDS 300

class LEDController {
private:

    uint16_t numLeds;
    uint8_t brightness;
    char currentState;
    CRGB leds[MAX_LEDS];
public:
    LEDController();
    void begin(uint16_t count);
    void setBrightness(uint8_t b);
    void show();
    void clear();
    void setState(char state);
    void run();
    void solid(CRGB color);
    void breathe(CRGB color, uint8_t bpm = 10);
    void sineWave(CRGB wave_color, uint8_t wave_speed);
    void dualBreathe(CRGB color1_in, CRGB color2_in); 
    void twinkle(CRGB color, uint8_t chance = 20, uint8_t fadeAmount = 10);
    CRGB* getLeds();
    uint16_t size();
};