/*MIT License

Copyright (c) 2024 Valerio "Nutpile" Casalino

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <graphx.h> //graphics library
#include <ti\getcsc.h>  //keypad input
#include <stdlib.h> //for random tile generation
#include <sys\rtc.h> //see above

// remember screen dimentions is 320x240

uint8_t initGame(uint8_t difficulty)
{
    const uint8_t spacing = 10;   //spacing in pixels from the screen borders
    const uint8_t size = 220; // size of the grid, most of the cases do not touch
    gfx_SetColor(0);

    //split square of size "size" in difficulty+3 parts since a 3x3 grid is the smallest it can be
    for(uint8_t i = 0; i<difficulty+4; i++){
        gfx_VertLine_NoClip(size/(difficulty+3)*i+spacing, spacing, size);
        gfx_HorizLine_NoClip(spacing, size/(difficulty+3)*i+spacing, size+1);
    }
    return size/(difficulty+3);
}

void drawTile(uint8_t cellSize, uint8_t difficulty, uint8_t gameGrid[][difficulty+3], uint8_t row, uint8_t coloumn)
{
    if (gameGrid[row][coloumn] == 1) gfx_SetColor(24); // blue
    else gfx_SetColor(230); // yellow

    //attention! the + 11 is the value of "spacing" from initGame() + 1
    gfx_FillRectangle_NoClip(coloumn * cellSize + 11,row * cellSize + 11,cellSize-1,cellSize-1);
}


int main(void)
{
    srand(rtc_Time());
    uint8_t difficulty = 0; // 0 is 3x3, 1 is 4x4, and 2 is 5x5, causes problems if set to over 2 for some reason
    gfx_Begin();
    uint8_t cellSize = initGame(difficulty);
    uint8_t gameGrid[difficulty+3][difficulty+3];
    for(uint8_t i = 0; i < difficulty+3; i++)
    {
        for(uint8_t j = 0; j < difficulty+3; j++)
        {
            gameGrid[i][j] = rand() % 2; // Use % 2 to generate 0 or 1
            drawTile(cellSize, difficulty, gameGrid, i, j);
        }
    }
    while(!os_GetCSC());
    gfx_End();
    return 0;
}
