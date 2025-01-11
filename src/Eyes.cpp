#include <Arduino.h>
#include "eyes.h"
#include "screen.h"

// General update function at 30 fps
void Eyes::updateEyes(bool leftPress, bool rightPress)
{
    bool posAchieved;
    // Don't update during cooldown
    if (millis() > cooldownTill)
    {
        posAchieved = moveEyes();
    }

    if (game == None)
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
        case Pong:
            updatePong(leftPress, rightPress, posAchieved);
            break;
        case Dino:
            updateDino();
            break;
        }
    }

    // Convert eye positions to int for drawing
    Vector2 eyePositionsInt[8] = {vector2fToVector2(eyePositions), vector2fToVector2(eyePositions + 1),
                                  vector2fToVector2(eyePositions + 2), vector2fToVector2(eyePositions + 3),
                                  vector2fToVector2(eyePositions + 4), vector2fToVector2(eyePositions + 5),
                                  vector2fToVector2(eyePositions + 6), vector2fToVector2(eyePositions + 7)};
    screen.clearBuffer();
    // Left eye
    screen.drawRectangleInBuffer(eyePositionsInt, eyeRadiuses);
    // Right eye
    screen.drawRectangleInBuffer(eyePositionsInt + 4, eyeRadiuses + 4);

    if (game != None)
    {
        // Convert game element positions to int for drawing
        Vector2 gameElementPositionInt[4] = {vector2fToVector2(gameElementPositionF), vector2fToVector2(gameElementPositionF + 1),
                                             vector2fToVector2(gameElementPositionF + 2), vector2fToVector2(gameElementPositionF + 3)};
        // Game Element
        screen.drawRectangleInBuffer(gameElementPositionInt, gameElementRadiuses);

        // Serial.print("Game Element: ");
        // Serial.print(gameElementPosition[0].x);
        // Serial.print(", ");
        // Serial.println(gameElementPosition[0].y);

        // Serial.print("Wanted Game Element: ");
        // Serial.print(wantedGameElementPosition[0].x);
        // Serial.print(", ");
        // Serial.println(wantedGameElementPosition[0].y);
    }

    screen.drawBufferToScreen();
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

        speed = 0.1;

        gameState = 1;
        break;
    case 1:
        if (positionsAchieved)
        {
            speed = 0.8;
            gameState = 2;
        }
        break;
    case 2:
        // Serial.println("State 2 Update");

        if (leftPress)
        {
            paddle1Position.y -= 0.5;
        }
        if (rightPress)
        {
            paddle1Position.y += 0.5;
        }

        pongBallPosition.x += pongBallVelocity.x;
        pongBallPosition.y += pongBallVelocity.y;
        if (pongBallPosition.x < 5 || pongBallPosition.x > 66)
        {
            pongBallVelocity.x = -pongBallVelocity.x;
            constrain(pongBallPosition.x, 5, 66);
        }
        if (pongBallPosition.y < 5 || pongBallPosition.y > 35)
        {
            pongBallVelocity.y = -pongBallVelocity.y;
            constrain(pongBallPosition.y, 5, 35);
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

    wantedPosition = paddlePositions;
    wantedRadiuses = paddleRadiuses;

    fakeGameElementPosition[0] = Vector2(pongBallPosition.x - 4, pongBallPosition.y - 4);
    fakeGameElementPosition[1] = Vector2(pongBallPosition.x + 4, pongBallPosition.y - 4);
    fakeGameElementPosition[2] = Vector2(pongBallPosition.x + 4, pongBallPosition.y + 4);
    fakeGameElementPosition[3] = Vector2(pongBallPosition.x - 4, pongBallPosition.y + 4);

    for (int i = 0; i < 4; i++)
    {
        fakeGameElementRadiuses[i] = 2;
    }

    wantedGameElementPosition = fakeGameElementPosition;
    wantedGameElementRadiuses = fakeGameElementRadiuses;
}

void Eyes::updateDino() {}

void Eyes::setGame(Game game)
{
    this->game = game;
    gameState = 0;
}

void Eyes::updateSubEmotion()
{
    //
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

    if (game == None)
    {
        return positionsAchieved && radiusesAchieved;
    }
    else
    {
        // Update game element position
        for (int i = 0; i < 4; i++)
        {
            gameElementPositionF[i] = Vector2f(lerp(gameElementPositionF[i].x, wantedGameElementPosition[i].x, speed),
                                               lerp(gameElementPositionF[i].y, wantedGameElementPosition[i].y, speed));
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
            gameElementRadiuses[i] = lerp(gameElementRadiuses[i], wantedGameElementRadiuses[i], speed);

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
            subEmotion = 0;
            wantedPosition = susPosition2;
            wantedRadiuses = susRadiuses2;
            speed = susSpeed2;
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
    }
}

// Update cooldown time for current sub emotion
void Eyes::updateCooldown()
{
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