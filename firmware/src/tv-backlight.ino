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
// Light is on or off
bool isOn;

RGBConverter converter = RGBConverter();


// Particle Function to control light
int ctrlLight(String args)
{
    int onoff = args.toInt();
    isOn = (1 == onoff);
    setFromHueAndBrightness(hue, 0);

    return onoff;
}

// Set brightness of LED
int setBrightness(String args)
{
    brightness = args.toInt();
    Particle.publish("set_brightness",
                     "brightness=" + String(brightness),
                     time_to_live);

    if (setFromHueAndBrightness(hue, brightness) == 1)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

// Set Hue
int setFromHueAndBrightness(int _hue, int _brightness)
{
    Particle.publish(
        "set_from_hb",
        "_hue=" + String(_hue) + ", brightness=" + String(_brightness),
        time_to_live);
    double h = (double)_hue / 360.0;
    double s = 1.0;
    double v = (double)_brightness / 100;

    byte rgb[] = {0, 0, 0};

    converter.hsvToRgb(h, s, v, rgb);

    Particle.publish(
        "set_from_hb",
        "rgb=" + String(rgb[0]) + "," + String(rgb[1]) + "," + String(rgb[2]),
        time_to_live);
    setColor(rgb[0], rgb[1], rgb[2]);

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
    return setFromHueAndBrightness(hue, brightness);
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

    Particle.publish("parsed_rgb",
                     "r=" + s(r) + ", g=" + s(g) + ", b=" + s(b),
                     time_to_live);

    if (colorIsValidRange(r) && colorIsValidRange(g) && colorIsValidRange(b))
    {
        double hsv[] = {0.0, 0.0, 0.0};
        //rgb2hsv(r / 255, g / 255, b / 255, hsv);
        converter.rgbToHsv((byte)r, (byte)g, (byte)b, hsv);

        Particle.publish(
            "parsed_hsv",
            "h=" + String(hsv[0]) + ", v=" + String(hsv[2]),
            time_to_live);

        hue = 360 * hsv[0];
        brightness = 100 * hsv[2];
        setFromHueAndBrightness(hue, brightness);
        return 1;
    }
    else
    {
        return -1;
    }
}

void setup()
{

    // setup pin modes
    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);

    // Setup Particle Functions
    Particle.function("state", ctrlLight);
    Particle.function("brightness", setBrightness);
    Particle.function("hue", setHue);

    Particle.function("color", setColorRGB);
    // blink to show that it's ready for use

    Particle.variable("brightness", brightness);
    Particle.variable("hue", hue);
    ready();
}

void loop()
{
}
