#include "ProcessConsole.h"
#include "ConsoleManager.h"
#include <iostream>
#include <iomanip>
#include <ctime>

ProcessConsole::ProcessConsole(std::shared_ptr<AttachedProcess> myProcess)
    : AConsole(myProcess->getProcessName()), thisProcess(myProcess) {
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
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        if (command == "exit") {
            this->isRunning = false;
            consoleManager->switchToConsoleFromProcess(MAIN_CONSOLE); 
        }
        else if (command == "process-smi") {
            if (thisProcess->getProcessState() == AttachedProcess::FINISHED) {
                std::cout << "\nProcess name: " << thisProcess->getProcessName() << "\n";
                std::cout << "ID: " << thisProcess->getPid() << "\n";
                std::time_t startTime = thisProcess->getStartTime();
                std::tm now;

                localtime_s(&now, &startTime);
                std::cout << "Created on: " << std::put_time(&now, "%m/%d/%Y, %I:%M:%S %p") << "\n";
                std::cout << "--------------------------\n";
                std::cout << "Finished!\n" << std::endl;
            }
            else {
                std::cout << "\n--------------------------\n";
                std::cout << "Process name: " << thisProcess->getProcessName() << "\n";
                std::cout << "ID: " << thisProcess->getPid() << "\n";
                std::cout << "Current instruction line: " << thisProcess->getCurrentLineOfInstruction() << "\n";
                std::cout << "Lines of code: " << thisProcess->getLinesOfCode() << "\n";
                std::time_t startTime = thisProcess->getStartTime();
                std::tm now;

                localtime_s(&now, &startTime);
                std::cout << "Created on: " << std::put_time(&now, "%m/%d/%Y, %I:%M:%S %p") << "\n";
                std::cout << "--------------------------\n";
            }
        }
        else if (command == "") {
            continue;
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
    std::cout << "Process name: " << thisProcess->getProcessName() << "\n";
    std::cout << "ID: " << thisProcess->getPid() << "\n";
    std::cout << "Current instruction line: " << thisProcess->getCurrentLineOfInstruction() << "\n";
    std::cout << "Lines of code: " << thisProcess->getLinesOfCode() << "\n";
    /*std::cout << "Process S"*/

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

std::shared_ptr<AttachedProcess> ProcessConsole::getAttachedProcess()
{
    return thisProcess;
}
