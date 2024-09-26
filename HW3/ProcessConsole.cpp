#include "ProcessConsole.h"
#include "ConsoleManager.h"
#include <iostream>
#include <iomanip>
#include <ctime>

/*
Contributors:
Del Castillo, Jose
Padua, Alexandra
*/

ProcessConsole::ProcessConsole(std::shared_ptr<AttachedProcess> myProcess)
    : AConsole("PROCESS_SCREEN_" + myProcess->getName()), thisProcess(myProcess) {
}

void ProcessConsole::onEnabled()
{
    this->isRunning = true;
    display();
    process();
}

void ProcessConsole::display()
{
    std::cout << "Process Display" << std::endl;
    displayHeader();
}

void ProcessConsole::process()
{
    std::string command;
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    while (this->isRunning) {
        std::cout << "Enter command (type 'exit' to return to main menu): ";
        std::getline(std::cin, command);

        if (command == "exit") {
            this->isRunning = false;
            consoleManager->switchToConsoleFromProcess(MAIN_CONSOLE); 
        }
        else {
            std::cout << "Unrecognized command: " << command << std::endl;
        }
    }
}

void ProcessConsole::displayHeader() {
    if (!thisProcess) {
        std::cout << "No process available.\n";
        return;
    }
    std::cout << "--------------------------\n";
    std::cout << "Process name: " << thisProcess->getName() << "\n";
    std::cout << "ID: " << thisProcess->getID() << "\n";
    std::cout << "Current instruction line: " << thisProcess->getCurrentInstruction() << "\n";

    std::time_t startTime = thisProcess->getStartTime();
    std::tm now;

    localtime_s(&now, &startTime);
    std::cout << "Created on: " << std::put_time(&now, "%m/%d/%Y, %I:%M:%S %p") << "\n";
    std::cout << "--------------------------\n";
}

bool ProcessConsole::running()
{
    return this->isRunning;
}
