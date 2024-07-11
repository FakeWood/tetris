#include <iostream>

using namespace std;

wstring tetromino[7];

int main()
{
    // use 1-d array rather than 2-d array for easy rotation
    tetromino[0].append(L"..X.")
                .append(L"..X.")
                .append(L"..X.")
                .append(L"..X.");

    tetromino[1].append(L"..X.")
                .append(L".XX.")
                .append(L".X..")
                .append(L"....");

    tetromino[2].append(L".X.")
                .append(L".XX.")
                .append(L"..X.")
                .append(L"....");
    
    tetromino[3].append(L"....")
                .append(L".XX.")
                .append(L".XX.")
                .append(L"....");
    
    tetromino[4].append(L"..X.")
                .append(L".XX.")
                .append(L"..X.")
                .append(L"....");
    
    tetromino[5].append(L"....")
                .append(L".XX.")
                .append(L"..X.")
                .append(L"..X.");

    tetromino[6].append(L"....")
                .append(L".XX.")
                .append(L".X..")
                .append(L".X..");
}