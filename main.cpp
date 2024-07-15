#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <thread>
#include <vector>

using namespace std;

wstring tetrominos[7];

int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr;

int nScreenWidth = 80;   // console screen size
int nScreenHeight = 30;  // console screen size

void initTetrominos()
{
     // use 1-d array rather than 2-d array for easy rotation
    tetrominos[0].append(L".X..")
                 .append(L".X..")
                 .append(L".X..")
                 .append(L".X..");

    tetrominos[1].append(L"..X.")
                 .append(L".XX.")
                 .append(L".X..")
                 .append(L"....");

    tetrominos[2].append(L".X..")
                 .append(L".XX.")
                 .append(L"..X.")
                 .append(L"....");
    
    tetrominos[3].append(L".XX.")
                 .append(L".XX.")
                 .append(L"....")
                 .append(L"....");
    
    tetrominos[4].append(L"..X.")
                 .append(L".XX.")
                 .append(L"..X.")
                 .append(L"....");
    
    tetrominos[5].append(L".XX.")
                 .append(L"..X.")
                 .append(L"..X.")
                 .append(L"....");

    tetrominos[6].append(L".XX.")
                 .append(L".X..")
                 .append(L".X..")
                 .append(L"....");
}

void initField()
{
    pField = new unsigned char[nFieldWidth * nFieldHeight];
    for (int x = 0; x < nFieldWidth; x++)
        for (int y = 0; y < nFieldHeight; y++)
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;  // 9 for boarder
}

int Rotate(int px, int py, int r)
{
    switch (r % 4)
    {
    case 0: return (py * 4) + px;       // 0 degree
    case 1: return 12 + py - (px * 4);  // 90 degree
    case 2: return 15 - (4 * py) - px;  // 180 degree
    case 3: return 3 - py + (4 * px);    // 270 degree
    }
    return 0;
}

// test if the tetromino fit in current pos. (x,y) is upper left conner
bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
        {
            // get index in tetrominoqaawdwwd
            int ti = Rotate(x, y, nRotation);
            // get index in field
            int fi = (nPosY + y) * nFieldWidth + (nPosX + x);

            // Note that out of bound does not necessarily mean a fail
            // '.' part of tetromino can be out of bound
            if ((nPosX + x) >= 0 && (nPosX + x) < nFieldWidth &&
                (nPosY + y) >= 0 && (nPosY + y) < nFieldHeight)
            {
                if(tetrominos[nTetromino][ti] != L'.' && pField[fi] != 0)
                    return false;
            }
            
        }

    return true;
}

int main()
{
    initTetrominos();
    initField();

    // Create console handle
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    // IDK how to get the console windows size...
    // Get console window's size
    // CONSOLE_SCREEN_BUFFER_INFO csbi;
    // GetConsoleScreenBufferInfo(hConsole, &csbi);
    // nScreenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    // nScreenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // // Check window's size
    // if (nScreenWidth < nFieldWidth + xbias || nScreenHeight < nFieldHeight + ybias)
    // {
    //     if (haveCheckSize) continue;
    //     printf("Invalid window size!\nPlease go setting and restart\n");
    //     printf("Current: %d column * %d row\n", nScreenWidth, nScreenHeight);
    //     printf("Require: %d column * %d row (at least)\n", nFieldWidth + xbias, nFieldHeight + ybias);
    //     haveCheckSize = true;
    // }
    // else
    // {
    //     break;
    // }

    // Create screen buffer (IDK how to by the console size) and initialize it
    wchar_t *screen = new wchar_t[nScreenHeight * nScreenWidth + 1];
    screen[nScreenHeight * nScreenWidth] = L'\0';  // set the end for printing
    for (int i = 0; i < nScreenHeight * nScreenWidth; i++)
        screen[i] = L' ';

    // Game loop
    DWORD byteWritten;
    bool gameOver = false;
    int biasX = 2;
    int biasY = 2;
    int curPiece = 0;
    // current position in the Field
    int curX = (nFieldWidth - 4)/2;
    int curY = 0;
    int curRotate = 0;
    bool keyDowns[4];
    bool rotateHold = false;
    int speed = 20;
    int speedCounter = 0;
    vector<int> lines = {}; // those lines we got
    int score = 0;
    int pieceCount = 0;

    while(!gameOver)
    {
        // GAME TIMEING ======================

        this_thread::sleep_for(50ms);

        // USER INPUT ========================

        for (int i = 0; i < 4; i++)
            keyDowns[i] = (0x8000 & (GetKeyState("WASD"[i] | GetKeyState("wasd"[i]))));  //0x8000 means key down

        // GAME LOGIC ========================

        // W/w for rotate
        if (keyDowns[0])
        { 
            if (!rotateHold && DoesPieceFit(curPiece, curRotate + 1, curX, curY))
                curRotate = (curRotate + 1)%4;

            rotateHold = true;
        }
        else
        {
            rotateHold = false;
        }

        // A/a move left
        if (keyDowns[1] && DoesPieceFit(curPiece, curRotate, curX - 1, curY))
            curX -= 1;
        
        // S/s move down
        if (keyDowns[2] && DoesPieceFit(curPiece, curRotate, curX, curY+1))
            curY += 1;
        
        // D/d move right
        if (keyDowns[3] && DoesPieceFit(curPiece, curRotate, curX + 1, curY))
            curX += 1;
        
        // Fall down
        if (speedCounter == speed)
        {
            speedCounter = 0;
            pieceCount += 1;
            if(pieceCount % 50 == 0)  // speed up every 50 fall
                speed -= speed > 0 ? 1 : 0;

            if (DoesPieceFit(curPiece, curRotate, curX, curY + 1))
            {
                curY++;
            }
            else  // stuck
            {
                // Put the peices into Field
                for (int x = 0; x < 4; x++)
                    for (int y = 0; y < 4; y++)
                        if (tetrominos[curPiece][Rotate(x, y, curRotate)] != L'.')
                            pField[(curY + y)*nFieldWidth + (curX + x)] = curPiece + 1;
            
                // Check if there is any line
                for (int y = 0; y < 4; y++)
                {
                    if (curY + y < nFieldHeight - 1)
                    {
                        bool hasLine = true;
                        for (int x = 1; x < nFieldWidth - 1; x++)
                            hasLine &= pField[(curY + y)*nFieldWidth + x] != 0;

                        if (hasLine)
                        {
                            for (int x = 1; x < nFieldWidth - 1; x++)
                                pField[(curY + y)*nFieldWidth + x] = 8;

                            lines.emplace_back(curY + y);
                        }
                    }

                }

                // score
                pieceCount++;
                score += 25;
                // exponential line award
                if (!lines.empty())
                    score += (1 << (lines.size() - 1)) * 100;

                // Choose next piece
                curPiece = rand() % 7;
                curX = (nFieldWidth - 4) / 2;
                curY = 0;
                curRotate = 0;

                // Check Gameover
                gameOver = !DoesPieceFit(curPiece, curRotate, curX, curY);
            }
        }
        else
        {
            speedCounter++;
        }

        // OUTPUT ============================

        // Draw Field
        for (int x = 0; x < nFieldWidth; x++)
            for (int y = 0; y < nFieldHeight; y++)
                screen[(y + biasY)*nScreenWidth + (x + biasX)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]];
        
        // Draw line disappear animation cheekily
        if (!lines.empty())
        {
            WriteConsoleOutputCharacterW(hConsole, screen, wcslen(screen), {0,0}, &byteWritten);
            this_thread::sleep_for(350ms);

            // Delete lines
            for (int lineY: lines)
                for (int y = lineY; y > 0; y--)
                    for (int x = 1; x < nFieldWidth - 1; x++)
                        pField[y*nFieldWidth + x] = pField[(y - 1)*nFieldWidth + x];
            
             // Draw Field
            for (int x = 0; x < nFieldWidth; x++)
                for (int y = 0; y < nFieldHeight; y++)
                    screen[(y + biasY)*nScreenWidth + (x + biasX)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]];

            lines.clear();
        }

        // Draw Current Piece
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                if (tetrominos[curPiece][Rotate(x, y, curRotate)] == L'X')
                    screen[(curY + y + biasY)*nScreenWidth + (curX + x + biasX)] = L'A' + curPiece;
        
        // Draw score
        int written = swprintf_s(&screen[2*nScreenWidth + nFieldWidth + biasX + 6], nScreenWidth - (nFieldWidth + biasX + 6 + 8), L"SCORE: %8d", score);
        screen[written + 2*nScreenWidth + nFieldWidth + biasX + 6] = L'G';  // replace the \0 cause by swprintf_s at the end of written string
        screen[wcslen(screen)] = L' ';  //  replace the \0 cause by swprintf_s at the end on given size

        // Show Frame
        WriteConsoleOutputCharacterW(hConsole, screen, wcslen(screen), {0,0}, &byteWritten);
       
        // cerr << byteWritten << endl;
    };

    CloseHandle(hConsole);
    printf("GAME OVER\n");
    printf("SCORE: %d\n", score);

    // printf("curPiece: %d\n", curPiece);
    // printf("curPos: (%d, %d)\n", curX, curY);
    // printf("curRotate: %d\n", curRotate);

    system("pause");

    return 0;
}