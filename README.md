# tv-backlight

[![Build Status](https://travis-ci.org/dakl/tv-backlight.svg?branch=master)](https://travis-ci.org/dakl/tv-backlight)

Particle-controlled tv-backlight with http-endpoints.

Compilation and deployment (flashing the mcu) is handled by travis-ci. 

# How?

```
# Color from hue (degrees)
curl https://api.particle.io/v1/devices/$TVBACKLIGHT_DEVICE_ID/hue -d access_token=$PARTICLE_ACCESS_TOKEN -d args=0

# color from RGB
curl https://api.particle.io/v1/devices/$TVBACKLIGHT_DEVICE_ID/color -d access_token=$PARTICLE_ACCESS_TOKEN -d args=128,128,128

# 100% brightness
curl https://api.particle.io/v1/devices/$TVBACKLIGHT_DEVICE_ID/brightness -d access_token=$PARTICLE_ACCESS_TOKEN -d args=100

# turn it off
curl https://api.particle.io/v1/devices/$TVBACKLIGHT_DEVICE_ID/state -d access_token=$PARTICLE_ACCESS_TOKEN -d args=0
```
