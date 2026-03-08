#include "LEDStripController.h"

#define LED_PIN     5
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

LEDController::LEDController() {
    numLeds = 0;
    brightness = 200;
    currentState = 'r';
}

void LEDController::begin(uint16_t count) {

    if (count > MAX_LEDS) {
        count = MAX_LEDS;  // Safety clamp
    }

    numLeds = count;

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, numLeds);
    FastLED.setBrightness(brightness);
    FastLED.clear();
    fill_solid(leds, numLeds, CRGB::Yellow);
    FastLED.show();
    Serial.println("LED strip initialized with " + String(numLeds) + " LEDs");
}

void LEDController::setState(char state) {
    currentState = state;
}

void LEDController::run() {

    switch (currentState) {

        case 'o':  // Off
            clear();
            break;

        case 'r':  // Solid
            breathe(CRGB::Green, 20); //breathe(CRGB::Green, bpm = 20);
            show();
            break;

        case 'i':  // Twinkle
            twinkle(CRGB::Blue, 45);
            show();
            break;

        case 'w':  // Dual breathe
            dualBreathe(CRGB::Purple, CRGB::Green);
            show();

        case 'e':  // Sine wave
            sineWave(CRGB::Red, 2);
            show();
            break;

        case 'u':
            if (size() != 10) {
                begin(10);
                Serial.println("LED count: " + String(size()));
            }
            break;
    }
}

void LEDController::setBrightness(uint8_t b) {
    brightness = b;
    FastLED.setBrightness(brightness);
}

void LEDController::show() {
    FastLED.show();
}

void LEDController::clear() {
    FastLED.clear();
    FastLED.show();
}

void LEDController::solid(CRGB color) {
    fill_solid(leds, numLeds, color);
}

void LEDController::twinkle(CRGB color, uint8_t chance, uint8_t fadeAmount) {
    fadeToBlackBy(leds, numLeds, fadeAmount);

    if (random8() < chance) {
        leds[random16(numLeds)] = color;
    }
    delay(25);
}

void LEDController::breathe(CRGB color, uint8_t bpm) {

    for (uint16_t i = 0; i < numLeds; i++) {
        uint8_t brightness = beatsin8(bpm, 50, 255, 0, i * (255 / numLeds));
        leds[i] = color;
        leds[i].fadeToBlackBy(255 - brightness);
    }
}

void LEDController::sineWave(CRGB wave_color, uint8_t wave_speed) {
    static uint8_t phase = 0;

    for (uint16_t i = 0; i < numLeds; i++) {
        uint8_t brightness = sin8(phase + (i * 255 / numLeds));
        leds[i] = wave_color;
        leds[i].fadeToBlackBy(255 - brightness);
    }
    phase += wave_speed;  // Adjust wave speed
}

void LEDController::dualBreathe(CRGB color1_in, CRGB color2_in) {
    uint8_t brightness1 = beatsin8(8);
    uint8_t brightness2 = 255 - brightness1;

    for (uint16_t i = 0; i < numLeds; i++) {
        CRGB color1 = color1_in;
        CRGB color2 = color2_in;

        color1.fadeToBlackBy(255 - brightness1);
        color2.fadeToBlackBy(255 - brightness2);

        leds[i] = color1 + color2;
    }
}

CRGB* LEDController::getLeds() {
    return leds;
}

uint16_t LEDController::size() {
    return numLeds;
}