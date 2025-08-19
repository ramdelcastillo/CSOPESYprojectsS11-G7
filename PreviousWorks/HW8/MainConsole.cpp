#include <iostream>
#include "MainConsole.h"
#include "ConsoleManager.h"

MainConsole::MainConsole()
    : AConsole("MAIN_CONSOLE") {
}

void MainConsole::colorFG(int nColor) {
    HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hc, nColor);
}

void MainConsole::resetColor() {
    HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hc, 0x07);
}

void MainConsole::enterCommand() {
    std::cout << "Enter a command: ";
}

void MainConsole::clearScreen() {
    system("CLS");
}

void MainConsole::printColored(String str, int colorCode) {
    colorFG(colorCode);

    std::cout << str;

    resetColor();
}

void MainConsole::header() {
    colorFG(0x03);
    std::cout << "   _____  _____  ____  _____  ______  _______     __" << std::endl;
    std::cout << "  / ____|/ ____|/ __ ||  __ ||  ____|/ ____| |   / /" << std::endl;
    std::cout << " | |    | (___ | |  | | |__) | |__  | (___  | |_/ / " << std::endl;
    std::cout << " | |     |___ || |  | |  ___/|  __|  |___ |  |   /  " << std::endl;
    std::cout << " | |____ ____) | |__| | |    | |____ ____) |  | |   " << std::endl;
    std::cout << " |______|_____/ |____/|_|    |______|_____/   |_|   " << std::endl;
    colorFG(0x02);
    std::cout << "Hello! Welcome to CSOPESY commandline!" << std::endl;
    colorFG(0x06);
    std::cout << "Type 'exit' to quit, 'clear' to clear the screen." << std::endl;
    resetColor();
}

void MainConsole::onEnabled()
{
    display();
    process();
}

void MainConsole::display()
{
    header();
}

void MainConsole::process() {
    String command;
    enterCommand();
    std::getline(std::cin, command);
    processCommand(command);
}

std::vector<std::string> MainConsole::tokenize(String command)
{
    std::vector<String> tokens;
    std::stringstream ss(command);
    std::string token;

    while (ss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

void MainConsole::processCommand(String command) {
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    std::vector<std::string> tokens = tokenize(command);

    if (!consoleManager->getIsFunctionalityInitialized()) {
        if (command == "initialize" || command == "in") {
            consoleManager->setIsFunctionalityInitialized(true);
            consoleManager->setupConfig();
            uint32_t nCpu = consoleManager->getConfigParser()->getNumCPU();
            std::string schedulingAlgo = consoleManager->getConfigParser()->getCurrentSchedulingAlgo(); 
            uint32_t quantumCycles = consoleManager->getConfigParser()->getQuantumCycles();
            uint32_t batchProcessFreq = consoleManager->getConfigParser()->getBatchProcessFreq();
            uint32_t minIns = consoleManager->getConfigParser()->getMinIns();
            uint32_t maxIns = consoleManager->getConfigParser()->getMaxIns();
            uint32_t delaysPerExecution = consoleManager->getConfigParser()->getDelaysPerExecution();
            uint32_t maxOverallMemory = consoleManager->getConfigParser()->getMaxOverallMemory();
            uint32_t memoryPerFrame = consoleManager->getConfigParser()->getMemoryPerFrame();
            uint32_t memoryPerProcess = consoleManager->getConfigParser()->getMemoryPerProcess();
            consoleManager->setupScheduler(nCpu, schedulingAlgo, quantumCycles, batchProcessFreq, minIns, maxIns, delaysPerExecution, maxOverallMemory, memoryPerFrame, memoryPerProcess);
            consoleManager->setupMemoryAllocator();
            clearScreen();
            header();
            std::cout << "OS is initialized.\n" << std::endl;
        }
        else if (command == "exit") {
            exit(0);
        }
        else {
            std::cout << "Please enter \"initialize\" or \"in\" to start the OS.\n" << std::endl;
        }
    }
    else {
        if (tokens.size() == 0) {
            std::cout;
        }
        else if (tokens.size() == 1) {
            if (command == "clear") {
                clearScreen();
                header();
            }
            else if (command == "exit") {
                exit(0);
            }
            else if (command == "rq") {
                consoleManager->getCPUScheduler()->printReadyQueue();
            }
            else if (command == "ru" || command == "report-util") {
                consoleManager->getCPUScheduler()->logRunningAndFinishedProcessesToFile();
            }
            else if (command == "st" || command == "scheduler-test") {
                consoleManager->getCPUScheduler()->start();
            }
            else if (command == "ss" || command == "scheduler-stop") {
                consoleManager->getCPUScheduler()->stop();
            }
            else if (command == "nvidia-smi") {
                consoleManager->displayNvidiaSMI();
                consoleManager->printProcessesTable2();
            }
            else if (command == "marquee") {
                consoleManager->switchToMarqueeConsoleFromMain(MARQUEE_CONSOLE);
            }
            else if (command == "c") {
            // just for checking CPU cycle count
                std::cout << consoleManager->getCPUScheduler()->getCPUCycles() << std::endl;
                time_t now = time(0);
                struct tm localTime;

                localtime_s(&localTime, &now);

                std::cout << std::put_time(&localTime, "%a %b %d %H:%M:%S %Y") << std::endl;
            }
            else if (command == "d") {
                consoleManager->getMemoryAllocator()->visualizeMemory();
             /*   std::cout << consoleManager->getConfigParser()->getMaxOverallMemory() << std::endl;
                std::cout << consoleManager->getConfigParser()->getMemoryPerProcess() << std::endl;
                std::cout << consoleManager->getConfigParser()->getMemoryPerFrame() << std::endl;*/
               
            }
            else if (command == "f") {
                auto baseAllocator = consoleManager->getMemoryAllocator();
                auto flatMemoryAllocator = std::dynamic_pointer_cast<FlatMemoryAllocator>(baseAllocator);
                flatMemoryAllocator->printMemory();
         

            }
            else if (command == "1") {
            // faster way of screen -ls
                consoleManager->getCPUScheduler()->printRunningAndFinishedProcesses();
            }
            else if (command == "2") {
            // faster way of checking amount of working cores
                std::cout << consoleManager->getCPUScheduler()->getAmountOfWorkingCores() << std::endl << std::endl;
            }
            else {
                std::cout << "Invalid command.\n" << std::endl;
            }
        }
        else if (tokens.size() == 2) {
            std::string screen = tokens[0];
            std::string option = tokens[1];
            if (screen == "screen" && option == "-ls") {
                consoleManager->getCPUScheduler()->printRunningAndFinishedProcesses();
            }
            else {
                std::cout << "Invalid command.\n" << std::endl;
            }
        }
        else if (tokens.size() == 3) {
            std::string screen = tokens[0];
            std::string option = tokens[1];
            std::string processName = tokens[2];
            if (screen == "screen" && option == "-s") {
                consoleManager->createProcessConsole(processName);
            }
            else if (screen == "screen" && option == "-r") {
                consoleManager->switchToProcessConsole(processName);
            }
            else {
                std::cout << "Invalid command.\n" << std::endl;
            }
        }
        else {
            std::cout << "Invalid command.\n" << std::endl;
        }
    }
}


