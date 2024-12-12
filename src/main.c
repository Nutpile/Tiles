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

#define MAX_GAME_SIZE 5
static uint8_t gameGrid[MAX_GAME_SIZE][MAX_GAME_SIZE];

// remember screen dimentions is 320x240

static uint8_t cellSize = 0;
static uint8_t difficulty = 0; // 0 is 3x3, 1 is 4x4, and 2 is 5x5, causes problems if set to over 2 for some reason
const uint8_t spacing = 10; //spacing in pixels from the screen borders
const uint8_t size = 220; // size of the grid, most of the cases do not touch

void drawTile(uint8_t cellSize, uint8_t row, uint8_t column)
{
    if (gameGrid[row][column] == 1) gfx_SetColor(24); // blue
    else gfx_SetColor(230); // yellow

    gfx_FillRectangle_NoClip(column * cellSize + spacing + 1,row * cellSize + spacing + 1,cellSize-1,cellSize-1);
}

uint8_t initGame()
{
    gfx_FillScreen(255);
    for(uint8_t i = 0; i < difficulty+3; i++)
    {
        for(uint8_t j = 0; j < difficulty+3; j++)
        {
            gameGrid[i][j] = rand() % 2; // Use % 2 to generate 0 or 1
            drawTile(size/(difficulty+3), i, j);
        }
    }
    gfx_SetColor(0);
    //split square of size "size" in difficulty+3 parts since a 3x3 grid is the smallest it can be
    for(uint8_t i = 0; i<difficulty+4; i++){
        gfx_VertLine_NoClip(size/(difficulty+3)*i+spacing, spacing, size);
        gfx_HorizLine_NoClip(spacing, size/(difficulty+3)*i+spacing, size);
    }
    if(difficulty != 0) gfx_SetPixel(size+spacing, size+spacing); //this is here to fix a missing pixel
    return size/(difficulty+3);
}

bool checkWin(void){
    uint8_t symbol = gameGrid[0][0];
    for(uint8_t i=0; i<difficulty+3; i++){
        for(uint8_t j=0; j<difficulty+3; j++){
            if(symbol != gameGrid[i][j]) return false;
            symbol = gameGrid[i][j];
        }
    }
    return true;
}

bool click(uint8_t row, uint8_t column, uint8_t cellSize)
{
    //invert current tile, no if condition here since it's always possible
    gameGrid[row][column] = !gameGrid[row][column];
    drawTile(cellSize, row, column);
    //check and invert adjacient tiles
    if(column+1<difficulty+3){
        gameGrid[row][column+1] = !gameGrid[row][column+1];
        drawTile(cellSize, row, column+1);
    }
    if(column > 0){
        gameGrid[row][column-1] = !gameGrid[row][column-1];
        drawTile(cellSize, row, column-1);
    }
    if(row+1<difficulty+3){
        gameGrid[row+1][column] = !gameGrid[row+1][column];
        drawTile(cellSize, row+1, column);
    }
    if(row > 0){
        gameGrid[row-1][column] = !gameGrid[row-1][column];
        drawTile(cellSize, row-1, column);
    }
    return checkWin();
}

void select(uint8_t cellSize)
{
    bool quit = false;
    int8_t deltaX = 0;
    int8_t deltaY = 0;
    uint8_t selectionX = 0;
    uint8_t selectionY = 0;
    uint8_t OLDselectionX = selectionX;
    uint8_t OLDselectionY = selectionY;

    // Draw first selection
    gfx_SetColor(224);
    gfx_Rectangle_NoClip(spacing, spacing, cellSize+1, cellSize+1);

    while (!quit)
    {
        uint8_t key = os_GetCSC();
        bool clicked = false;
        if (key) // execute only if a key is pressed
        {
            switch (key)
            {
                case sk_2nd: clicked = true; break;
                case sk_Down: deltaX = 0; deltaY = 1; break;
                case sk_Up: deltaX = 0; deltaY = -1; break;
                case sk_Left: deltaX = -1; deltaY = 0; break;
                case sk_Right: deltaX = 1; deltaY = 0; break;
                //the cases below are a temporary way to change difficulty without rebuildiing
                case sk_2: selectionX = 0; selectionY = 0; difficulty = 1; OLDselectionX = selectionX; OLDselectionY = selectionY; cellSize = initGame();; break;
                case sk_3: selectionX = 0; selectionY = 0; difficulty = 2; OLDselectionX = selectionX; OLDselectionY = selectionY; cellSize = initGame();; break;
                case sk_1: selectionX = 0; selectionY = 0; difficulty = 0; OLDselectionX = selectionX; OLDselectionY = selectionY; cellSize = initGame();; break;
                case sk_Clear: quit = true; break; // Exit the loop
                default: deltaX = 0; deltaY = 0; break;
            }

            if (clicked) quit = click(selectionY, selectionX, cellSize);

            // check bounds
            uint8_t NEWSelectionY = selectionY + deltaY;
            uint8_t NEWSelectionX = selectionX + deltaX;

            deltaX = 0;     //these resets are here to fix an issue with the selection moving
            deltaY = 0;     //only after clicking

            //the horror below is the only way my dumb ahh could code boundary detection
            if (NEWSelectionX >= 0 && NEWSelectionX < difficulty+3 && NEWSelectionY >= 0 && NEWSelectionY < difficulty+3) {
                // remove old selection
                gfx_SetColor(0); // black
                gfx_Rectangle_NoClip(OLDselectionX*cellSize+spacing, OLDselectionY*cellSize+spacing, cellSize + 1, cellSize + 1);

                // update selection coords
                selectionX = NEWSelectionX;
                selectionY = NEWSelectionY;

                // draw new selection
                gfx_SetColor(224); // red selection
                gfx_Rectangle_NoClip(selectionX*cellSize+spacing, selectionY*cellSize+spacing, cellSize + 1, cellSize + 1);

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
    gfx_Begin();
    cellSize = initGame();
    select(cellSize);
    gfx_End();
    return 0;
}
