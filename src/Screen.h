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

#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define SSD1306_I2C_ADDRESS 0x3C

#define SCREEN_WIDTH 72
#define SCREEN_HEIGHT 40
#define SCREEN_HEIGHT_FLOAT 40.0
#define RIGHT_OFFSET 28

struct Vector2
{
    Vector2(int8_t px, int8_t py) : x(px), y(py) {}
    int8_t x = 0;
    int8_t y = 0;
};

struct Vector2f
{
    Vector2f(float px, float py) : x(px), y(py) {}
    float x = 0;
    float y = 0;
};

class Screen
{
public:
    uint8_t i2cAddress;
    Screen(uint8_t address);
    void init();
    void drawBufferToScreen();
    void clearBuffer();
    void fillBuffer();
    void moveCursor(uint8_t page, uint8_t col);
    void setPixel(uint8_t x, uint8_t y, bool colour);
    void stop();
    bool edgeFunctionAboveZero(Vector2 a, Vector2 b, Vector2 c);
    bool edgeFunctionAboveZerof(Vector2f a, Vector2f b, Vector2 c);
    bool pointIsWithinCircle(Vector2 center, int radius, Vector2 point);
    bool pointIsWithinTriangle(Vector2 point, Vector2 r1, Vector2 r2, Vector2 r3);
    bool pointIsWithinRectangle(Vector2 point, Vector2 *vertices, int *radiuses, Vector2 *filletPoints1,
                                Vector2 *filletPoints2, Vector2 *filletPoints3, Vector2 *filletPoints4);
    void findFilletCirclePoints(Vector2 vertex1, Vector2 vertex2, Vector2 vertex3, int radius, Vector2 *arr);
    void drawRectangleInBuffer(Vector2 *vertices, float *radiuses);
    Vector2 minRect(Vector2 r1, Vector2 r2, Vector2 r3, Vector2 r4);
    Vector2 maxRect(Vector2 r1, Vector2 r2, Vector2 r3, Vector2 r4);
    int vectorDotProduct(Vector2f v1, Vector2f v2);
    Vector2f normalize(Vector2f vector);

private:
    void sendCommand(uint8_t command);
    void sendData(uint8_t data);

    uint8_t screenBuffer[5][72] = {0};
};

#endif // SCREEN_H