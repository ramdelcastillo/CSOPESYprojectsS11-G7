#pragma once

#include "AConsole.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <Windows.h>

typedef std::string String;

class MainConsole : public AConsole
{
public:
    MainConsole();
    void colorFG(int nColor);
    void resetColor();
    void enterCommand();
    void clearScreen();
    void printColored(String str, int colorCode);
    void header();
    
    void onEnabled() override;
    void display() override;
    void process() override;

    std::vector<std::string> tokenize(String command);
    void processCommand(String command);
   
private:
};

