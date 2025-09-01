/*
 * This file is part of Keychain-Pet.
 *
 * Keychain-Pet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Keychain-Pet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <Arduino.h>

#include "screen.h"
#include "eyes.h"
#include "EmotionController.h"

// #include <Wire.h>

Screen screen(SSD1306_I2C_ADDRESS);
Eyes eyes(screen);
EmotionController emotionController(eyes);

#define leftButtonPin 3
#define rightButtonPin 2
#define powerPin 0
#define photoPin 1

void setup()
{
    pinMode(leftButtonPin, INPUT_PULLDOWN);
    pinMode(rightButtonPin, INPUT_PULLDOWN);
    pinMode(powerPin, OUTPUT);
    digitalWrite(powerPin, HIGH);
    pinMode(photoPin, INPUT);
    // Serial.begin(115200);
    delay(1000);
    screen.init();
    screen.clearBuffer();
    screen.drawBufferToScreen();
    eyes.setEmotion(Eyes::WAKE);
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
        bool leftButtonPressed = digitalRead(leftButtonPin);
        bool rightButtonPressed = digitalRead(rightButtonPin);

        emotionController.updateEmotions(leftButtonPressed, rightButtonPressed);
        eyes.updateEyes(leftButtonPressed, rightButtonPressed);
    }
    lastTime = time;
}

long int stepSize = 5000;
int lastStep = 0;

void loop()
{
    eyePeriodic();
    // DEMO, NEEDS TO BE REPLACED
    // int step = millis() / stepSize;
    // bool notSame = step != lastStep;
    // if (step != lastStep)
    // {
    //     lastStep = step;
    // }

    // if (notSame)
    // {
    //     if (step == 0)
    //     {
    //         eyes.setEmotion(Eyes::NEUTRAL);
    //     }
    //     else if (step == 1)
    //     {
    //         eyes.setEmotion(Eyes::HAPPY);
    //     }
    //     else if (step == 2)
    //     {
    //         eyes.setEmotion(Eyes::SAD);
    //     }
    //     else if (step == 3)
    //     {
    //         eyes.setEmotion(Eyes::SUS);
    //     }
    //     else if (step == 4)
    //     {
    //         eyes.setEmotion(Eyes::ANGRY);
    //     }
    //     else if (step == 5)
    //     {
    //         eyes.setEmotion(Eyes::NEUTRAL);
    //     }
    // }
}