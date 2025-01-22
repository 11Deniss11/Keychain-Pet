#include <Arduino.h>
#include "screen.h"
#include "eyes.h"

#include <Wire.h>

Screen screen(SSD1306_I2C_ADDRESS);
Eyes eyes(screen);

#define leftButtonPin 18
#define rightButtonPin 19
#define powerPin 15

void setup()
{
    pinMode(leftButtonPin, INPUT_PULLDOWN);
    pinMode(rightButtonPin, INPUT_PULLDOWN);
    pinMode(powerPin, OUTPUT);
    digitalWrite(powerPin, HIGH);
    Serial.begin(115200);
    delay(1000);
    screen.init();
    screen.clearBuffer();
    screen.drawBufferToScreen();
    eyes.setEmotion(Eyes::NEUTRAL);
    // eyes.setGame(Eyes::DINO);
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
        eyes.updateEyes(digitalRead(leftButtonPin), digitalRead(rightButtonPin));
        // Serial.println("Update");
    }
    lastTime = time;
}

long int stepSize = 5000;
int lastStep = 0;

void loop()
{
    eyePeriodic();
    // DEMO, NEEDS TO BE REPLACED
    int step = millis() / stepSize;
    bool notSame = step != lastStep;
    if (step != lastStep)
    {
        lastStep = step;
    }

    if (notSame)
    {
        if (step == 0)
        {
            eyes.setEmotion(Eyes::NEUTRAL);
        }
        else if (step == 1)
        {
            eyes.setEmotion(Eyes::HAPPY);
        }
        else if (step == 2)
        {
            eyes.setEmotion(Eyes::SAD);
        }
        else if (step == 3)
        {
            eyes.setEmotion(Eyes::SUS);
        }
        else if (step == 4)
        {
            eyes.setEmotion(Eyes::ANGRY);
        }
        else if (step == 5)
        {
            eyes.setEmotion(Eyes::NEUTRAL);
        }
    }
}