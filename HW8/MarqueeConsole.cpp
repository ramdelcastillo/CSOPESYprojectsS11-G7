#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <conio.h> 
#include <windows.h>
#include "MarqueeConsole.h"
#include "ConsoleManager.h"

MarqueeConsole::MarqueeConsole()
    : AConsole("MARQUEE_CONSOLE"), running(true) { 
}

void MarqueeConsole::onEnabled()
{
    consoleReset();
    running.store(true);
    process();
}

void MarqueeConsole::display()
{
    displayHeader();
    displayFooter();
}

void MarqueeConsole::process()
{
    running.store(true);  

    marqueeThread = std::thread(&MarqueeConsole::marqueeDisplay, this);
    commandThread = std::thread(&MarqueeConsole::inputCommand, this);

    if (marqueeThread.joinable()) {
        marqueeThread.join();  
    }
    if (commandThread.joinable()) {
        commandThread.join(); 
    } 

    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    consoleManager->switchToMainConsoleFromProcessOrMarquee(MAIN_CONSOLE);
}

void MarqueeConsole::marqueeDisplay()
{
    int x = 0, y = 3;
    int dx = 1, dy = 1;

    int frameDelay = 1000 / fps;

    while (running.load()) {
        system("cls");
        display();
        x += dx;
        y += dy;

        if (x <= 0 || x + marqueeText.length() >= screenWidth) {
            dx = -dx;
        }
        if (y <= 3 || y >= 19) {
            dy = -dy;
        }

        setC(x, y, marqueeText);

        setC(0, 0, "");

        std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));
    }
}

void MarqueeConsole::inputCommand()
{
    while (running.load()) {
        if (_kbhit()) { 
            char c = _getch(); 

            if (c == '\b') { 
                if (!userInput.empty()) {
                    userInput.pop_back();
                }
            }
            else if (c == '\r') {
                std::string userInputCopy = userInput;

                if (userInputCopy == "exit") {
                    running.store(false);
                }
                else {
                    commands.push_back(userInput);
                    userInput.clear();
                }
            }
            else {
                userInput += c; 
            }

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(pollingRate));
    }
}

void MarqueeConsole::setC(int x, int y, const std::string& text)
{
    COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) }; 
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);  
    std::cout << text << std::flush;  
}

void MarqueeConsole::displayHeader()
{
    setC(0, 0, "***********************************");
    setC(0, 1, "*Displaying a Marquee console!");
    setC(0, 2, "***********************************");
    setC(112, 0, "FPS: " + std::to_string(getFPS()));
}

void MarqueeConsole::displayFooter()
{
    setC(0, screenHeight - 10, "Type your text here, to exit, type 'exit': " + userInput);
    displayCommands(commands);
}

void MarqueeConsole::displayCommands(const std::vector<std::string>& commands)
{
    for (int i = 0; i < commands.size(); ++i) {
        setC(0, screenHeight - 9 + i, "Entered command: " + commands[i]); 
    }
}

void MarqueeConsole::consoleReset()
{
    running.store(false);

    if (marqueeThread.joinable()) {
        marqueeThread.join();
    }
    if (commandThread.joinable()) {
        commandThread.join();
    }

    commands.clear();
    userInput.clear();
}

int MarqueeConsole::getFPS()
{
    return fps;
}





