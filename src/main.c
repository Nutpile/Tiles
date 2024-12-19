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
#include <gfx\gfx.h> //for logo
#include <stdlib.h> // for exit()

#define MAX_GAME_SIZE 5
static uint8_t gameGrid[MAX_GAME_SIZE][MAX_GAME_SIZE];
unsigned int moves = 0;

// remember screen dimentions is 320x240

static uint8_t cellSize = 0;
static int8_t difficulty = 0; // 0 is 3x3, 1 is 4x4, and 2 is 5x5, causes problems if set to over 2 for some reason
const uint8_t spacing = 10; //spacing in pixels from the screen borders
const uint8_t size = 220; // size of the grid, most of the cases do not touch

void Menu(void);

void drawTile(uint8_t row, uint8_t column)
{
    if (gameGrid[row][column] == 1) gfx_SetColor(24); // blue
    else gfx_SetColor(230); // yellow

    gfx_FillRectangle_NoClip(column * cellSize + spacing + 1,row * cellSize + spacing + 1,cellSize-1,cellSize-1);
}

void drawSideBar(void)
{
    gfx_SetDrawBuffer();
    const int Xpos = 2*spacing+size;
    const uint8_t width = GFX_LCD_WIDTH-size-3*spacing;
    const uint8_t height = GFX_LCD_HEIGHT-2*spacing;

    gfx_SetColor(255);
    gfx_SetTextXY(Xpos+width/6, 2*spacing);
    gfx_FillRectangle_NoClip(Xpos, 2*spacing, width-1, 27);  //move counter
    gfx_SetColor(2);
    gfx_Rectangle_NoClip(Xpos, spacing, width, height); //border
    gfx_SetTextScale(2,2);
    gfx_PrintUInt(moves, 3);

    gfx_SetTextScale(1,1);
    gfx_PrintStringXY("[clear]", Xpos+1.2*spacing, GFX_LCD_HEIGHT-3*spacing);
    gfx_PrintStringXY("-quit-", Xpos+1.2*spacing+2, GFX_LCD_HEIGHT-2*spacing);

    gfx_SetTextXY(Xpos+width/3, 4*spacing);
    gfx_PrintUInt(difficulty+3, 1);
    gfx_PrintChar('x');
    gfx_PrintUInt(difficulty+3, 1);

    gfx_BlitRectangle(gfx_buffer, Xpos, spacing, width, height);

    gfx_SetDrawScreen();
}

void initGame(void)
{
    cellSize = size/(difficulty+3);
    gfx_FillScreen(255);
    moves = 0;
    drawSideBar();
    gfx_SetDrawBuffer();
    for(uint8_t i = 0; i < difficulty+3; i++)
    {
        for(uint8_t j = 0; j < difficulty+3; j++)
        {
            gameGrid[i][j] = rand() % 2; // Use % 2 to generate 0 or 1
            drawTile(i, j);
        }
    }
    gfx_SetColor(2);
    //split square of size "size" in difficulty+3 parts since a 3x3 grid is the smallest it can be
    for(uint8_t i = 0; i<difficulty+4; i++){
        gfx_VertLine_NoClip(cellSize*i+spacing, spacing, size);
        gfx_HorizLine_NoClip(spacing, cellSize*i+spacing, size);
    }
    if(difficulty != 0){
        gfx_SetPixel(size+spacing, size+spacing); //this is here to fix a missing pixel
        gfx_BlitRectangle(gfx_buffer, spacing, spacing, size+1, size+1);
        gfx_SetDrawScreen();    //kind of a shame that i have to execute the same code twice
        return;                 //but i have to since non-3x3s are 1 pixel larger for some reason
    }
    gfx_BlitRectangle(gfx_buffer, spacing, spacing, size, size);
    gfx_SetDrawScreen();
    return;
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

bool click(uint8_t row, uint8_t column)
{
    moves++;
    if(moves > 999) moves=0;
    drawSideBar();
    //invert current tile, no if condition here since it's always possible
    gameGrid[row][column] = !gameGrid[row][column];
    drawTile(row, column);
    //check and invert adjacient tiles
    if(column+1<difficulty+3){
        gameGrid[row][column+1] = !gameGrid[row][column+1];
        drawTile(row, column+1);
    }
    if(column > 0){
        gameGrid[row][column-1] = !gameGrid[row][column-1];
        drawTile(row, column-1);
    }
    if(row+1<difficulty+3){
        gameGrid[row+1][column] = !gameGrid[row+1][column];
        drawTile(row+1, column);
    }
    if(row > 0){
        gameGrid[row-1][column] = !gameGrid[row-1][column];
        drawTile(row-1, column);
    }
    return checkWin();
}

void select(void)
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
                case sk_2: selectionX = 0; selectionY = 0; difficulty = 1; OLDselectionX = selectionX; OLDselectionY = selectionY; initGame(); break;
                case sk_3: selectionX = 0; selectionY = 0; difficulty = 2; OLDselectionX = selectionX; OLDselectionY = selectionY; initGame(); break;
                case sk_1: selectionX = 0; selectionY = 0; difficulty = 0; OLDselectionX = selectionX; OLDselectionY = selectionY; initGame(); break;
                case sk_Clear: quit = true; break; // Exit the loop
                default: deltaX = 0; deltaY = 0; break;
            }

            if (clicked) quit = click(selectionY, selectionX);

            // check bounds
            uint8_t NEWSelectionY = selectionY + deltaY;
            uint8_t NEWSelectionX = selectionX + deltaX;

            deltaX = 0;     //these resets are here to fix an issue with the selection moving
            deltaY = 0;     //only after clicking

            //the horror below is the only way my dumb ahh could code boundary detection
            if (NEWSelectionX >= 0 && NEWSelectionX < difficulty+3 && NEWSelectionY >= 0 && NEWSelectionY < difficulty+3) {
                // remove old selection
                gfx_SetColor(2); // black
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
    Menu();
}

void Menu(void)
{
    gfx_SetDrawBuffer();
    gfx_FillScreen(255);
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_TransparentSprite_NoClip(logo, GFX_LCD_WIDTH / 2 - 100, 50);
    gfx_SetTextFGColor(2);
    gfx_PrintStringXY("Easy", 140, 150);
    gfx_PrintStringXY("Medium", 140, 165);
    gfx_PrintStringXY("Hard", 140, 180);
    gfx_BlitBuffer();
    gfx_SetDrawScreen();

    while (true)
    {
        bool start = false;
        uint8_t key = os_GetCSC(); // Get key input
        gfx_PrintStringXY(">", 130, 150+difficulty*15);
        if(key){

            switch (key)
            {
            case sk_Clear:
                gfx_End();
                exit(0);
            case sk_Down:
                difficulty++;
                break;
            case sk_Up:
                difficulty--;
                break;
            case sk_2nd:
                start = true;
                break;
            default:
                break;
            }
            
            if(difficulty > 2) difficulty = 0;
            else if(difficulty < 0) difficulty = 2;

            gfx_SetColor(1);
            gfx_FillRectangle_NoClip(130, 150, 5, 37);
            gfx_PrintStringXY(">", 130, 150+difficulty*15);

            if(start){
                gfx_SetDrawBuffer();
                gfx_FillScreen(255);
                gfx_SetDrawScreen();

                initGame();
                drawSideBar();
                select();
            }
        }
        // If no key is pressed, gamestate remains 0
    }
}


int main(void)
{
    srand(rtc_Time());
    gfx_Begin();
    Menu();
    gfx_End();
    return 0;
}
