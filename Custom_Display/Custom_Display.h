#ifndef Custom_Display_h
#define Custom_Display_h

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class Custom_Display
{
private:
    int _SCL_pin;
    int _SDA_pin;

public:
    Custom_Display(int SCL_pin, int SDA_pin);
    void begin();
    void printMessage(String message);
};

#endif