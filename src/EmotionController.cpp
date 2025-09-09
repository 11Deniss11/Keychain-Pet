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

void EmotionController::updateEmotions(bool leftPress, bool rightPress, int lightLevel)
{
    pollInputs(leftPress, rightPress, lightLevel);

    Serial.println(String(averageLightLevel) + " " + String(lightLevelThreshold) + " " + String(sensorJustCovered));

    if (eyes.game != Eyes::NONE)
    {
        return;
    }

    switch (eyes.emotion)
    {
    case Eyes::WAKE:
        if ((leftJustPressed || rightJustPressed) && lastPress.time != 0)
        {
            // forceful wake up
            changeEmotion(Eyes::ANGRY);
        }
        break;

    case Eyes::NEUTRAL:
        if (sensorJustCovered)
        {
            if (esp_random() % 2 == 1)
            {
                changeEmotion(Eyes::SUS);
            }
            else
            {
                changeEmotion(Eyes::HAPPY);
            }
        }
        break;

    case Eyes::HAPPY:
        if (leftJustPressed || rightJustPressed)
        {
            changeEmotion(Eyes::SUS);
        }
        else if (esp_random() % 1000 == 1)
        {
            changeEmotion(Eyes::HEART);
        }
        break;

    case Eyes::SAD:
        if (sensorJustCovered)
        {
            changeEmotion(Eyes::NEUTRAL);
        }
        break;

    case Eyes::SUS:
        if (leftJustPressed || rightJustPressed)
        {
            changeEmotion(Eyes::ANGRY);
        }
        else if (sensorJustCovered)
        {
            changeEmotion(Eyes::HAPPY);
        }
        break;

    case Eyes::ANGRY:
        if (sensorJustCovered)
        {
            changeEmotion(Eyes::SAD);
        }
        break;

    default:
        break;
    }

    checkEmotionTimeout();
    updatePressHistory();
}

void EmotionController::calibrateLightLevelThreshold(unsigned int lightLevel)
{
    lightLevelThreshold = lightLevel * 0.25;
}

void EmotionController::pollInputs(bool leftPress, bool rightPress, unsigned int lightLevel)
{
    averageLightLevel = (averageLightLevel * 8 + lightLevel * 2) / 10;
    lightSensorCovered = averageLightLevel < lightLevelThreshold;
    sensorJustCovered = lightSensorCovered && !lastLightSensorCovered;
    lastLightSensorCovered = lightSensorCovered;

    leftJustPressed = leftPress && !lastLeftPressed;
    lastLeftPressed = leftPress;

    rightJustPressed = rightPress && !lastRightPressed;
    lastRightPressed = rightPress;
}

void EmotionController::changeEmotion(Eyes::Emotion newEmotion)
{
    if (newEmotion != eyes.emotion)
    {
        eyes.setEmotion(newEmotion);
        currentEmotionStartTime = millis();
    }
}

void EmotionController::checkEmotionTimeout()
{
    switch (eyes.emotion)
    {
    case Eyes::NEUTRAL:
        if (millis() - currentEmotionStartTime > neutralEmotionTimeout)
        {
            changeEmotion(getRandomNonNeutralEmotion());
        }
        break;
    case Eyes::HAPPY:
        if (millis() - currentEmotionStartTime > happyEmotionTimeout)
        {
            changeEmotion(Eyes::NEUTRAL);
        }
        break;
    case Eyes::SAD:
        if (millis() - currentEmotionStartTime > sadEmotionTimeout)
        {
            changeEmotion(Eyes::NEUTRAL);
        }
        break;
    case Eyes::SUS:
        if (millis() - currentEmotionStartTime > susEmotionTimeout)
        {
            changeEmotion(Eyes::HAPPY);
        }
        break;
    case Eyes::ANGRY:
        if (millis() - currentEmotionStartTime > angryEmotionTimeout)
        {
            changeEmotion(Eyes::SAD);
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

void EmotionController::updatePressHistory()
{
    if (leftJustPressed || rightJustPressed)
    {
        secondLastPress = lastPress;
        lastPress.time = millis();

        if (leftJustPressed && rightJustPressed)
        {
            lastPress.type = 2;
        }
        else if (leftJustPressed)
        {
            lastPress.type = 0;
        }
        else
        {
            lastPress.type = 1;
        }
    }
}