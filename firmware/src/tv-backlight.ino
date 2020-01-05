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

RGBConverter converter = RGBConverter();


// Particle Function to control light
int ctrlLight(String args)
{
    int onoff = args.toInt();
    isOn = (1 == onoff);
    setFromHSB(hue, saturation, 0);

    return onoff;
}

// Set brightness of LED
int setBrightness(String args)
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

int setSaturation(String args)
{
    saturation = args.toInt();
    if (setFromHSB(hue, saturation, brightness) == 1)
    {
        return 1;
    }
    else
    {
        return -1;
    }

}

// Set Hue
int setFromHSB(int _hue, int _saturation, int _brightness)
{
    double h = (double)_hue / 360.0;
    double s = (double)_saturation / 100.0;
    double v = (double)_brightness / 100;

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

// Set Hue
int setHue(String args)
{
    hue = args.toInt();
    return setFromHSB(hue, saturation, brightness);
}

int rainbow(int delay_millis)
{
    while(true)
    {
        hue++;
        if (hue >= 360)
        {
            hue = 0;
        }
        setFromHSB(hue, saturation, brightness);
        delay(delay_millis);
    }
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

int setColorRGB(String args)
{
    unsigned int r, g, b;
    sscanf(args, "%u,%u,%u", &r, &g, &b);
    setColor(r, g, b);
    return 1;
}

void setup()
{

    // setup pin modes
    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);

    // Setup Particle Functions
    Particle.function("state", ctrlLight);
    Particle.function("hue", setHue);
    Particle.function("saturation", setSaturation);
    Particle.function("brightness", setBrightness);
    Particle.function("color", setColorRGB);

    Particle.variable("brightness", brightness);
    Particle.variable("hue", hue);

    // blink to show that it's ready for use
    ready();
}

void loop()
{
}
