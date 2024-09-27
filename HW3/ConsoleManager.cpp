#include "ConsoleManager.h"
#include "AConsole.h"
#include "MainConsole.h"
#include "ProcessConsole.h"

/*

Contributors:
Atienza, Angelene
Del Castillo, Jose
Rivera, Jose

*/

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

	const std::shared_ptr<MainConsole> mainConsole = std::make_shared<MainConsole>();

	this->consoleTable[MAIN_CONSOLE] = mainConsole;

	//std::cout << "INITIALIZED CONSOLE MANAGER";
}

void ConsoleManager::createConsole(String screenName)
{
	if (this->consoleTable.find(screenName) != this->consoleTable.end()) {
		std::cerr << "Screen name " << screenName << " already exists. Please use a different name." << std::endl;
		return;
	}

	std::shared_ptr<AttachedProcess> process = std::make_shared<AttachedProcess>(screenName, 0, 0);

	std::shared_ptr<ProcessConsole> newConsole = std::make_shared<ProcessConsole>(process);

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

void ConsoleManager::printSmth()
{
	std::cout << "HERE";
}

bool ConsoleManager::running()
{
	return this->isRunning;
}

