/*
 * Project tv-backlight
 * Description: Control my TV-backlight over http
 * Author: Daniel Klevebring
 * Date: 2015-10-15
 */

#include "application.h"

#define PUBLISH_TOPIC   "HKSValues"

#define DELAY_LOOP      50

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define R_PIN D0
#define G_PIN D1
#define B_PIN D2

SYSTEM_MODE(AUTOMATIC);

// Brightness
int brightness = 100;
// Current Hue
int hue;
// Light is on or off
bool isOn;

// Forward declarations
int internalSetHue(int hue);

// Particle Function to control light
int ctrlLight(String args){
    int onoff = args.toInt();

    isOn = (1 == onoff);

    internalSetHue(hue);

    return onoff;
}

// Set brightness of LED
int setBrightness(String args){
    brightness = args.toInt();

    internalSetHue(hue);

    return brightness;
}

// Set Hue
int internalSetHue(int hue){
    if(!isOn){
        setColor(0,0,0);
        return 0;
    }

    float h1 = (float)hue / 360.0;
    float h2 = h1 * 255;

    int h = (int)h2;
    int s = 255;
    int v = (brightness * 255) / 100;

    unsigned char r,g,b;

    hsvtorgb(&r, &g, &b, h, s, v);

    setColor(r, g, b);

    return 0;
}

int setColor(int r, int g, int b)
{
    /* r, g, and b should be ints on the range 0-255 */
    analogWrite(R_PIN, r);
    analogWrite(G_PIN, g);
    analogWrite(B_PIN, b);

    return r+g+b;
}

// Set Hue
int setHue(String args){
    hue = args.toInt();

    return internalSetHue(hue);
}

void hsvtorgb(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char h, unsigned char s, unsigned char v)
{
    unsigned char region, fpart, p, q, t;

    if(s == 0) {
        /* color is grayscale */
        *r = *g = *b = v;
        return;
    }

    /* make hue 0-5 */
    region = h / 43;
    /* find remainder part, make it from 0-255 */
    fpart = (h - (region * 43)) * 6;

    /* calculate temp vars, doing integer multiplication */
    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * fpart) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;

    /* assign temp vars based on color cone region */
    switch(region) {
        case 0:
            *r = v; *g = t; *b = p; break;
        case 1:
            *r = q; *g = v; *b = p; break;
        case 2:
            *r = p; *g = v; *b = t; break;
        case 3:
            *r = p; *g = q; *b = v; break;
        case 4:
            *r = t; *g = p; *b = v; break;
        default:
            *r = v; *g = p; *b = q; break;
    }

    return;
}

void ready() {
    // red
    setColor(255, 0, 0);
    delay(100);
    setColor(0, 0, 0);
    delay(200);
    // green
    setColor(0, 255, 0);
    delay(100);
    setColor(0, 0, 0);
    delay(200);
    // blue
    setColor(0, 0, 250);
    delay(100);
    setColor(0, 0, 0);
    delay(200);
}

void setup() {
    
    // setup pin modes
    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);
    
    // Setup Particle Functions
    Particle.function("state", ctrlLight);
    Particle.function("brightness", setBrightness);
    Particle.function("hue", setHue);

    // blink to show that it's ready for use
    ready();
}

void loop() {
    delay(DELAY_LOOP);
}
