#pragma once
#include "AConsole.h"
#include <thread>
#include <atomic>
#include <string>
#include <windows.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include <chrono>

class MarqConsole : public AConsole {
public:
    MarqConsole();
    MarqConsole(const std::string& marqueeText);

    void onEnabled() override;
    void exitConsole();
    void display() override;
    void process() override;

private:
    void displayMarquee();
    void handleInput();
    void setCursorPosition(int x, int y);
    void clearScreen();
    void displayCommands();

	std::string marqueeText = "Hello world in marquee!";
    std::atomic<bool> running{ true };
    std::thread marqueeThread;
    std::thread inputThread;
    std::vector<std::string> commands;
    std::string userInput;

    static const int screenWidth = 120;
    static const int screenHeight = 30;
};
