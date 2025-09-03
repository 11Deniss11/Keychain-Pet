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

#ifndef EYES_H
#define EYES_H

#include <stdint.h>
#include "screen.h"
// #include <esp_system.h>

class Eyes
{
public:
    Eyes(Screen &pScreen) : screen(pScreen) {};
    void updateEyes(bool leftPress, bool rightPress);
    enum Emotion
    {
        NEUTRAL,
        HAPPY,
        SAD,
        SUS,
        ANGRY,
        WAKE,
        HEART
    };
    enum Game
    {
        NONE,
        PONG,
        DINO
    };
    Emotion emotion = WAKE;
    Game game = NONE;
    float speed = 0.15;
    float interEmotionSpeed = 0.2;
    void setEmotion(Emotion pEmotion);
    void setSpeed(float speed);
    void setGame(Game pGame);
    bool emotionAchieved = true;
    // boolean subEmotionAchieved = true;
    int subEmotion = 0;

private:
    Screen &screen;
    int positionTolerance = 2;
    bool moveEyes();
    void updateSubEmotion();
    void resetBall();
    void updatePong(bool leftPress, bool rightPress, bool positionsAchieved);
    void updateDino(bool leftPress, bool rightPress, bool positionsAchieved);
    float lerp(float a, float b, float f);
    Vector2 vector2fToVector2(Vector2f *arr);
    void cycleSubEmotion();
    void updateCooldown();
    unsigned long cooldownTill = 0;

    int gameState = 0;
    int gameScore1 = 0;
    int gameScore2 = 0;

    long int buttonHoldEndTime = 0;
    bool buttonHold = false;

    Vector2f paddle1Position = Vector2f(4, 7);
    Vector2f paddle2Position = Vector2f(68, 7);

    Vector2 paddlePositions[8] = {Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0),
                                  Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)};
    float paddleRadiuses[8] = {1, 1, 1, 1,
                               1, 1, 1, 1};

    Vector2f pongBallVelocity = Vector2f(1, 1);
    Vector2f pongBallPosition = Vector2f(36, 20);

    long int pongPlayingAnimEndTime = 0;

    float dinoVertVelocity = 0;
    float dinoVertPosition = 34;
    float dinoSpeed = 1;
    float dinoJumpSpeed = 3.5;
    float dinoGravity = 0.4;
    int dinoFloor = 34;
    bool cactusIsBird = false;
    float cactusPositionX = 75;
    int cactusHalfWidth = 3;
    int cactusHeight = 7;
    int cactusFloor = dinoFloor;
    float cactusRadiuses = 1;
    bool dinoCrouched = false;

    Vector2 dinoStandPosition[4] = {Vector2(8, 0), Vector2(8, -14), Vector2(17, -14), Vector2(17, 0)};
    Vector2 dinoCrouchPosition[4] = {Vector2(4, -6), Vector2(17, -10), Vector2(21, -2), Vector2(7, 3)};
    Vector2 dinoPositions[8] = {Vector2(7, 14), Vector2(18, 14), Vector2(18, 68), Vector2(7, 68),
                                Vector2(0, dinoFloor), Vector2(70, dinoFloor), Vector2(70, 40), Vector2(0, 40)};
    float dinoRadiuses[8] = {1, 1, 1, 1,
                             0, 0, 0, 0};

    Vector2f dinoStars[15] = {
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
        Vector2f(esp_random() % SCREEN_WIDTH, esp_random() % dinoFloor),
    };

    // Extra Game Element
    Vector2 fakeGameElementPosition[4] = {Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)};
    Vector2f gameElementPositionF[4] = {Vector2f(0, 0), Vector2f(0, 0), Vector2f(0, 0), Vector2f(0, 0)};
    // Vector2 gameElementPosition[4] = {Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)};
    float fakeGameElementRadiuses[4] = {0, 0, 0, 0};
    float gameElementRadiuses[4] = {0, 0, 0, 0};
    float gameElementSpeed = 0.5;

    Vector2 *wantedGameElementPosition = fakeGameElementPosition;
    float *wantedGameElementRadiuses = gameElementRadiuses;

    // Neutral, 0
    Vector2 neutralPosition0[8] = {Vector2(11, 6), Vector2(32, 6), Vector2(32, 31), Vector2(11, 31),
                                   Vector2(41, 6), Vector2(62, 6), Vector2(62, 31), Vector2(41, 31)};
    float neutralRadiuses0[8] = {5, 5, 5, 5,
                                 5, 5, 5, 5};
    int neutralTime0 = 1500;
    float neutralSpeed0 = 0.2;

    // Neutral, 1
    Vector2 neutralPosition1[8] = {Vector2(4, 3), Vector2(23, 3), Vector2(23, 27), Vector2(4, 26),
                                   Vector2(28, 3), Vector2(48, 3), Vector2(48, 28), Vector2(28, 27)};
    float neutralRadiuses1[8] = {4, 4, 4, 4,
                                 5, 5, 5, 5};
    int neutralTime1 = 800;
    float neutralSpeed1 = 0.2;

    // Neutral, 2
    Vector2 neutralPosition2[8] = {Vector2(11, 6), Vector2(32, 6), Vector2(32, 31), Vector2(11, 31),
                                   Vector2(41, 6), Vector2(62, 6), Vector2(62, 31), Vector2(41, 31)};
    float neutralRadiuses2[8] = {5, 5, 5, 5,
                                 5, 5, 5, 5};
    int neutralTime2 = 1500;
    float neutralSpeed2 = 0.2;

    // Neutral, 3
    Vector2 neutralPosition3[8] = {Vector2(9, 10), Vector2(30, 10), Vector2(30, 34), Vector2(9, 34),
                                   Vector2(38, 10), Vector2(60, 10), Vector2(60, 34), Vector2(38, 34)};
    float neutralRadiuses3[8] = {5, 5, 5, 5,
                                 5, 5, 5, 5};
    int neutralTime3 = 800;
    float neutralSpeed3 = 0.2;

    // Neutral, 4
    Vector2 neutralPosition4[8] = {Vector2(23, 6), Vector2(43, 7), Vector2(43, 32), Vector2(23, 33),
                                   Vector2(51, 7), Vector2(70, 7), Vector2(70, 32), Vector2(51, 32)};
    float neutralRadiuses4[8] = {5, 5, 5, 5,
                                 4, 4, 4, 4};
    int neutralTime4 = 800;
    float neutralSpeed4 = 0.2;

    // Happy 0
    Vector2 happyPosition0[8] = {Vector2(10, 4), Vector2(31, 4), Vector2(31, 12), Vector2(10, 12),
                                 Vector2(42, 4), Vector2(63, 4), Vector2(63, 12), Vector2(42, 12)};
    float happyRadiuses0[8] = {10, 10, 0, 0,
                               10, 10, 0, 0};
    int happyTime0 = 400;
    float happySpeed0 = 0.2;

    // Happy 1
    Vector2 happyPosition1[8] = {Vector2(6, 4), Vector2(27, 3), Vector2(27, 13), Vector2(6, 12),
                                 Vector2(38, 3), Vector2(58, 3), Vector2(58, 13), Vector2(38, 13)};
    float happyRadiuses1[8] = {10, 10, 0, 0,
                               10, 10, 0, 0};
    int happyTime1 = 100;
    float happySpeed1 = 0.2;

    // Happy 2
    Vector2 happyPosition2[8] = {Vector2(10, 9), Vector2(31, 9), Vector2(31, 18), Vector2(10, 18),
                                 Vector2(42, 9), Vector2(63, 9), Vector2(63, 18), Vector2(42, 18)};
    float happyRadiuses2[8] = {10, 10, 0, 0,
                               10, 10, 0, 0};
    int happyTime2 = 100;
    float happySpeed2 = 0.2;

    // Happy 3
    Vector2 happyPosition3[8] = {Vector2(15, 3), Vector2(35, 3), Vector2(35, 13), Vector2(15, 13),
                                 Vector2(46, 3), Vector2(67, 4), Vector2(67, 12), Vector2(46, 13)};
    float happyRadiuses3[8] = {10, 10, 0, 0,
                               10, 10, 0, 0};
    int happyTime3 = 100;
    float happySpeed3 = 0.2;

    // Sad 0
    Vector2 sadPosition0[8] = {Vector2(11, 25), Vector2(32, 19), Vector2(32, 34), Vector2(11, 34),
                               Vector2(41, 19), Vector2(62, 25), Vector2(62, 34), Vector2(41, 34)};
    float sadRadiuses0[8] = {3, 3, 3, 3,
                             3, 3, 3, 3};
    int sadTime0 = 800;
    float sadSpeed0 = 0.2;

    // Sad 1
    Vector2 sadPosition1[8] = {Vector2(7, 26), Vector2(28, 20), Vector2(28, 33), Vector2(7, 33),
                               Vector2(37, 19), Vector2(58, 25), Vector2(58, 34), Vector2(37, 34)};
    float sadRadiuses1[8] = {3, 3, 3, 3,
                             3, 3, 3, 3};
    int sadTime1 = 800;
    float sadSpeed1 = 0.2;

    // Sad 2 is same as Sad 0

    // Sad 3
    Vector2 sadPosition3[8] = {Vector2(11, 22), Vector2(32, 15), Vector2(32, 33), Vector2(11, 33),
                               Vector2(41, 15), Vector2(62, 22), Vector2(62, 33), Vector2(41, 33)};
    float sadRadiuses3[8] = {3, 3, 3, 3,
                             3, 3, 3, 3};
    int sadTime3 = 400;
    float sadSpeed3 = 0.2;

    // Sad 4 is same as Sad 0

    // Sad 5
    Vector2 sadPosition5[8] = {Vector2(15, 25), Vector2(36, 19), Vector2(36, 34), Vector2(15, 34),
                               Vector2(45, 20), Vector2(66, 24), Vector2(66, 33), Vector2(45, 33)};
    float sadRadiuses5[8] = {3, 3, 3, 3,
                             3, 3, 3, 3};
    int sadTime5 = 800;
    float sadSpeed5 = 0.2;

    // Sad 6 is same as Sad 0
    // Sad 7 is same as Sad 3
    // Sad 8 is same as Sad 0

    // Sus 0
    Vector2 susPosition0[8] = {Vector2(11, 4), Vector2(32, 4), Vector2(32, 32), Vector2(11, 32),
                               Vector2(41, 17), Vector2(62, 13), Vector2(62, 30), Vector2(41, 30)};
    float susRadiuses0[8] = {5, 5, 5, 5,
                             0, 0, 5, 5};
    int susTime0 = 200;
    float susSpeed0 = 0.2;

    // Sus 1
    Vector2 susPosition1[8] = {Vector2(7, 5), Vector2(28, 5), Vector2(28, 32), Vector2(7, 32),
                               Vector2(37, 17), Vector2(58, 13), Vector2(58, 30), Vector2(37, 30)};
    float susRadiuses1[8] = {5, 5, 5, 5,
                             0, 0, 5, 5};
    int susTime1 = 1000;
    float susSpeed1 = 0.2;

    // Sus 2
    Vector2 susPosition2[8] = {Vector2(15, 13), Vector2(36, 17), Vector2(36, 30), Vector2(15, 30),
                               Vector2(45, 4), Vector2(66, 4), Vector2(66, 32), Vector2(45, 32)};
    float susRadiuses2[8] = {0, 0, 5, 5,
                             5, 5, 5, 5};
    int susTime2 = 700;
    float susSpeed2 = 0.2;

    // Sus 3 is same as Sus 0

    // Angry 0
    Vector2 angryPosition0[8] = {Vector2(11, 10), Vector2(32, 14), Vector2(32, 25), Vector2(11, 25),
                                 Vector2(41, 14), Vector2(62, 10), Vector2(62, 25), Vector2(41, 25)};
    float angryRadiuses0[8] = {0, 0, 4, 4,
                               0, 0, 4, 4};
    int angryTime0 = 800;
    float angrySpeed0 = 0.2;

    // Angry 1
    Vector2 angryPosition1[8] = {Vector2(8, 11), Vector2(28, 15), Vector2(28, 25), Vector2(8, 25),
                                 Vector2(37, 14), Vector2(58, 10), Vector2(58, 25), Vector2(37, 25)};
    float angryRadiuses1[8] = {0, 0, 4, 4,
                               0, 0, 4, 4};
    int angryTime1 = 80;
    float angrySpeed1 = 0.4;

    // Angry 2
    Vector2 angryPosition2[8] = {Vector2(15, 10), Vector2(36, 14), Vector2(36, 25), Vector2(15, 25),
                                 Vector2(45, 15), Vector2(65, 11), Vector2(65, 25), Vector2(45, 25)};
    float angryRadiuses2[8] = {0, 0, 4, 4,
                               0, 0, 4, 4};
    int angryTime2 = 80;
    float angrySpeed2 = 0.4;

    // Angry 3 same as Angry 1
    // Angry 4 same as Angry 2
    // Angry 5 same as Angry 1
    // Angry 6 same as Angry 2

    // Angry 7
    Vector2 angryPosition7[8] = {Vector2(11, 10), Vector2(32, 12), Vector2(32, 25), Vector2(11, 25),
                                 Vector2(41, 12), Vector2(62, 10), Vector2(62, 25), Vector2(41, 25)};
    float angryRadiuses7[8] = {0, 0, 4, 4,
                               0, 0, 4, 4};
    int angryTime7 = 200;
    float angrySpeed7 = 0.2;

    // Wake 0
    Vector2 wakePosition0[8] = {Vector2(9, 16), Vector2(34, 14), Vector2(34, 15), Vector2(9, 17),
                                Vector2(40, 15), Vector2(65, 17), Vector2(65, 18), Vector2(40, 16)};
    float wakeRadiuses0[8] = {0, 0, 0, 0,
                              0, 0, 0, 0};
    int wakeTime0 = 700;
    float wakeSpeed0 = 0.2;

    // Wake 1
    Vector2 wakePosition1[8] = {Vector2(9, 15), Vector2(33, 13), Vector2(33, 19), Vector2(9, 20),
                                Vector2(40, 14), Vector2(64, 16), Vector2(64, 21), Vector2(40, 20)};
    float wakeRadiuses1[8] = {0, 0, 4, 4,
                              0, 0, 4, 4};
    int wakeTime1 = 1000;
    float wakeSpeed1 = 0.12;

    // Wake 2 is mostly same as Wake 0
    int wakeTime2 = 300;
    float wakeSpeed2 = 0.12;

    // Wake 3
    Vector2 wakePosition3[8] = {Vector2(10, 11), Vector2(32, 11), Vector2(32, 24), Vector2(10, 24),
                                Vector2(41, 11), Vector2(63, 12), Vector2(63, 25), Vector2(41, 24)};
    float wakeRadiuses3[8] = {4, 4, 8, 8,
                              4, 4, 8, 8};
    int wakeTime3 = 400;
    float wakeSpeed3 = 0.2;

    // Wake 4
    Vector2 wakePosition4[8] = {Vector2(10, 17), Vector2(33, 17), Vector2(33, 18), Vector2(10, 18),
                                Vector2(40, 17), Vector2(63, 17), Vector2(63, 18), Vector2(40, 18)};
    float wakeRadiuses4[8] = {0, 0, 0, 0,
                              0, 0, 0, 0};
    int wakeTime4 = 50;
    float wakeSpeed4 = 0.4;

    // Wake 5
    Vector2 wakePosition5[8] = {Vector2(11, 6), Vector2(32, 6), Vector2(32, 31), Vector2(11, 31),
                                Vector2(41, 6), Vector2(62, 6), Vector2(62, 31), Vector2(41, 31)};
    float wakeRadiuses5[8] = {5, 5, 5, 5,
                              5, 5, 5, 5};
    int wakeTime5 = 1300;
    float wakeSpeed5 = 0.4;

    // Heart, 0
    Vector2 heartPosition0[8] = {Vector2(14, 20), Vector2(27, 2), Vector2(49, 17), Vector2(36, 35),
                                 Vector2(46, 2), Vector2(59, 20), Vector2(37, 35), Vector2(24, 17)};
    float heartRadiuses0[8] = {7, 7, 0, 0,
                               7, 7, 0, 0};
    int heartTime0 = 1500;
    float heartSpeed0 = 0.2;

    // Heart, 1
    Vector2 heartPosition1[8] = {Vector2(17, 20), Vector2(28, 5), Vector2(47, 18), Vector2(36, 33),
                                 Vector2(45, 5), Vector2(56, 20), Vector2(37, 33), Vector2(26, 18)};
    float heartRadiuses1[8] = {6.5, 6.5, 0, 0,
                               6.5, 6.5, 0, 0};
    int heartTime1 = 800;
    float heartSpeed1 = 0.2;

    // Heart, 2
    Vector2 heartPosition2[8] = {Vector2(14, 20), Vector2(27, 2), Vector2(49, 17), Vector2(36, 35),
                                 Vector2(46, 2), Vector2(59, 20), Vector2(37, 35), Vector2(24, 17)};
    float heartRadiuses2[8] = {7, 7, 0, 0,
                               7, 7, 0, 0};
    int heartTime2 = 1500;
    float heartSpeed2 = 0.2;

    // Heart, 3
    Vector2 heartPosition3[8] = {Vector2(17, 20), Vector2(28, 5), Vector2(47, 18), Vector2(36, 33),
                                 Vector2(45, 5), Vector2(56, 20), Vector2(37, 33), Vector2(26, 18)};
    float heartRadiuses3[8] = {6.5, 6.5, 0, 0,
                               6.5, 6.5, 0, 0};
    int heartTime3 = 800;
    float heartSpeed3 = 0.2;

    // Heart, 4
    Vector2 heartPosition4[8] = {Vector2(14, 20), Vector2(27, 2), Vector2(49, 17), Vector2(36, 35),
                                 Vector2(46, 2), Vector2(59, 20), Vector2(37, 35), Vector2(24, 17)};
    float heartRadiuses4[8] = {7, 7, 0, 0,
                               7, 7, 0, 0};
    int heartTime4 = 800;
    float heartSpeed4 = 0.2;

    Vector2 *wantedPosition = neutralPosition0;
    float *wantedRadiuses = neutralRadiuses0;

    Vector2f eyePositions[8] = {Vector2f(9, 16), Vector2f(34, 14), Vector2f(34, 15), Vector2f(9, 17),
                                Vector2f(40, 15), Vector2f(65, 17), Vector2f(65, 18), Vector2f(40, 16)};
    float eyeRadiuses[8] = {0, 0, 4, 4,
                            0, 0, 4, 4};
};

#endif // EYES_H