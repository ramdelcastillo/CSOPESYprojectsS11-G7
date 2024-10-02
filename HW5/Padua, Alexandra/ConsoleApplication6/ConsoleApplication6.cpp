#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <conio.h> // For _kbhit and _getch on Windows
#include <windows.h>
#include <string>
#include <vector>

// MarqueeConsole class definition
class MarqueeConsole {
public:
    MarqueeConsole() : running(true) {
        // Initialize console size
        setConsoleSize(consoleWidth, consoleHeight);
        displayHeader();
    }

    void process() {
        // Start the marquee and command threads
        marqueeThread = std::thread(&MarqueeConsole::marqueeDisplay, this);
        commandThread = std::thread(&MarqueeConsole::marqueeCommand, this);
    }

    bool isRunning() {
        return running.load();
    }

    void setConsoleSize(int width, int height) {
        COORD bufferSize;
        bufferSize.X = width;
        bufferSize.Y = height;
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);

        SMALL_RECT windowSize;
        windowSize.Left = 0;
        windowSize.Top = 0;
        windowSize.Right = width - 1;
        windowSize.Bottom = height - 1;
        SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
    }

    void setCursor(int x, int y, const std::string& text) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        std::cout << text << std::flush;
    }

    void displayHeader() {
        setCursor(0, 0, "***********************************");
        setCursor(0, 1, "* Displaying a marquee console!   *");
        setCursor(0, 2, "***********************************");
    }

    void marqueeDisplay() {
        using namespace std::literals::chrono_literals;

        int x = 0, y = 3;
        int dx = 1, dy = 1;
        int frameDelay = 1000 / 60; // 60 FPS

        while (running.load()) {
            system("cls"); // Clear the console for each frame
            displayHeader();

            // Update the position of the marquee text
            setCursor(x, y, "Hello world in marquee!");

            // Update the coordinates for the next frame
            x += dx;
            y += dy;

            // Bounce the text off the walls
            if (x <= 0 || x + 20 >= consoleWidth) {
                dx = -dx;
            }
            if (y <= 3 || y >= consoleHeight - 3) {
                dy = -dy;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));
        }
    }

    void marqueeCommand() {
        // Clear the command input
        commands.clear();
        currentInput.clear();

        // Handle keyboard input in a separate thread
        std::thread inputThread(&MarqueeConsole::keyboardInputHandler, this, std::ref(commands), std::ref(currentInput));

        while (running.load()) {
            // Check if we need to clear the console
            if (running.load()) {
                system("cls"); // Clear the screen for the command input section
                setCursor(0, 23, "Enter command: " + currentInput);
                displayCommands(commands);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // Exit condition
                if (currentInput == "exit") {
                    running.store(false);  // Stop the threads
                }
            }
        }

        if (inputThread.joinable()) inputThread.join();
    }

    void keyboardInputHandler(std::vector<std::string>& commands, std::string& currentInput) {
        while (running.load()) {
            if (_kbhit()) {
                char key = _getch();
                if (key == '\r') {  // Enter key
                    if (!currentInput.empty()) {
                        commands.push_back(currentInput);  // Add command to history
                        currentInput.clear();  // Reset input
                    }
                }
                else if (key == '\b') {  // Backspace key
                    if (!currentInput.empty()) {
                        currentInput.pop_back();  // Remove the last character
                    }
                }
                else {
                    currentInput += key;  // Add the typed character
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60)); // Check for input at 60 FPS
        }
    }

    void displayCommands(const std::vector<std::string>& commands) {
        for (size_t i = 0; i < commands.size(); ++i) {
            setCursor(0, 25 + i, "Entered command: " + commands[i]);
        }
    }

    void stop() {
        // This method is called to stop the threads
        running.store(false);  // Stop both threads

        if (marqueeThread.joinable()) {
            marqueeThread.join();
        }
        if (commandThread.joinable()) {
            commandThread.join();
        }

        // Clear command history and user input
        commands.clear();
        currentInput.clear();
    }

private:
    std::thread marqueeThread;
    std::thread commandThread;
    std::atomic<bool> running;
    const int consoleWidth = 120;
    const int consoleHeight = 30;

    std::vector<std::string> commands;
    std::string currentInput;
};

// Main function
int main() {
    MarqueeConsole console;

    // Start the marquee display and command processing
    console.process();

    // Keep the main function alive until the marquee console stops
    while (console.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Call stop to clean up resources and clear input
    console.stop();

    return 0;
}