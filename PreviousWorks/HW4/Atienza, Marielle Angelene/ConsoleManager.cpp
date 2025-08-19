#include "ConsoleManager.h"
#include "AConsole.h"
#include "MainConsole.h"
#include "ProcessConsole.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <map>
#include <chrono>
#include <sstream> 

struct Process {
	int pid;
	std::string type;
	std::string name;
	std::string memoryUsage;
};

std::vector<Process> processes;

ConsoleManager* ConsoleManager::instance = nullptr;

ConsoleManager* ConsoleManager::getInstance()
{
	if (!instance) {
		instance = new ConsoleManager();
	}
	return instance;
}

ConsoleManager::ConsoleManager() {
	this->isRunning = true;

	srand(static_cast<unsigned>(time(0)));

	const std::shared_ptr<MainConsole> mainConsole = std::make_shared<MainConsole>();

	this->consoleTable[MAIN_CONSOLE] = mainConsole;
}

void ConsoleManager::createConsole(String screenName)
{
	if (this->consoleTable.find(screenName) != this->consoleTable.end()) {
		std::cerr << "Screen name " << screenName << " already exists. Please use a different name." << std::endl;
		return;
	}

	std::shared_ptr<AttachedProcess> process = std::make_shared<AttachedProcess>(screenName, 0, 0);

	std::shared_ptr<ProcessConsole> newConsole = std::make_shared<ProcessConsole>(process);

	processes.push_back({ process->getID(), "C+G", process->getName(), "N/A" });  // example memory usage

	this->consoleTable[screenName] = newConsole;
	std::cout << "Console " << screenName << " created successfully." << std::endl;

	switchToConsole(screenName);
}

void ConsoleManager::switchToConsole(String screenName)
{
	if (this->consoleTable.contains(screenName)) {
		system("cls");
		this->previousConsole = this->currentConsole;
		this->currentConsole = this->consoleTable[screenName];
		this->currentConsole->onEnabled();
	}
	else {
		std::cerr << "Screen name " << screenName << " not found. Was it initialized?" << std::endl;
	}
}

void ConsoleManager::switchToConsoleFromProcess(String screenName)
{
	if (screenName == "MAIN_CONSOLE") {
		system("cls");
		this->previousConsole = this->currentConsole;
		this->currentConsole = this->consoleTable[screenName];
		this->currentConsole->onEnabled();
	}
	else {
		std::cerr << "Invalid operation. You can only switch back to MAIN_CONSOLE from a process console." << std::endl;
	}
}

void ConsoleManager::showListOfConsoles()
{
	std::cout << "\n" << "List of Consoles:" << std::endl;
	for (const auto& console : consoleTable) {
		if (console.first != MAIN_CONSOLE) {
			std::cout << " - " << console.first << std::endl;
		}
	}

	std::vector<String> restrictedConsoles = { "MAIN_CONSOLE" };

	std::cout << "\nRestricted Consoles:" << std::endl;
	for (const auto& restrictedConsole : restrictedConsoles) {
		if (consoleTable.find(restrictedConsole) != consoleTable.end()) {
			std::cout << " - " << restrictedConsole << "\n" << std::endl;
		}
	}
}

void ConsoleManager::showCustomizedCLIlayout() {
	// Get current time
	std::string currentTime = getCurrentDateTime();
	std::cout << currentTime << std::endl;

	// Display GPU summary
	std::cout << "+-----------------------------------------------------------------------------------------+\n";
	std::cout << "| NVIDIA-SMI 556.13               Driver Version: 556.13           CUDA Version: 12.5     |\n";
	std::cout << "|-----------------------------------------+------------------------+----------------------+\n";
	std::cout << "| GPU  Name                  Driver-Model | Bus-Id          Disp.A | Volatile Uncorr. ECC |\n";
	std::cout << "| Fan  Temp  Perf           Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |\n";
	std::cout << "|                                         |                        |               MIG M. |\n";
	std::cout << "|=========================================+========================+======================|\n";
	std::cout << "|   0  NVIDIA GeForce MX130        WDDM   |   00000000:01:00.0 Off |                 N/A  |\n";
	std::cout << "| N/A    0C     P0            N/A / 200W  |     0MiB / 2048MiB     |     0%      Default  |\n";
	std::cout << "|                                                                                    N/A  |\n";
	std::cout << "+-----------------------------------------------------------------------------------------+\n\n";
	std::cout << "+-----------------------------------------------------------------------------------------+\n";
	std::cout << "| Processes:                                                                              |\n";
	std::cout << "|  GPU   GI   CI        PID   Type   Process name                              GPU Memory |\n";
	std::cout << "|        ID   ID                                                               Usage      |\n";
	std::cout << "|=========================================================================================|\n";

	if (processes.empty()) {
		std::cout << "| No running processes found                                                              |\n";
	}
	else {
		for (const auto& process : processes) {
			std::string displayName = process.name;

			if (displayName.length() > 37) {
				displayName = "..." + displayName.substr(displayName.length() - 35);
			}

			std::cout << "|   0    N/A  N/A      "
				<< std::left << std::setw(8) << (rand() % 9000) + 1000
				<< std::left << std::setw(6) << process.type
				<< std::left << std::setw(43) << displayName
				<< std::setw(10) << process.memoryUsage << "|\n";
		}
	}
	std::cout << "+-----------------------------------------------------------------------------------------+\n";
}

std::string ConsoleManager::getCurrentDateTime() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm timeinfo;
	localtime_s(&timeinfo, &now_time);
	std::stringstream ss;
	ss << std::put_time(&timeinfo, "%a %b %d %T %Y");
	return ss.str();
}

void ConsoleManager::printSmth()
{
	std::cout << "HERE";
}

bool ConsoleManager::running()
{
	return this->isRunning;
}