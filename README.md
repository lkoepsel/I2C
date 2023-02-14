# I2C - Arduino, Pico and I2C

## Introduction
This repository contains the coding examples used to understand the I2C *Wire/Wire1* interfaces on the Pi Pico. 

## Pico Sketches, Recommended for Use:

### Pico/Blink2
Simple blink sketch to test the setup of the Arduino IDE, Pico and computer. This specific sketch blinks two pins using a non-blocking approach.

### Pico/I2C_Scanner
On boot, will scan all 10 of the I2C pairs for I2C devices. It will respond with the *Wire* instance used (*Wire or Wire1*), the SDA/SCL pins as well as the addresses of the devices found, if any.

Use frequently to confirm your hardward matches your software. Trust me it doesn't.

## TempDisplay
An example sketch which uses 4 pin pairs to demonstrate creating multiple *Wire/Wire1* instances. For this example, the temperature sensor uses *Wire* and the display uses *Wire1*, one could easily have both the temp sensor and display on the same I2C (*Wire*) interface, which means you could have up to 10 pairs of temp/sensor displays at once.

## Possible Errors
While attempting to upload code to a Pico, you get this error:
```bash
Error opening serial port '/dev/cu.usbmodem3101'.
```
Be sure to press the Boot/Sel button while plugging in a Pi Pico, or press Reset while holding the Boot/Sel button on the Adafruit Feather. This will put the Pico board in the proper state to accept a new program.
