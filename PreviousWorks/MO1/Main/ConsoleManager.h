#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <memory>
#include "AConsole.h"
#include "ProcessConsole.h"
#include "CPUScheduler.h"
#include "ConfigParser.h"

const std::string MAIN_CONSOLE = "MAIN_CONSOLE";
const std::string PROCESS_CONSOLE = "PROCESS_CONSOLE";
const std::string MARQUEE_CONSOLE = "MARQUEE_CONSOLE";
typedef std::string String;

class ConsoleManager {
public:
	typedef std::unordered_map<String, std::shared_ptr<AConsole>> ConsoleTable;

	static ConsoleManager* getInstance();

	void createProcessConsole(String screenName);
	void createConsoleWOswitch(String screenName);
	void switchToProcessConsole(String screenName);
	void showListOfConsoles();

	void switchToMainConsoleFromProcessOrMarquee(String screenName);
	void switchToMarqueeConsoleFromMain(String screenName);
	std::shared_ptr<ProcessConsole> getProcessConsole(String screenName);
	std::vector<String> getProcessConsoleNames();
	void printProcessConsolesInfo();

	const ConsoleTable& getConsoleTable() const;
	bool running();

	void displayNvidiaSMI();
	void printProcessesTable2();
	std::string truncateTextWithEllipsis(const std::string& text, size_t max_width);
	bool getIsFunctionalityInitialized();
	void setIsFunctionalityInitialized(bool in);
	void setupScheduler(uint32_t nCores, std::string currentSchedulingAlgo, uint32_t quantumCycles, uint32_t batchProcessFreq, uint32_t minIns, uint32_t maxIns, uint32_t delaysPerExecution);
	std::shared_ptr<CPUScheduler> getCPUScheduler();
	int getMadePid();
	void addOneToMadePid();
	void addToConsoleTable(std::string screenName, std::shared_ptr<ProcessConsole> newConsole);
	std::shared_ptr<ConfigParser> getConfigParser();
	void setupConfig();

private:
	int madePid = -1;
	ConsoleTable consoleTable;
	std::shared_ptr<ConfigParser> configParser;
	std::shared_ptr<AConsole> currentConsole;
	std::shared_ptr<AConsole> previousConsole;

	ConsoleManager();
	~ConsoleManager() = default;
	ConsoleManager(ConsoleManager const&) {}
	ConsoleManager& operator=(ConsoleManager const&) {};

	static ConsoleManager* instance;
	std::shared_ptr<CPUScheduler> cpuScheduler;
	bool isRunning = true;
	bool isFunctionalityInitialized = false;
};

