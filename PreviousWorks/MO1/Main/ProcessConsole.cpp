#include "ProcessConsole.h"
#include "ConsoleManager.h"
#include <iostream>
#include <iomanip>
#include <ctime>

ProcessConsole::ProcessConsole(std::shared_ptr<AttachedProcess> attachedProcess)
    : AConsole(attachedProcess->getProcessName()), attachedProcess(attachedProcess), isRunning(false) {
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
            consoleManager->switchToMainConsoleFromProcessOrMarquee(MAIN_CONSOLE);
        }
        else if (command == "process-smi") {
            if (attachedProcess->getProcessState() == AttachedProcess::FINISHED) {
                std::cout << "\nProcess name: " << attachedProcess->getProcessName() << "\n";
                std::cout << "ID: " << attachedProcess->getPid() << "\n";
                std::time_t startTime = attachedProcess->getStartTime();
                std::tm now;

                localtime_s(&now, &startTime);
                std::cout << "Finished on: " << std::put_time(&now, "%m/%d/%Y, %I:%M:%S %p") << "\n";
                std::cout << "--------------------------\n";
                std::cout << "Finished!\n" << std::endl;
            }
            else {
                std::cout << "\n--------------------------\n";
                std::cout << "Process name: " << attachedProcess->getProcessName() << "\n";
                std::cout << "ID: " << attachedProcess->getPid() << "\n";
                std::cout << "Current instruction line: " << attachedProcess->getCurrentLineOfInstruction() << "\n";
                std::cout << "Lines of code: " << attachedProcess->getLinesOfCode() << "\n";
                std::time_t startTime = attachedProcess->getStartTime();
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
            std::cout << "Unrecognized command: " << command << "\n" << std::endl;
        }
    }
}

void ProcessConsole::displayHeader() {
    std::cout << "--------------------------\n";
    std::cout << "Process name: " << attachedProcess->getProcessName() << "\n";
    std::cout << "ID: " << attachedProcess->getPid() << "\n";
    std::cout << "Current instruction line: " << attachedProcess->getCurrentLineOfInstruction() << "\n";
    std::cout << "Lines of code: " << attachedProcess->getLinesOfCode() << "\n";

    std::time_t startTime = attachedProcess->getStartTime();
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
    return attachedProcess;
}
