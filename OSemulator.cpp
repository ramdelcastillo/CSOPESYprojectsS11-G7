#include <iostream>

void colorFG(int nColor);
void resetColor();
void header();

int main() {
    header();
    
    return 0;
}

void colorFG(int nColor) {
    std::cout << "\x1b[38;5;" << nColor << "m";
}

void resetColor() {
    std::cout << "\x1b[0m";
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
    std::cout << "Enter a command: " << std::endl;
}