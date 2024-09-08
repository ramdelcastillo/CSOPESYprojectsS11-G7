#include <iostream>
#include <string>
#include <windows.h>  

class OSemulator {
    public:
        std::string command;

        void colorFG(int nColor) {
            HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hc, nColor);
        }

        void resetColor() {
            HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hc, 0x07);
        }

        void enterCommand() {
            std::cout << "Enter a command: ";
        }

        void clearScreen() {
            system("CLS");
        }

        void printColored(std::string str, int colorCode) {
            colorFG(colorCode);

            std::cout << str;

            resetColor();
        }

        void mainProgram() {
            header();

            while (true) {
                enterCommand();

                std::getline(std::cin, getCommand());

                if (getCommand().empty()) {
                    continue;
                }
                else if (getCommand() == "clear") {
                    clearScreen();
                    header();
                }
                else if (getCommand() == "initialize") {
                    printColored(getCommand(), 0x06);
                    std::cout << " command recognized. Doing something.\n" << std::endl;
                }
                else if (getCommand() == "screen") {
                    printColored(getCommand(), 0x06);
                    std::cout << " command recognized. Doing something.\n" << std::endl;
                }
                else if (getCommand() == "scheduler-test") {
                    printColored(getCommand(), 0x06);
                    std::cout << " command recognized. Doing something.\n" << std::endl;
                }
                else if (getCommand() == "scheduler-stop") {
                    printColored(getCommand(), 0x06);
                    std::cout << " command recognized. Doing something.\n" << std::endl;
                }
                else if (getCommand() == "report-util") {
                    printColored(getCommand(), 0x06);
                    std::cout << " command recognized. Doing something.\n" << std::endl;
                }
                else if (getCommand() == "exit") {
                    exit(0);
                }
                else {
                    printColored(getCommand(), 0x06);
                    std::cout << " command unrecognized. Enter a valid command.\n" << std::endl;
                }
            }
        }

        void header() {
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

        std::string& getCommand() {
            return this->command;
        }

        void setCommand(std::string& newCommand) {
            this->command = newCommand;
        }
};

int main() {
    OSemulator os;
    os.mainProgram();

    return 0;
}
