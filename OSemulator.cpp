#include <iostream>
#include <string>

void colorFG(int nColor);
void resetColor();
void enterCommand();
void clearScreen();
void printColored(std::string str, int colorCode);
void mainProgram();
void header();


int main() {
    mainProgram();
    //std::exit(0);
    return 0;
}

void colorFG(int nColor) {
    std::cout << "\x1b[38;5;" << nColor << "m";
}

void resetColor() {
    std::cout << "\x1b[0m";
}

void enterCommand() {
    std::cout << "Enter a command: ";
}

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

void printColored(std::string str, int colorCode) {
    colorFG(colorCode);

    std::cout << str;

    resetColor();
}

void mainProgram() {
    std::string command;
    header();

    while (true) {
        enterCommand();

        std::getline(std::cin, command);

        if (command.empty()) {
            continue;
        }
        else if (command == "clear") {
            clearScreen();
            header();
        }
        else if (command == "initialize") {
            printColored(command, 220);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "screen") {
            printColored(command, 220);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "scheduler-test") {
            printColored(command, 220);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "scheduler-stop") {
            printColored(command, 220);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "report-util") {
            printColored(command, 220);
            std::cout << " command recognized. Doing something.\n" << std::endl;
        }
        else if (command == "exit") {
            break;
        }
        else {
            printColored(command, 220);
            std::cout << " command unrecognized. Enter a valid command.\n" << std::endl;
        }
    }
}

void header() {
    colorFG(50);
    std::cout << "   _____  _____  ____  _____  ______  _______     __" << std::endl;
    std::cout << "  / ____|/ ____|/ __ ||  __ ||  ____|/ ____| |   / /" << std::endl;
    std::cout << " | |    | (___ | |  | | |__) | |__  | (___  | |_/ / " << std::endl;
    std::cout << " | |     |___ || |  | |  ___/|  __|  |___ |  |   /  " << std::endl;
    std::cout << " | |____ ____) | |__| | |    | |____ ____) |  | |   " << std::endl;
    std::cout << " |______|_____/ |____/|_|    |______|_____/   |_|   " << std::endl;
    colorFG(10);
    std::cout << "Hello! Welcome to CSOPESY commandline!" << std::endl;
    colorFG(220);
    std::cout << "Type 'exit' to quit, 'clear' to clear the screen." << std::endl;
    resetColor();
}
