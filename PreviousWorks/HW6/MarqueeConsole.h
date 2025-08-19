#pragma once
#include "AConsole.h"
#include <thread>
#include <atomic>
#include <string>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <chrono>

class MarqueeConsole : public AConsole {
public:
    MarqueeConsole();
    void onEnabled() override;
    void display() override;
    void process() override;

    void marqueeDisplay();
    void inputCommand();
    void setC(int x, int y, const std::string& text);
    void displayHeader();
    void displayFooter();
    void displayCommands(const std::vector<std::string>& commands);
    void consoleReset();
    int getFPS();
   
private:
    std::string userInput;
    std::vector<std::string> commands;
    int screenWidth = 120;
    int screenHeight = 30;
    int fps = 60;
    int pollingRate = 1;
    std::string marqueeText = "Hello world in marquee!";
    std::atomic<bool> running{ true };
    std::thread marqueeThread;
    std::thread commandThread;
};

