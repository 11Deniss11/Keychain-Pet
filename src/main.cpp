#include <Arduino.h>
#include "screen.h"
#include "eyes.h"

#include <Wire.h>

Screen screen(SSD1306_I2C_ADDRESS);
Eyes eyes(screen);

#define leftButtonPin 18
#define rightButtonPin 19

void setup()
{
    pinMode(leftButtonPin, INPUT_PULLUP);
    pinMode(rightButtonPin, INPUT_PULLUP);
    Serial.begin(115200);
    delay(1000);
    screen.init();
    screen.clearBuffer();
    screen.drawBufferToScreen();
    // eyes.setEmotion(Eyes::ANGRY);
    eyes.setGame(Eyes::Pong);
}

// Last time eyes were updated
unsigned long lastTime = 0;

// Update eyes at 30 fps
void eyePeriodic()
{
    unsigned long time = millis();
    time = time % 30;
    if (time < lastTime)
    {
        eyes.updateEyes(digitalRead(leftButtonPin) == HIGH, digitalRead(rightButtonPin) == HIGH);
        // Serial.println("Update");
    }
    lastTime = time;
}

void loop()
{
    eyePeriodic();
}