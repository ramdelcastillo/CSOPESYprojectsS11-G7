#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <conio.h>
#include <windows.h>
#include "MarqConsole.h"
#include "ConsoleManager.h"

MarqConsole::MarqConsole()
    : AConsole("MARQUEE_CONSOLE"), running(true) {
}

MarqConsole::MarqConsole(const std::string& text)
    : AConsole("MARQUEE_CONSOLE"), marqueeText(text), running(true) {
}

void MarqConsole::process() {
	running.store(true);

    marqueeThread = std::thread(&MarqConsole::displayMarquee, this);
    inputThread = std::thread(&MarqConsole::handleInput, this);

    exitConsole();
}

void MarqConsole::displayMarquee() {
    int x = 0;
    int y = 3;
    int xDirection = 1;
    int yDirection = 1;
    const int delayMs = 1000 / 60;

    while (running) {
        clearScreen();
        setCursorPosition(x, y);
        std::cout << marqueeText << std::flush;

        x += xDirection;
        y += yDirection;

        if (x <= 0 || x + static_cast<int>(marqueeText.length()) >= screenWidth) {
            xDirection *= -1;
        }
        if (y <= 3 || y >= screenHeight - 10) {
            yDirection *= -1;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
}


void MarqConsole::handleInput() {
    while (running) {
        if (_kbhit()) {
            char c = _getch();
            if (c == '\r') {
                if (userInput == "exit") {
					running = false;
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
    }
}

void MarqConsole::setCursorPosition(int x, int y) {
    COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void MarqConsole::clearScreen() {
    system("cls");
    std::cout << "****************************************\n";
    std::cout << "*    Displaying a Marquee console!     *\n";
    std::cout << "****************************************\n";
    displayCommands();
}

void MarqConsole::displayCommands() {
    setCursorPosition(0, screenHeight - 10);
    std::cout << "Type your text here (type 'exit' to quit): " << userInput << std::endl;

    for (size_t i = 0; i < commands.size(); ++i) {
        setCursorPosition(0, screenHeight - 9 + static_cast<int>(i));
        std::cout << "Entered command: " << commands[i] << std::endl;
    }
}

void MarqConsole::onEnabled() {
    process();
}

void MarqConsole::exitConsole() {
    if (marqueeThread.joinable()) {
        marqueeThread.join();
    }
    if (inputThread.joinable()) {
        inputThread.join();
    }

    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    consoleManager->switchToConsole(MAIN_CONSOLE);
}

void MarqConsole::display() {
    static int position = 0;
    static int direction = 1;

    position += direction;
    if (position == 0 || position == screenWidth - 10) {
        direction *= -1;
    }

    for (int i = 0; i < position; ++i) {
        std::cout << " ";
    }
    std::cout << "Marquee Text" << std::endl;
}