#include <iostream>
#include <windows.h> 
#include <locale>
#include <io.h>
#include <fcntl.h>

#include "Game.h"

int main() {
    // Set console output and input codepages to UTF-8 (65001).
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::locale::global(std::locale("")); // Use user's default locale settings
    _setmode(_fileno(stdout), _O_U8TEXT); // Set stdout (output stream) to UTF-8 text mode
    _setmode(_fileno(stdin), _O_U8TEXT);  // Set stdin (input stream) to UTF-8 text mode

    // Set the console window title using the wide character version
    SetConsoleTitleW(L"Space Invaders C++ Console Game");

    Game myGame; 
    myGame.run(); 

    return 0; 
}