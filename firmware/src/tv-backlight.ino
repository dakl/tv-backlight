 /*
 * Project tv-backlight
 * Description: Control my TV-backlight over http
 * Author: Daniel Klevebring
 * Date: 2015-10-15
 */

#include "application.h"
#include "RGBConverter.h"

#define R_PIN D0
#define G_PIN D1
#define B_PIN D2

#define PUBLISH_TOPIC   "HKSValues"

int time_to_live = 16777215;

// Brightness
int brightness = 100;
// Current Hue
int hue;
// Curren saturation
int saturation = 100;
// Light is on or off
bool isOn;
// is currently displaying rainbow?
bool isRainbow = false;
int rainbowDelay = 50;

RGBConverter converter = RGBConverter();


// Set Hue
int setFromHSB(int _hue, int _saturation, int _brightness)
{
    double h = (double)_hue / 360.0;
    double s = (double)_saturation / 100.0;
    double v = (double)_brightness / 100.0;

    byte rgb[] = {0, 0, 0};

    converter.hsvToRgb(h, s, v, rgb);

    setColor(rgb[0], rgb[1], rgb[2]);
    hue = _hue;
    saturation = _saturation;
    brightness = _brightness;

    return 1;
}

int setColor(int r, int g, int b)
{
    /* r, g, and b should be ints on the range 0-255 */
    analogWrite(R_PIN, r);
    analogWrite(G_PIN, g);
    analogWrite(B_PIN, b);

    return 1;
}

void setNextRainbowColor()
{
    hue++;
    if (hue >= 360)
    {
        hue = 0;
    }
    setFromHSB(hue, saturation, brightness);
}

bool colorIsValidRange(int col)
{
    if (col > 255 || col < 0)
    {
        return false;
    }
    return true;
}

String s(int num)
{
    return String(num);
}


/**************************************************************
                          Callbacks
**************************************************************/

// Particle Function to control light
int setStateCallback(String args)
{
    isRainbow = false;
    int onoff = args.toInt();
    isOn = (1 == onoff);
    if (isOn)
    {
        hue = 35;
        saturation = 100;
        brightness = 100;
    }
    else
    {
        brightness = 0;
    }
    setFromHSB(hue, saturation, brightness);

    return onoff;
}

int setHueCallback(String args)
{Callback
    isRCallbackainbow = false;
    hue = args.toInt();
    return setFromHSB(hue, saturation, brightness);
}

int setSaturationCallback(String args)
{
    saturation = 100; //args.toInt();
    if (setFromHSB(hue, saturation, brightness) == 1)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

// Set brightness of LED
int setBrightnessCallback(String args)
{
    brightness = args.toInt();
    Particle.publish("set_brightness",
                     "brightness=" + String(brightness),
                     time_to_live);

    if (setFromHSB(hue, saturation, brightness) == 1)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int setRainbowCallback(String args)
{
    rainbowDelay = args.toInt();
    isRainbow = true;
    return 1;
}

int setColorRGBCallback(String args)
{
    isRainbow = false;
    unsigned int r, g, b;
    sscanf(args, "%u,%u,%u", &r, &g, &b);
    setColor(r, g, b);
    return 1;
}

void ready()
{
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

void setup()
{

    // setup pin modes
    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);

    // Setup Particle Functions
    Particle.function("state", setStateCallback);
    Particle.function("hue", setHueCallback);
    Particle.function("saturation", setSaturationCallback);
    Particle.function("brightness", setBrightnessCallback);
    Particle.function("color", setColorRGBCallback);
    Particle.function("rainbow", setRainbowCallback);

    // Set up partivle variables
    Particle.variable("brightness", brightness);
    Particle.variable("hue", hue);

    // blink to show that it's ready for use
    ready();
}

void loop()
{
    if (isRainbow)
    {
        setNextRainbowColor();
        delay(rainbowDelay);
    }
}
