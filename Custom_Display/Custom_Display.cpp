#include "Custom_Display.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

Custom_Display::Custom_Display(int SCL_pin, int SDA_pin)
{
    _SCL_pin = SCL_pin;
    _SDA_pin = SDA_pin;
}

void Custom_Display::begin()
{
    pinMode(_SCL_pin, OUTPUT);
    pinMode(_SDA_pin, OUTPUT);
    digitalWrite(_SDA_pin, LOW);
    digitalWrite(_SCL_pin, LOW);
    Wire.begin(_SDA_pin, _SCL_pin);
    lcd.init();
    lcd.backlight();
}

void Custom_Display::printMessage(String message)
{
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Bus Traking.");
    lcd.setCursor(0, 1);
    lcd.print(message);
}