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

#ifndef EMOTIONCONTROLLER_H
#define EMOTIONCONTROLLER_H

#include <Arduino.h>
#include "eyes.h"

#define neutralEmotionTimeout 15000
#define happyEmotionTimeout 4000
#define sadEmotionTimeout 8000
#define susEmotionTimeout 4000
#define angryEmotionTimeout 6000
#define shortEmotionTimeout 4000

struct Press
{
    unsigned long time;
    uint8_t type; // left = 0, right = 1, both = 2
};

class EmotionController
{
public:
    EmotionController(Eyes &peyes) : eyes(peyes) {};
    void updateEmotions(bool leftPress, bool rightPress, int lightLevel);
    void changeEmotion(Eyes::Emotion newEmotion);
    void calibrateLightLevelThreshold(unsigned int lightLevel);

private:
    unsigned int lightLevelThreshold = 800;

    bool lightSensorCovered = false;
    bool lastLightSensorCovered = false;
    bool sensorJustCovered = false;
    unsigned int averageLightLevel = 0;

    bool lastLeftPressed = false;
    bool leftJustPressed = false;

    bool lastRightPressed = false;
    bool rightJustPressed = false;

    Eyes::Emotion getRandomNonNeutralEmotion();
    void updatePressHistory();
    void checkEmotionTimeout();
    // void changeEmotion(Eyes::Emotion newEmotion);
    void pollInputs(bool leftPress, bool rightPress, unsigned int lightLevel);
    Eyes &eyes;
    Press lastPress = {0, 2};
    Press secondLastPress = {0, 2};
    unsigned long currentEmotionStartTime = 0;
};

#endif // EMOTIONCONTROLLER_H