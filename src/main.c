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
        gfx_HorizLine_NoClip(spacing, size/(difficulty+3)*i+spacing, size);
    }
    if(difficulty != 0) gfx_SetPixel(size+spacing, size+spacing); //this is here to fix a missing pixel
    return size/(difficulty+3);
}

void drawTile(uint8_t cellSize, uint8_t difficulty, uint8_t gameGrid[][difficulty+3], uint8_t row, uint8_t coloumn)
{
    if (gameGrid[row][coloumn] == 1) gfx_SetColor(24); // blue
    else gfx_SetColor(230); // yellow

    //attention! the + 11 is the value of "spacing" from initGame() + 1
    gfx_FillRectangle_NoClip(coloumn * cellSize + 11,row * cellSize + 11,cellSize-1,cellSize-1);
}

void select(uint8_t cellSize)
{
    bool quit = false;
    int8_t deltaX = 0;
    int8_t deltaY = 0;
    uint8_t selectionX = 10;
    uint8_t selectionY = 10;
    uint8_t OLDselectionX = selectionX;
    uint8_t OLDselectionY = selectionY;

    // Draw first selection
    gfx_SetColor(224);
    gfx_Rectangle_NoClip(selectionX, selectionY, cellSize+1, cellSize+1);

    while (!quit)
    {
        uint8_t key = os_GetCSC();
        if (key) // execute only if a key is pressed
        {
            switch (key)
            {
                case sk_Down: deltaX = 0; deltaY = cellSize; break;
                case sk_Up: deltaX = 0; deltaY = -cellSize; break;
                case sk_Left: deltaX = -cellSize; deltaY = 0; break;
                case sk_Right: deltaX = cellSize; deltaY = 0; break;
                case sk_Clear: quit = true; continue; // Exit the loop
                default: deltaX = 0; deltaY = 0; break;
            }

            // check bounds
            int16_t NEWSelectionY = selectionY + deltaY;
            int16_t NEWSelectionX = selectionX + deltaX;

            //the horror below is the only way my dumb ahh could code boundary detection
            if (NEWSelectionX >= 10 && NEWSelectionX < 220 && NEWSelectionY >= 10 && NEWSelectionY < 220) {
                // remove old selection
                gfx_SetColor(0); // black
                gfx_Rectangle_NoClip(OLDselectionX, OLDselectionY, cellSize + 1, cellSize + 1);

                // update selection coords
                selectionX = NEWSelectionX;
                selectionY = NEWSelectionY;

                // draw new selection
                gfx_SetColor(224); // Red selection
                gfx_Rectangle_NoClip(selectionX, selectionY, cellSize + 1, cellSize + 1);

                // update old position
                OLDselectionX = selectionX;
                OLDselectionY = selectionY;
            }
            else
            {
                // revert to old position if out of bounds
                selectionX = OLDselectionX;
                selectionY = OLDselectionY;
            }
        }
    }
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
    select(cellSize);
    gfx_End();
    return 0;
}
