#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <memory>
#include "AConsole.h"
#include "ProcessConsole.h"

const std::string MAIN_CONSOLE = "MAIN_CONSOLE";
const std::string PROCESS_CONSOLE = "PROCESS_CONSOLE";
const std::string MARQUEE_CONSOLE = "MARQUEE_CONSOLE";
typedef std::string String;

class ConsoleManager {
public:
	typedef std::unordered_map<String, std::shared_ptr<AConsole>> ConsoleTable;

	static ConsoleManager* getInstance();

	void createConsole(String screenName);
	void createConsoleWOswitch(String screenName);
	void switchToConsole(String screenName);
	void showListOfConsoles();

	void switchToConsoleFromProcess(String screenName);
	std::shared_ptr<ProcessConsole> getProcessConsole(String screenName);
	std::vector<String> getProcessConsoleNames();
	void printStringList(std::vector<String>& stringList);
	void printProcessConsolesInfo();

	const ConsoleTable& getConsoleTable() const;

	void printSmth();

	bool running();

	void displayNvidiaSMI();
	void printProcessesTable2();
	std::string truncateTextWithEllipsis(const std::string& text, size_t max_width);

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

