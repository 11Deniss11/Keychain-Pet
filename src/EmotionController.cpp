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

#include "EmotionController.h"
#include "eyes.h"

void EmotionController::updateEmotions(bool leftPress, bool rightPress)
{
    // during wake up
    if (eyes.emotion == Eyes::WAKE)
    {
        // forceful wake up
        if ((leftPress || rightPress) && lastClick.time != 0)
        {
            eyes.setEmotion(Eyes::ANGRY);
        }
    }
    checkEmotionTimeout();
    updateClicks(leftPress, rightPress);
}

void EmotionController::checkEmotionTimeout()
{
    switch (eyes.emotion)
    {
    case Eyes::NEUTRAL:
        if (millis() - currentEmotionStartTime > neutralEmotionTime)
        {
            eyes.setEmotion(getRandomNonNeutralEmotion());
        }
        break;
    case Eyes::HAPPY:
        if (millis() - currentEmotionStartTime > happyEmotionTime)
        {
            eyes.setEmotion(Eyes::NEUTRAL);
        }
        break;
    case Eyes::SAD:
        if (millis() - currentEmotionStartTime > sadEmotionTime)
        {
            eyes.setEmotion(Eyes::NEUTRAL);
        }
        break;
    case Eyes::SUS:
        if (millis() - currentEmotionStartTime > susEmotionTime)
        {
            eyes.setEmotion(Eyes::NEUTRAL);
        }
        break;
    case Eyes::ANGRY:
        if (millis() - currentEmotionStartTime > angryEmotionTime)
        {
            eyes.setEmotion(Eyes::NEUTRAL);
        }
        break;
    default:
        break;
    }
}

Eyes::Emotion EmotionController::getRandomNonNeutralEmotion()
{
    int r = esp_random() % 4;
    switch (r)
    {
    case 0:
        return Eyes::HAPPY;
    case 1:
        return Eyes::SAD;
    case 2:
        return Eyes::SUS;
    case 3:
        return Eyes::ANGRY;
    default:
        return Eyes::NEUTRAL; // Fallback, should never happen
    }
}

void EmotionController::updateClicks(bool leftPress, bool rightPress)
{
    if (leftPress || rightPress)
    {
        secondLastClick = lastClick;
        lastClick.time = millis();
        if (leftPress && rightPress)
        {
            lastClick.type = 2;
        }
        else if (leftPress)
        {
            lastClick.type = 0;
        }
        else
        {
            lastClick.type = 1;
        }
    }
}