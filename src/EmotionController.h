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

#define wakeTime 15000 // 15 secs

struct click
{
    unsigned long time;
    uint8_t type; // left = 0, right = 1, both = 2
};

class EmotionController
{
public:
    EmotionController(Eyes &peyes) : eyes(peyes) {};
    void updateEmotions(bool leftPress, bool rightPress);

private:
    Eyes::Emotion getRandomNonNeutralEmotion();
    void updateClicks(bool leftPress, bool rightPress);
    void checkEmotionTimeout();
    Eyes &eyes;
    bool asleep = false;
    unsigned long wakeUpTime;
    click lastClick = {0, 0};
    click secondLastClick = {0, 0};
    unsigned long currentEmotionStartTime = 0;
    unsigned long neutralEmotionTime = 10000;
    unsigned long angryEmotionTime = 2000;
};

#endif // EMOTIONCONTROLLER_H