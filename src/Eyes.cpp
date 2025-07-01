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
#include "eyes.h"
#include "screen.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// General update function at 30 fps
void Eyes::updateEyes(bool leftPress, bool rightPress)
{
    bool posAchieved;
    // Don't update during cooldown
    if (millis() > cooldownTill)
    {
        posAchieved = moveEyes();
    }

    if (game != NONE)
    {
        if (leftPress && rightPress)
        {
            if (!buttonHold)
            {
                buttonHoldEndTime = millis() + 3000;
            }

            buttonHold = true;
            if (millis() > buttonHoldEndTime - 2000)
            {
                Vector2 progressBarRectangle[4] = {Vector2(5, 5), Vector2(SCREEN_WIDTH / 3 - 5, 5), Vector2(SCREEN_WIDTH / 3 - 5, 10), Vector2(5, 10)};
                float progressBarRadiuses[4] = {1, 1, 1, 1};
                screen.drawRectangleInBuffer(progressBarRectangle, progressBarRadiuses);
            }
            if (millis() > buttonHoldEndTime - 1000)
            {
                Vector2 progressBarRectangle[4] = {Vector2((SCREEN_WIDTH / 3) + 5, 5), Vector2((SCREEN_WIDTH / 3) * 2 - 5, 5), Vector2((SCREEN_WIDTH / 3) * 2 - 5, 10), Vector2((SCREEN_WIDTH / 3) + 5, 10)};
                float progressBarRadiuses[4] = {1, 1, 1, 1};
                screen.drawRectangleInBuffer(progressBarRectangle, progressBarRadiuses);
            }
            if (millis() > buttonHoldEndTime)
            {
                Vector2 progressBarRectangle[4] = {Vector2((SCREEN_WIDTH / 3) * 2 + 5, 5), Vector2(SCREEN_WIDTH - 5, 5), Vector2(SCREEN_WIDTH - 5, 10), Vector2((SCREEN_WIDTH / 3) * 2 + 5, 10)};
                float progressBarRadiuses[4] = {1, 1, 1, 1};
                screen.drawRectangleInBuffer(progressBarRectangle, progressBarRadiuses);
                buttonHold = false;
                setGame(NONE);
                setEmotion(NEUTRAL);
            }
        }
        else
        {
            buttonHold = false;
        }
    }

    if (game == NONE || millis() < pongPlayingAnimEndTime)
    {
        // If both posiions and radiuses are achieved, update sub emotion
        if (posAchieved)
        {
            updateSubEmotion();
        }
    }
    else
    {
        switch (game)
        {
        case PONG:
            updatePong(leftPress, rightPress, posAchieved);
            break;
        case DINO:
            updateDino(leftPress, rightPress, posAchieved);
            break;
        }
    }

    // Convert eye positions to int for drawing
    Vector2 eyePositionsInt[8] = {vector2fToVector2(eyePositions), vector2fToVector2(eyePositions + 1),
                                  vector2fToVector2(eyePositions + 2), vector2fToVector2(eyePositions + 3),
                                  vector2fToVector2(eyePositions + 4), vector2fToVector2(eyePositions + 5),
                                  vector2fToVector2(eyePositions + 6), vector2fToVector2(eyePositions + 7)};
    // Left eye
    screen.drawRectangleInBuffer(eyePositionsInt, eyeRadiuses);
    // Right eye
    screen.drawRectangleInBuffer(eyePositionsInt + 4, eyeRadiuses + 4);

    if (game != NONE && millis() > pongPlayingAnimEndTime)
    {
        // Convert game element positions to int for drawing
        Vector2 gameElementPositionInt[4] = {vector2fToVector2(gameElementPositionF), vector2fToVector2(gameElementPositionF + 1),
                                             vector2fToVector2(gameElementPositionF + 2), vector2fToVector2(gameElementPositionF + 3)};
        // Game Element
        screen.drawRectangleInBuffer(gameElementPositionInt, gameElementRadiuses);
    }

    screen.drawBufferToScreen();
    screen.clearBuffer();
}

void Eyes::resetBall()
{
    pongBallPosition = Vector2f(36, 20);
    pongBallVelocity = screen.normalize(
        Vector2f((random(0, 1) ? random(-100, -20) : random(20, 100)) / 100.0, random(-85, 85) / 100.0));
    pongBallVelocity = Vector2f(pongBallVelocity.x * 2, pongBallVelocity.y * 2);
}

void Eyes::updatePong(bool leftPress, bool rightPress, bool positionsAchieved)
{
    switch (gameState)
    {
    case 0:
        // Serial.println("State 1 Update");

        gameElementPositionF[0] = eyePositions[0];
        gameElementPositionF[1] = eyePositions[1];
        gameElementPositionF[2] = eyePositions[2];
        gameElementPositionF[3] = eyePositions[3];

        // resetBall();

        speed = 0.1;
        gameElementSpeed = 0.1;

        gameState = 1;
        break;
    case 1:
        resetBall();
        if (positionsAchieved)
        {
            speed = 0.8;
            gameElementSpeed = 1;
            gameState = 2;
        }
        break;
    case 2:
        if (leftPress && !rightPress)
        {
            paddle1Position.y -= 2;
            paddle1Position.y = constrain(paddle1Position.y, 7, 33);
        }
        if (rightPress && !leftPress)
        {
            paddle1Position.y += 2;
            paddle1Position.y = constrain(paddle1Position.y, 7, 33);
        }

        if (pongBallPosition.x > 36)
        {
            if (pongBallPosition.y > paddle2Position.y)
            {
                paddle2Position.y += 1.1;
                paddle2Position.y = constrain(paddle2Position.y, 7, 33);
            }
            else
            {
                paddle2Position.y -= 1.1;
                paddle2Position.y = constrain(paddle2Position.y, 7, 33);
            }
        }

        pongBallPosition.x += pongBallVelocity.x;
        pongBallPosition.y += pongBallVelocity.y;
        // score by player 2
        if (pongBallPosition.x < 5)
        {
            gameScore2++;
            Eyes::resetBall();
            gameState = 0;
            pongPlayingAnimEndTime = millis() + 5000;
            setEmotion(HAPPY);
        }
        // hits paddle 1
        else if (pongBallPosition.x < 11 && pongBallPosition.y > paddle1Position.y - 8 && pongBallPosition.y < paddle1Position.y + 8)
        {
            pongBallVelocity.x = -pongBallVelocity.x;
            pongBallPosition.x = 11;
            pongBallVelocity = Vector2f(pongBallVelocity.x * 1.05, pongBallVelocity.y * 1.05);
        }
        // score by player 1
        if (pongBallPosition.x > 66)
        {
            gameScore1++;
            Eyes::resetBall();

            gameState = 0;
            pongPlayingAnimEndTime = millis() + 5000;
            random(0, 2) == 1 ? setEmotion(SAD) : setEmotion(ANGRY);
        }
        // hits paddle 2
        else if (pongBallPosition.x > 60 && pongBallPosition.y > paddle2Position.y - 8 && pongBallPosition.y < paddle2Position.y + 8)
        {
            pongBallVelocity.x = -pongBallVelocity.x;
            pongBallPosition.x = 60;
            pongBallVelocity = Vector2f(pongBallVelocity.x * 1.05, pongBallVelocity.y * 1.05);
        }
        // hits top or bottom
        if (pongBallPosition.y < 5 || pongBallPosition.y > 35)
        {
            pongBallVelocity.y = -pongBallVelocity.y;
            // pongBallVelocity.x = constrain(pongBallPosition.y, 5, 35);
            pongBallVelocity = Vector2f(pongBallVelocity.x * 1.05, pongBallVelocity.y * 1.05);
        }
        break;
    }

    paddlePositions[0] = Vector2(paddle1Position.x - 3, paddle1Position.y - 7);
    paddlePositions[1] = Vector2(paddle1Position.x + 3, paddle1Position.y - 7);
    paddlePositions[2] = Vector2(paddle1Position.x + 3, paddle1Position.y + 7);
    paddlePositions[3] = Vector2(paddle1Position.x - 3, paddle1Position.y + 7);

    paddlePositions[4] = Vector2(paddle2Position.x - 3, paddle2Position.y - 7);
    paddlePositions[5] = Vector2(paddle2Position.x + 3, paddle2Position.y - 7);
    paddlePositions[6] = Vector2(paddle2Position.x + 3, paddle2Position.y + 7);
    paddlePositions[7] = Vector2(paddle2Position.x - 3, paddle2Position.y + 7);

    for (int i = 0; i < 8; i++)
    {
        paddleRadiuses[i] = 1;
    }

    wantedPosition = paddlePositions;
    wantedRadiuses = paddleRadiuses;

    fakeGameElementPosition[0] = Vector2(pongBallPosition.x - 4, pongBallPosition.y - 4);
    fakeGameElementPosition[1] = Vector2(pongBallPosition.x + 4, pongBallPosition.y - 4);
    fakeGameElementPosition[2] = Vector2(pongBallPosition.x + 4, pongBallPosition.y + 4);
    fakeGameElementPosition[3] = Vector2(pongBallPosition.x - 4, pongBallPosition.y + 4);

    for (int i = 0; i < 4; i++)
    {
        fakeGameElementRadiuses[i] = 1;
    }

    wantedGameElementPosition = fakeGameElementPosition;
    wantedGameElementRadiuses = fakeGameElementRadiuses;
}

void Eyes::updateDino(bool leftPress, bool rightPress, bool positionsAchieved)
{
    switch (gameState)
    {
    case 0:
        // Serial.println("State 1 Update");

        gameElementPositionF[0] = eyePositions[0];
        gameElementPositionF[1] = eyePositions[1];
        gameElementPositionF[2] = eyePositions[2];
        gameElementPositionF[3] = eyePositions[3];
        cactusPositionX = 75;

        dinoSpeed = 1.5;

        speed = 0.1;
        gameElementSpeed = 0.1;

        gameState = 1;
        break;
    case 1:
        resetBall();
        if (positionsAchieved)
        {
            speed = 0.5;
            gameElementSpeed = 1;
            gameState = 2;
        }
        break;
    case 2:
        if (dinoVertPosition == dinoFloor)
        {
            if (leftPress && !rightPress)
            {
                dinoVertVelocity = -dinoJumpSpeed;
            }
        }
        else if (dinoVertPosition < dinoFloor + 1)
        {
            dinoVertVelocity += dinoGravity;
        }
        else if (dinoVertPosition > dinoFloor)
        {
            dinoVertPosition = dinoFloor;
            if (dinoVertVelocity > 0)
            {
                dinoVertVelocity = 0;
            }
        }

        if (rightPress && !leftPress)
        {
            dinoCrouched = true;
        }
        else
        {
            dinoCrouched = false;
        }

        dinoVertPosition += dinoVertVelocity;

        dinoSpeed += 0.001;

        if (!dinoCrouched && cactusPositionX - cactusHalfWidth < dinoStandPosition[2].x && cactusPositionX + cactusHalfWidth > dinoStandPosition[1].x && cactusFloor - cactusHeight < dinoStandPosition[3].y + dinoVertPosition && cactusFloor > dinoStandPosition[1].y + dinoVertPosition)
        {
            gameState = 0;
            pongPlayingAnimEndTime = millis() + 5000;
            setEmotion(SAD);
        }

        if (cactusPositionX + cactusHalfWidth < 0)
        {
            if (dinoVertPosition == dinoFloor && esp_random() % 100 < 3)
            {
                cactusPositionX = 80;
                if (dinoSpeed > 2 && esp_random() % 7 < 3)
                {
                    cactusIsBird = true;
                    cactusHalfWidth = 3;
                    cactusHeight = 4;
                    cactusFloor = -(esp_random() % 2 * 6) + dinoFloor - 8;
                }
                else
                {
                    cactusIsBird = false;
                    cactusHalfWidth = 2 + esp_random() % 3;
                    cactusHeight = 6 + esp_random() % 3;
                    cactusFloor = dinoFloor;
                }
            }
        }
        else
        {
            cactusPositionX -= dinoSpeed;
        }

        break;
    }

    if (!dinoCrouched)
    {
        dinoPositions[0] = Vector2(dinoStandPosition[0].x, dinoStandPosition[0].y + dinoVertPosition);
        dinoPositions[1] = Vector2(dinoStandPosition[1].x, dinoStandPosition[1].y + dinoVertPosition);
        dinoPositions[2] = Vector2(dinoStandPosition[2].x, dinoStandPosition[2].y + dinoVertPosition);
        dinoPositions[3] = Vector2(dinoStandPosition[3].x, dinoStandPosition[3].y + dinoVertPosition);
    }
    else
    {
        dinoPositions[0] = Vector2(dinoCrouchPosition[0].x, dinoCrouchPosition[0].y + dinoVertPosition);
        dinoPositions[1] = Vector2(dinoCrouchPosition[1].x, dinoCrouchPosition[1].y + dinoVertPosition);
        dinoPositions[2] = Vector2(dinoCrouchPosition[2].x, dinoCrouchPosition[2].y + dinoVertPosition);
        dinoPositions[3] = Vector2(dinoCrouchPosition[3].x, dinoCrouchPosition[3].y + dinoVertPosition);
    }

    for (int i = 0; i < 8; i++)
    {
        dinoRadiuses[i] = 1;
    }

    wantedPosition = dinoPositions;
    wantedRadiuses = dinoRadiuses;

    fakeGameElementPosition[0] = Vector2(max(cactusPositionX - cactusHalfWidth, 0), cactusFloor - cactusHeight);
    fakeGameElementPosition[1] = Vector2(max(cactusPositionX + cactusHalfWidth, 0), cactusFloor - cactusHeight);
    fakeGameElementPosition[2] = Vector2(max(cactusPositionX + cactusHalfWidth, 0), cactusFloor);
    fakeGameElementPosition[3] = Vector2(max(cactusPositionX - cactusHalfWidth, 0), cactusFloor);

    for (int i = 0; i < 4; i++)
    {
        fakeGameElementRadiuses[i] = 1;
    }

    wantedGameElementPosition = fakeGameElementPosition;
    wantedGameElementRadiuses = fakeGameElementRadiuses;

    for (int i = 0; i < 15; i++)
    {
        dinoStars[i] = Vector2f(dinoStars[i].x - dinoSpeed / 4, dinoStars[i].y);
        if (dinoStars[i].x < 0)
        {
            dinoStars[i] = Vector2f(72, esp_random() % dinoFloor);
        }
        screen.setPixel(dinoStars[i].x, dinoStars[i].y, true);
    }
}

void Eyes::setGame(Game game)
{
    this->game = game;
    gameState = 0;
}

void Eyes::updateSubEmotion()
{

    if (!emotionAchieved)
    {
        emotionAchieved = true;
        subEmotion = 0;
        updateCooldown();
        cycleSubEmotion();
    }
    else
    {
        updateCooldown();
        subEmotion++;
        cycleSubEmotion();
    }
}

// Move eyes to wanted position
// Returns true if positions and radiuses are achieved
bool Eyes::moveEyes()
{
    // Update eye positions
    for (int i = 0; i < 8; i++)
    {
        eyePositions[i] = Vector2f(lerp(eyePositions[i].x, wantedPosition[i].x, speed),
                                   lerp(eyePositions[i].y, wantedPosition[i].y, speed));
        int diffX = abs(wantedPosition[i].x - eyePositions[i].x);
        int diffY = abs(wantedPosition[i].y - eyePositions[i].y);
        if (diffX <= positionTolerance)
        {
            eyePositions[i].x = wantedPosition[i].x;
        }
        if (diffY <= positionTolerance)
        {
            eyePositions[i].y = wantedPosition[i].y;
        }
    }
    // Update eye radiuses
    for (int i = 0; i < 8; i++)
    {
        eyeRadiuses[i] = lerp(eyeRadiuses[i], wantedRadiuses[i], speed);

        int diff = abs(wantedRadiuses[i] - eyeRadiuses[i]);
        if (diff <= positionTolerance)
        {
            eyeRadiuses[i] = wantedRadiuses[i];
        }
    }

    // Check if positions are achieved
    bool positionsAchieved = true;
    for (int i = 0; i < 8; i++)
    {
        if (eyePositions[i].x != wantedPosition[i].x || eyePositions[i].y != wantedPosition[i].y)
        {
            positionsAchieved = false;
            break;
        }
    }

    // Check if radiuses are achieved
    bool radiusesAchieved = true;
    for (int i = 0; i < 8; i++)
    {
        if (eyeRadiuses[i] != wantedRadiuses[i])
        {
            radiusesAchieved = false;
            break;
        }
    }

    if (game == NONE)
    {
        return positionsAchieved && radiusesAchieved;
    }
    else
    {
        // Update game element position
        for (int i = 0; i < 4; i++)
        {
            gameElementPositionF[i] = Vector2f(lerp(gameElementPositionF[i].x, wantedGameElementPosition[i].x, gameElementSpeed),
                                               lerp(gameElementPositionF[i].y, wantedGameElementPosition[i].y, gameElementSpeed));
            int diffX = abs(wantedGameElementPosition[i].x - gameElementPositionF[i].x);
            int diffY = abs(wantedGameElementPosition[i].y - gameElementPositionF[i].y);
            if (diffX <= positionTolerance)
            {
                gameElementPositionF[i].x = wantedGameElementPosition[i].x;
            }
            if (diffY <= positionTolerance)
            {
                gameElementPositionF[i].y = wantedGameElementPosition[i].y;
            }
        }
        // Update game element radiuses
        for (int i = 0; i < 4; i++)
        {
            gameElementRadiuses[i] = lerp(gameElementRadiuses[i], wantedGameElementRadiuses[i], gameElementSpeed);

            int diff = abs(wantedGameElementRadiuses[i] - gameElementRadiuses[i]);
            if (diff <= positionTolerance)
            {
                gameElementRadiuses[i] = wantedGameElementRadiuses[i];
            }
        }

        // Check if positions are achieved
        bool gameElementPositionsAchieved = true;
        for (int i = 0; i < 4; i++)
        {
            if (gameElementPositionF[i].x != wantedGameElementPosition[i].x || gameElementPositionF[i].y != wantedGameElementPosition[i].y)
            {
                gameElementPositionsAchieved = false;
                break;
            }
        }

        // Check if radiuses are achieved
        bool gameElementRadiusesAchieved = true;
        for (int i = 0; i < 4; i++)
        {
            if (gameElementRadiuses[i] != wantedGameElementRadiuses[i])
            {
                gameElementRadiusesAchieved = false;
                break;
            }
        }

        return positionsAchieved && radiusesAchieved && gameElementPositionsAchieved && gameElementRadiusesAchieved; // && gameElementPositionsAchieved && gameElementRadiusesAchieved;
    }
}

// Convert Vector2f to Vector2
Vector2 Eyes::vector2fToVector2(Vector2f *arr)
{
    Vector2 vec = Vector2(arr->x, arr->y);
    return vec;
}

// Set wanted position/radiuses for current sub emotion
void Eyes::cycleSubEmotion()
{
    switch (emotion)
    {
    case NEUTRAL:
        switch (subEmotion)
        {
        case 0:
            wantedPosition = neutralPosition0;
            wantedRadiuses = neutralRadiuses0;
            speed = neutralSpeed0;
            break;
        case 1:
            wantedPosition = neutralPosition1;
            wantedRadiuses = neutralRadiuses1;
            speed = neutralSpeed1;
            break;
        case 2:
            wantedPosition = neutralPosition2;
            wantedRadiuses = neutralRadiuses2;
            speed = neutralSpeed2;
            break;
        case 3:
            wantedPosition = neutralPosition3;
            wantedRadiuses = neutralRadiuses3;
            speed = neutralSpeed3;
            break;
        case 4:
            wantedPosition = neutralPosition4;
            wantedRadiuses = neutralRadiuses4;
            speed = neutralSpeed4;
            break;
        default:
            subEmotion = 0;
            wantedPosition = neutralPosition0;
            wantedRadiuses = neutralRadiuses0;
            speed = neutralSpeed0;
            break;
        }
        break;
    case HAPPY:
        switch (subEmotion)
        {
        case 0:
            wantedPosition = happyPosition0;
            wantedRadiuses = happyRadiuses0;
            speed = happySpeed0;
            break;
        case 1:
            wantedPosition = happyPosition1;
            wantedRadiuses = happyRadiuses1;
            speed = happySpeed1;
            break;
        case 2:
            wantedPosition = happyPosition2;
            wantedRadiuses = happyRadiuses2;
            speed = happySpeed2;
            break;
        case 3:
            wantedPosition = happyPosition3;
            wantedRadiuses = happyRadiuses3;
            speed = happySpeed3;
            break;
        case 4:
            wantedPosition = happyPosition2;
            wantedRadiuses = happyRadiuses2;
            speed = happySpeed2;
            break;
        case 5:
            wantedPosition = happyPosition1;
            wantedRadiuses = happyRadiuses1;
            speed = happySpeed1;
            break;
        default:
            subEmotion = 0;
            wantedPosition = happyPosition0;
            wantedRadiuses = happyRadiuses0;
            speed = happySpeed0;
            break;
        }
        break;
    case SAD:
        switch (subEmotion)
        {
        case 0:
            wantedPosition = sadPosition0;
            wantedRadiuses = sadRadiuses0;
            speed = sadSpeed0;
            break;
        case 1:
            wantedPosition = sadPosition1;
            wantedRadiuses = sadRadiuses1;
            speed = sadSpeed1;
            break;
        case 2:
            wantedPosition = sadPosition0;
            wantedRadiuses = sadRadiuses0;
            speed = sadSpeed0;
            break;
        case 3:
            wantedPosition = sadPosition3;
            wantedRadiuses = sadRadiuses3;
            speed = sadSpeed3;
            break;
        case 4:
            wantedPosition = sadPosition0;
            wantedRadiuses = sadRadiuses0;
            speed = sadSpeed0;
            break;
        case 5:
            wantedPosition = sadPosition5;
            wantedRadiuses = sadRadiuses5;
            speed = sadSpeed5;
            break;
        case 6:
            wantedPosition = sadPosition0;
            wantedRadiuses = sadRadiuses0;
            speed = sadSpeed0;
            break;
        case 7:
            wantedPosition = sadPosition3;
            wantedRadiuses = sadRadiuses3;
            speed = sadSpeed3;
            break;
        case 8:
            subEmotion = 0;
            wantedPosition = sadPosition0;
            wantedRadiuses = sadRadiuses0;
            speed = sadSpeed0;
            break;
        default:
            break;
        }
        break;
    case SUS:
        switch (subEmotion)
        {
        case 0:
            wantedPosition = susPosition0;
            wantedRadiuses = susRadiuses0;
            speed = susSpeed0;
            break;
        case 1:
            wantedPosition = susPosition1;
            wantedRadiuses = susRadiuses1;
            speed = susSpeed1;
            break;
        case 2:
            wantedPosition = susPosition2;
            wantedRadiuses = susRadiuses2;
            speed = susSpeed2;
            break;
        case 3:
            subEmotion = 0;
            wantedPosition = susPosition0;
            wantedRadiuses = susRadiuses0;
            speed = susSpeed0;
            break;
        default:
            break;
        }
        break;
    case ANGRY:
        switch (subEmotion)
        {
        case 0:
            wantedPosition = angryPosition0;
            wantedRadiuses = angryRadiuses0;
            speed = angrySpeed0;
            break;
        case 1:
            wantedPosition = angryPosition1;
            wantedRadiuses = angryRadiuses1;
            speed = angrySpeed0;
            break;
        case 2:
            wantedPosition = angryPosition2;
            wantedRadiuses = angryRadiuses2;
            speed = angrySpeed0;
            break;
        case 3:
            wantedPosition = angryPosition1;
            wantedRadiuses = angryRadiuses1;
            speed = angrySpeed1;
            break;
        case 4:
            wantedPosition = angryPosition2;
            wantedRadiuses = angryRadiuses2;
            speed = angrySpeed2;
            break;
        case 5:
            wantedPosition = angryPosition1;
            wantedRadiuses = angryRadiuses1;
            speed = angrySpeed1;
            break;
        case 6:
            wantedPosition = angryPosition2;
            wantedRadiuses = angryRadiuses2;
            speed = angrySpeed2;
            break;
        case 7:
            subEmotion = 0;
            wantedPosition = angryPosition7;
            wantedRadiuses = angryRadiuses7;
            speed = angrySpeed7;
            break;
        }
        break;
    case WAKE:
        switch (subEmotion)
        {
        case 0:
            wantedPosition = wakePosition0;
            wantedRadiuses = wakeRadiuses0;
            speed = wakeSpeed0;
            break;
        case 1:
            wantedPosition = wakePosition1;
            wantedRadiuses = wakeRadiuses1;
            speed = wakeSpeed1;
            break;
        case 2:
            wantedPosition = wakePosition0;
            wantedRadiuses = wakeRadiuses0;
            speed = wakeSpeed2;
            break;
        case 3:
            wantedPosition = wakePosition3;
            wantedRadiuses = wakeRadiuses3;
            speed = wakeSpeed3;
            break;
        case 4:
            wantedPosition = wakePosition4;
            wantedRadiuses = wakeRadiuses4;
            speed = wakeSpeed4;
            break;
        case 5:
            wantedPosition = wakePosition5;
            wantedRadiuses = wakeRadiuses5;
            speed = wakeSpeed5;
            break;

        case 6:
            subEmotion = 0;
            setEmotion(NEUTRAL);
            break;
        }
    }
}

// Update cooldown time for current sub emotion
void Eyes::updateCooldown()
{
    if (subEmotion == 0)
    {
        cooldownTill = millis();
        return;
    }
    switch (emotion)
    {
    case NEUTRAL:
        switch (subEmotion)
        {
        case 0:
            cooldownTill = millis() + neutralTime0;
            break;
        case 1:
            cooldownTill = millis() + neutralTime1;
            break;
        case 2:
            cooldownTill = millis() + neutralTime2;
            break;
        case 3:
            cooldownTill = millis() + neutralTime3;
            break;
        case 4:
            cooldownTill = millis() + neutralTime4;
            break;
        default:
            break;
        }
        break;
    case HAPPY:
        switch (subEmotion)
        {
        case 0:
            cooldownTill = millis() + happyTime0;
            break;
        case 1:
            cooldownTill = millis() + happyTime1;
            break;
        case 2:
            cooldownTill = millis() + happyTime2;
            break;
        case 3:
            cooldownTill = millis() + happyTime3;
            break;
        case 4:
            cooldownTill = millis() + happyTime2;
            break;
        case 5:
            cooldownTill = millis() + happyTime1;
            break;
        default:
            break;
        }
        break;
    case SAD:
        switch (subEmotion)
        {
        case 0:
            cooldownTill = millis() + sadTime0;
            break;
        case 1:
            cooldownTill = millis() + sadTime1;
            break;
        case 2:
            cooldownTill = millis() + sadTime0;
            break;
        case 3:
            cooldownTill = millis() + sadTime3;
            break;
        case 4:
            cooldownTill = millis() + sadTime0;
            break;
        case 5:
            cooldownTill = millis() + sadTime5;
            break;
        case 6:
            cooldownTill = millis() + sadTime0;
            break;
        case 7:
            cooldownTill = millis() + sadTime3;
            break;
        case 8:
            cooldownTill = millis() + sadTime0;
            break;
        default:
            break;
        }
        break;
    case SUS:
        switch (subEmotion)
        {
        case 0:
            cooldownTill = millis() + susTime0;
            break;
        case 1:
            cooldownTill = millis() + susTime1;
            break;
        case 2:
            cooldownTill = millis() + susTime2;
            break;
        default:
            break;
        }
        break;
    case ANGRY:
        switch (subEmotion)
        {
        case 0:
            cooldownTill = millis() + angryTime0;
            break;
        case 1:
            cooldownTill = millis() + angryTime1;
            break;
        case 2:
            cooldownTill = millis() + angryTime2;
            break;
        case 3:
            cooldownTill = millis() + angryTime1;
            break;
        case 4:
            cooldownTill = millis() + angryTime2;
            break;
        case 5:
            cooldownTill = millis() + angryTime1;
            break;
        case 6:
            cooldownTill = millis() + angryTime2;
            break;
        case 7:
            cooldownTill = millis() + angryTime7;
            break;
        default:
            break;
        }
    case WAKE:
        switch (subEmotion)
        {
        case 0:
            cooldownTill = millis() + wakeTime0;
            break;
        case 1:
            cooldownTill = millis() + wakeTime1;
            break;
        case 2:
            cooldownTill = millis() + wakeTime2;
            break;
        case 3:
            cooldownTill = millis() + wakeTime3;
            break;
        case 4:
            cooldownTill = millis() + wakeTime4;
            break;
        case 5:
            cooldownTill = millis() + wakeTime5;
            break;
        case 6:
            cooldownTill = millis() + wakeTime5;
            break;
        default:
            break;
        }
    }
}

// Set emotion and update wanted position/radiuses
void Eyes::setEmotion(Emotion pEmotion)
{
    emotion = pEmotion;

    switch (emotion)
    {
    case NEUTRAL:
        wantedPosition = neutralPosition0;
        wantedRadiuses = neutralRadiuses0;
        break;
    case HAPPY:
        wantedPosition = happyPosition0;
        wantedRadiuses = happyRadiuses0;
        break;
    case SAD:
        wantedPosition = sadPosition0;
        wantedRadiuses = sadRadiuses0;
        break;
    case SUS:
        wantedPosition = susPosition0;
        wantedRadiuses = susRadiuses0;
        break;
    case ANGRY:
        wantedPosition = angryPosition0;
        wantedRadiuses = angryRadiuses0;
        break;
    case WAKE:
        wantedPosition = wakePosition0;
        wantedRadiuses = wakeRadiuses0;
        break;
    default:
        break;
    }
    emotionAchieved = false;
    speed = interEmotionSpeed;
}

// Set speed for eye movement
void Eyes::setSpeed(float pSpeed)
{
    interEmotionSpeed = pSpeed;
}

// Linear interpolation
float Eyes::lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}