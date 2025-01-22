#include <Arduino.h>
#include "screen.h"
#include <Wire.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// Constructor
Screen::Screen(uint8_t address) : i2cAddress(address)
{
    Wire.begin(21, 22, 400000);
    delay(1000);
}

// Send command to SSD1306
void Screen::sendCommand(uint8_t command)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x00); // Co = 0, D/C = 0
    Wire.write(command);
    Wire.endTransmission();
}

// Send data to SSD1306
void Screen::sendData(uint8_t data)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x40); // Co = 0, D/C = 1
    Wire.write(data);
    Wire.endTransmission();
}

// Initialize SSD1306
void Screen::init()
{
    // Initialize SSD1306
    sendCommand(0xAE); // Display off
    sendCommand(0xD5); // Set display clock divide ratio/oscillator frequency
    sendCommand(0x80); // Set divide ratio
    sendCommand(0xA8); // Set multiplex ratio
    sendCommand(0x27); // 40 (multiplex ratio for 40 rows)
    sendCommand(0xD3); // Set display offset (this is vertical offset)
    sendCommand(0x00); // Offset Value (no vertical offset)
    sendCommand(0x40); // Set start line address (start from line 0)
    sendCommand(0x8D); // Charge pump setting
    sendCommand(0x14); // Enable charge pump
    sendCommand(0x20); // Memory addressing mode
    sendCommand(0x00); // Horizontal addressing mode
    sendCommand(0xA1); // Set segment re-map (column address 127 is mapped to SEG0)
    sendCommand(0xC8); // Set COM output scan direction (remapped mode)
    sendCommand(0xDA); // Set COM pins hardware configuration
    sendCommand(0x12); // Alternative COM pin configuration, disable COM left/right remap
    sendCommand(0x81); // Set contrast control
    sendCommand(0xCF); // Contrast value
    sendCommand(0xD9); // Set pre-charge period
    sendCommand(0xF1); // Phase 1 period: 15 DCLKs, Phase 2 period: 1 DCLK
    sendCommand(0xDB); // Set VCOMH deselect level
    sendCommand(0x40); // VCOMH deselect level ~0.77*Vcc
    sendCommand(0xA4); // Entire display on (resume to RAM content display)
    sendCommand(0xA6); // Set normal display (0xA6 for normal, 0xA7 for inverse)
    sendCommand(0xAF); // Display on
}

// Move cursor to a specific page and column
void Screen::moveCursor(uint8_t page, uint8_t col)
{
    sendCommand(0xB0 + page);                          // Set page address
    sendCommand(0x00 | ((RIGHT_OFFSET + col) & 0x0F)); // Set lower column address (32 & 0x0F extracts the lower 4 bits of 32)
    sendCommand(0x10 | ((RIGHT_OFFSET + col) >> 4));   // Set higher column address (32 >> 4 extracts the higher 4 bits of 32)
}

// Draw the buffer to the screen
void Screen::drawBufferToScreen()
{
    for (uint8_t page = 0; page < 5; page++)
    { // 5 pages for 40 pixels height
        moveCursor(page, 0);
        for (uint8_t col = 0; col < SCREEN_WIDTH; col++)
        {
            sendData(screenBuffer[page][col]); // Each byte represents 8 vertical pixels
        }
    }
}

// Clear the screen buffer
void Screen::clearBuffer()
{
    memset(screenBuffer, 0x00, sizeof screenBuffer);
}

// Fill the screen buffer with full white
void Screen::fillBuffer()
{
    memset(screenBuffer, 0xff, sizeof screenBuffer);
}

// Set colour of pixel
void Screen::setPixel(uint8_t x, uint8_t y, boolean colour)
{
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
    {
        return;
    }
    if (colour)
    {
        screenBuffer[y / 8][x] |= (1 << (y % 8));
    }
    else
    {
        screenBuffer[y / 8][x] &= ~(1 << (y % 8));
    }
}

// Stop the screen
void Screen::stop()
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x00); // Command mode
    Wire.write(0xAE); // Display off
    Wire.write(0x8D); // Charge pump setting
    Wire.write(0x10); // Disable charge pump
    Wire.write(0xA5); // Entire display ON, ignore RAM content
    Wire.endTransmission();
}

// Check if a point is below a linear line
boolean Screen::edgeFunctionAboveZero(Vector2 a, Vector2 b, Vector2 c)
{
    if (a.x == b.x && a.y == b.y)
    {
        return false;
    }
    return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) >= 0);
} // point is within shape if it's within all edges

// Check if a point is below a linear line with float values
boolean Screen::edgeFunctionAboveZerof(Vector2f a, Vector2f b, Vector2 c)
{
    return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) > 0);
} // point is within shape if it's within all edges

// Check if a point is within a circle
boolean Screen::pointIsWithinCircle(Vector2 center, int radius, Vector2 point)
{
    if (radius == 0)
    {
        return false;
    }
    Vector2f diff = Vector2f(point.x - center.x, point.y - center.y);
    return (sqrt(diff.x * diff.x + diff.y * diff.y) < radius);
}

// Check if a point is within a triangle
boolean Screen::pointIsWithinTriangle(Vector2 point, Vector2 r1, Vector2 r2, Vector2 r3)
{
    return edgeFunctionAboveZero(r1, r2, point) && edgeFunctionAboveZero(r2, r3, point) && edgeFunctionAboveZero(r3, r1, point);
}

// Check if a point is within a rectangle
boolean Screen::pointIsWithinRectangle(Vector2 point, Vector2 *vertices, int *radiuses, Vector2 *filletPoints1,
                                       Vector2 *filletPoints2, Vector2 *filletPoints3, Vector2 *filletPoints4)
{
    boolean pointWithinRectangle = edgeFunctionAboveZero(vertices[0], vertices[1], point) && edgeFunctionAboveZero(vertices[1], vertices[2], point) &&
                                   edgeFunctionAboveZero(vertices[2], vertices[3], point) && edgeFunctionAboveZero(vertices[3], vertices[0], point);

    if (pointWithinRectangle)
    {
        boolean pointOutsideRoundedEdges = (edgeFunctionAboveZero(filletPoints1[2], filletPoints1[0], point) && edgeFunctionAboveZero(filletPoints1[0], filletPoints1[1], point) &&
                                            !pointIsWithinCircle(filletPoints1[0], radiuses[0], point)) ||
                                           (edgeFunctionAboveZero(filletPoints2[2], filletPoints2[0], point) && edgeFunctionAboveZero(filletPoints2[0], filletPoints2[1], point) &&
                                            !pointIsWithinCircle(filletPoints2[0], radiuses[1], point)) ||
                                           (edgeFunctionAboveZero(filletPoints3[2], filletPoints3[0], point) && edgeFunctionAboveZero(filletPoints3[0], filletPoints3[1], point) &&
                                            !pointIsWithinCircle(filletPoints3[0], radiuses[2], point)) ||
                                           (edgeFunctionAboveZero(filletPoints4[2], filletPoints4[0], point) && edgeFunctionAboveZero(filletPoints4[0], filletPoints4[1], point) &&
                                            !pointIsWithinCircle(filletPoints4[0], radiuses[3], point));
        return !pointOutsideRoundedEdges;
    }
    return false;
}

// Find the points of a circle that is tangent to two lines
void Screen::findFilletCirclePoints(Vector2 vertex1, Vector2 vertex2, Vector2 vertex3, int radius, Vector2 *arr)
{
    if (radius == 0)
    {
        return;
    }
    Vector2f a = normalize(Vector2f(vertex2.x - vertex1.x, vertex2.y - vertex1.y));
    Vector2f b = normalize(Vector2f(vertex2.x - vertex3.x, vertex2.y - vertex3.y));
    float halfang = acos(vectorDotProduct(a, b)) / 2;
    Vector2f factor = normalize(Vector2f((a.x + b.x) / 2, (a.y + b.y) / 2));
    Vector2 Cr1 = Vector2(int(round(float(vertex2.x) - float(radius) / sin(halfang) * factor.x)), int(round(float(vertex2.y) - float(radius) / sin(halfang) * factor.y)));
    Vector2 Ar1 = Vector2(int(round(float(vertex2.x) - float(radius) / tan(halfang) * a.x)), int(round(float(vertex2.y) - float(radius) / tan(halfang) * a.y)));
    Vector2 Br1 = Vector2(int(round(float(vertex2.x) - float(radius) / tan(halfang) * b.x)), int(round(float(vertex2.y) - float(radius) / tan(halfang) * b.y)));
    arr[0] = Cr1;
    arr[1] = Ar1;
    arr[2] = Br1;
}

// Draw a rectangle in the screen buffer
void Screen::drawRectangleInBuffer(Vector2 *vertices, float *radiuses)
{
    int radiusesInt[4] = {int(radiuses[0]), int(radiuses[1]), int(radiuses[2]), int(radiuses[3])};
    Vector2 min = minRect(vertices[0], vertices[1], vertices[2], vertices[3]);
    Vector2 max = maxRect(vertices[0], vertices[1], vertices[2], vertices[3]);

    int minPage = floor(min.y / 8.0);
    int maxPage = floor(max.y / 8.0);

    Vector2 filletPoints1[3] = {Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)};
    Vector2 filletPoints2[3] = {Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)};
    Vector2 filletPoints3[3] = {Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)};
    Vector2 filletPoints4[3] = {Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)};

    findFilletCirclePoints(vertices[3], vertices[0], vertices[1], radiusesInt[0], filletPoints1);
    findFilletCirclePoints(vertices[0], vertices[1], vertices[2], radiusesInt[1], filletPoints2);
    findFilletCirclePoints(vertices[1], vertices[2], vertices[3], radiusesInt[2], filletPoints3);
    findFilletCirclePoints(vertices[2], vertices[3], vertices[0], radiusesInt[3], filletPoints4);

    for (uint8_t page = minPage; page < maxPage + 1; page++) // might need to be maxPage + 1
    {
        for (uint8_t col = min.x; col < max.x + 1; col++)
        {
            Vector2 pixel = Vector2(col, page * 8);
            uint8_t newValue = 0;
            for (uint8_t miniY = 0; miniY < 8; miniY++)
            {
                if (pointIsWithinRectangle(Vector2(pixel.x, pixel.y + miniY), vertices, radiusesInt, filletPoints1, filletPoints2, filletPoints3, filletPoints4))
                {
                    newValue = newValue | (uint8_t)(1 << miniY);
                }
            }
            screenBuffer[page][col] = newValue | screenBuffer[page][col];
        }
    }
}

// Find the minimum point of a rectangle
Vector2 Screen::minRect(Vector2 r1, Vector2 r2, Vector2 r3, Vector2 r4)
{
    // Vector2 min = r1;
    // if (r2.x < min.x)
    // {
    //     min.x = r2.x;
    // }
    // if (r2.y < min.y)
    // {
    //     min.y = r2.y;
    // }
    // if (r3.x < min.x)
    // {
    //     min.x = r3.x;
    // }
    // if (r3.y < min.y)
    // {
    //     min.y = r3.y;
    // }
    // if (r4.x < min.x)
    // {
    //     min.x = r4.x;
    // }
    // if (r4.y < min.y)
    // {
    //     min.y = r4.y;
    // }

    Vector2 min = Vector2(min(min(r1.x, r2.x), min(r3.x, r4.x)), min(min(r1.y, r2.y), min(r3.y, r4.y)));

    // min.x = min(min(min(r1.x, r2.x), min(r3.x, r4.x)), 0);
    // min.y = min(min(min(r1.y, r2.y), min(r3.y, r4.y)), 0);

    return min;
}

// Find the maximum point of a rectangle
Vector2 Screen::maxRect(Vector2 r1, Vector2 r2, Vector2 r3, Vector2 r4)
{
    // Vector2 max = r1;
    // if (r2.x > max.x)
    // {
    //     max.x = r2.x;
    // }
    // if (r2.y > max.y)
    // {
    //     max.y = r2.y;
    // }
    // if (r3.x > max.x)
    // {
    //     max.x = r3.x;
    // }
    // if (r3.y > max.y)
    // {
    //     max.y = r3.y;
    // }
    // if (r4.x > max.x)
    // {
    //     max.x = r4.x;
    // }
    // if (r4.y > max.y)
    // {
    //     max.y = r4.y;
    // }

    Vector2 max = Vector2(max(max(r1.x, r2.x), max(r3.x, r4.x)), max(max(r1.y, r2.y), max(r3.y, r4.y)));

    // max.x = max(max(max(r1.x, r2.x), max(r3.x, r4.x)), SCREEN_WIDTH - 1);
    // max.y = max(max(max(r1.y, r2.y), max(r3.y, r4.y)), SCREEN_HEIGHT - 1);

    return max;
}

// Calculate the dot product of two vectors
int Screen::vectorDotProduct(Vector2f v1, Vector2f v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

// Normalize a vector
Vector2f Screen::normalize(Vector2f vector)
{
    float mag = sqrt(vector.x * vector.x + vector.y * vector.y);
    float mag_inv = 1.0 / mag;
    return Vector2f(vector.x * mag_inv, vector.y * mag_inv);
}
