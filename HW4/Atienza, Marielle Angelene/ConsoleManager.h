#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include "AConsole.h"

const std::string MAIN_CONSOLE = "MAIN_CONSOLE";
const std::string PROCESS_CONSOLE = "PROCESS_CONSOLE";
typedef std::string String;

class ConsoleManager {
public:
	typedef std::unordered_map<String, std::shared_ptr<AConsole>> ConsoleTable;

	static ConsoleManager* getInstance();

	void createConsole(String screenName);
	void switchToConsole(String screenName);
	void showListOfConsoles();
	void showCustomizedCLIlayout();
	void switchToConsoleFromProcess(String screenName);
	std::string getCurrentDateTime();
	void printSmth();

	bool running();

private:
	ConsoleTable consoleTable;
	std::shared_ptr<AConsole> currentConsole;
	std::shared_ptr<AConsole> previousConsole;

	ConsoleManager();
	~ConsoleManager() = default;
	ConsoleManager(ConsoleManager const&) {}
	ConsoleManager& operator=(ConsoleManager const&) {};

	static ConsoleManager* instance;

	bool isRunning = true;
};