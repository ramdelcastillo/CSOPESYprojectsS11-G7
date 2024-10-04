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

    if (tokens.size() == 0) {
        std::cout;
    }
    else if (tokens.size() == 1) {
        if (command == "clear") {
            clearScreen();
            header();
        }
        else if (command == "initialize") {
            printColored(command, 0x06);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "screen") {
            printColored(command, 0x06);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "scheduler-test") {
            printColored(command, 0x06);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "scheduler-stop") {
            printColored(command, 0x06);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "report-util") {
            printColored(command, 0x06);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "nvidia-smi") {
            consoleManager->displayNvidiaSmi();
		}
        else if (command == "exit") {
            exit(0);
        }
        else if (command == "show") {
            consoleManager->showListOfConsoles(); 
        }
        else {
            printColored(command, 0x06);
            std::cout << " command unrecognized. Enter a valid command.\n" << std::endl;
        }
    }
    else if (tokens.size() == 3) { 
        std::string option = tokens[1];
        std::string processName = tokens[2];

        if (processName.empty()) {
            std::cout << "Command not recognized! Please provide a valid process name." << std::endl;
        }
        else {
            if (option == "-s") {
                consoleManager->createConsole(processName);
            }
            else if (option == "-r") {
                consoleManager->switchToConsole(processName);
            }
            else {
                std::cout << "Invalid option! Use '-s' to create or '-r' to switch." << std::endl;
            }
        }
    }
    else if (tokens.size() == 2) {
        std::string option = tokens[1];
        if (option == "-ls") {
            consoleManager->showListOfConsoles();
        }
    }

}