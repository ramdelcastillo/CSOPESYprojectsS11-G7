#include "ConsoleManager.h"
#include "AConsole.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "MarqConsole.h"

#include <iomanip>
#include <random>



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
	const std::shared_ptr<MarqConsole> marqueeConsole = std::make_shared<MarqConsole>();

	this->consoleTable[MAIN_CONSOLE] = mainConsole;
	this->consoleTable[MARQUEE_CONSOLE] = marqueeConsole;
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

void ConsoleManager::createConsoleWOswitch(String screenName)
{
    if (this->consoleTable.find(screenName) != this->consoleTable.end()) {
        std::cerr << "Screen name " << screenName << " already exists. Please use a different name." << std::endl;
        return;
    }

    std::shared_ptr<AttachedProcess> process = std::make_shared<AttachedProcess>(screenName, 0, 0);

    std::shared_ptr<ProcessConsole> newConsole = std::make_shared<ProcessConsole>(process);

    this->consoleTable[screenName] = newConsole;
}

std::shared_ptr<ProcessConsole> ConsoleManager::getProcessConsole(String screenName)
{
    auto it = consoleTable.find(screenName);
    if (it != consoleTable.end() && screenName != MAIN_CONSOLE && screenName != MARQUEE_CONSOLE) {

        return std::dynamic_pointer_cast<ProcessConsole>(it->second);
    }
    return nullptr;
}

std::vector<String> ConsoleManager::getProcessConsoleNames()
{
    std::vector<String> processConsoleNames;

    for (const auto& console : consoleTable) {
        if (console.first != MAIN_CONSOLE && console.first != MARQUEE_CONSOLE) {
            processConsoleNames.push_back(console.first);
        }
    }

    return processConsoleNames;
}

void ConsoleManager::printStringList(std::vector<String>& stringList)
{
    if (stringList.empty()) {
        std::cout << "The list is empty." << std::endl;
        return;
    }

    std::cout << "List of Consoles:" << std::endl;
    for (const auto& str : stringList) {
        std::cout << " - " << str << std::endl;
    }
}

void ConsoleManager::printProcessConsolesInfo()
{
    std::vector<String> processConsoleNames = getProcessConsoleNames();

    if (processConsoleNames.empty()) {
        std::cout << "No process consoles available." << std::endl;
        return;
    }

    for (const auto& name : processConsoleNames) {
        auto processConsole = getProcessConsole(name);
        if (processConsole && processConsole->getAttachedProcess()) {
            const auto& attachedProcess = processConsole->getAttachedProcess();
            std::cout << "Process Console: " << name
                << ", Name: " << attachedProcess->getName()
                << ", ID: " << attachedProcess->getID() << std::endl;
        }
    }
}

const ConsoleManager::ConsoleTable& ConsoleManager::getConsoleTable() const {
    return consoleTable;
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
		if (console.first != MAIN_CONSOLE && console.first != MARQUEE_CONSOLE) {
			std::cout << " - " << console.first << std::endl;
		}
	}

	std::vector<String> restrictedConsoles = { "MAIN_CONSOLE", "MARQUEE_CONSOLE"};

	std::cout << "\nRestricted Consoles:" << std::endl;
	for (const auto& restrictedConsole : restrictedConsoles) {
		if (consoleTable.find(restrictedConsole) != consoleTable.end()) {
			std::cout << " - " << restrictedConsole << "\n" << std::endl;
		}
	}
}

void ConsoleManager::colorFG(int nColor) {
    HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hc, nColor);
}

void ConsoleManager::resetColor() {
    HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hc, 0x07);
}

void ConsoleManager::printColored(String str, int colorCode) {
    colorFG(colorCode);

    std::cout << str;

    resetColor();
} 


void ConsoleManager::displayNvidiaSmi() {
    std::time_t currentTime = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &currentTime);
    std::cout << std::put_time(&localTime, "%A %B %d %H:%M:%S %Y") << std::endl;
	colorFG(0x03);
    std::string header =
        "+-----------------------------------------------------------------------------------------+\n"
        "| NVIDIA-SMI 561.09                 Driver Version: 561.09         CUDA Version: 12.6     |\n"
        "|-----------------------------------------+------------------------+----------------------+\n"
        "| GPU  Name                  Driver-Model | Bus-Id          Disp.A | Volatile Uncorr. ECC |\n"
        "| Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |\n"
        "|                                         |                        |               MIG M. |\n"
        "|=========================================+========================+======================|\n"
        "|   0  NVIDIA GeForce RTX 3050 ...  WDDM  |   00000000:01:00.0 Off |                  N/A |\n"
        "| N/A   53C    P8             10W /   35W |       7MiB /   4096MiB |      1%      Default |\n"
        "|                                         |                        |                  N/A |\n"
        "+-----------------------------------------+------------------------+----------------------+\n";

    std::cout << header;

    std::cout << "\n+-----------------------------------------------------------------------------------------+\n"
        << "| Processes:                                                                              |\n"
        << "|  GPU   GI   CI        PID   Type   Process name                              GPU Memory |\n"
        << "|        ID   ID                                                               Usage      |\n"
        << "|=========================================================================================|\n";
		resetColor();

    if (this->consoleTable.size() == 1) {
        std::cout << "|    0  N/A  N/A        N/A   N/A    N/A                                       N/A        |\n"
            << "|=========================================================================================|\n";
    }
    else {
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1000, 9999);

        for (const auto& console : consoleTable) {
            if (console.first != MAIN_CONSOLE && console.first != MARQUEE_CONSOLE) {
                std::string processName = console.first;
                if (processName.length() >= 39) {
                    processName = "..." + processName.substr(processName.length() - 36);
                }
                std::cout << "|    0  N/A  N/A       " << std::setw(4) << std::setfill(' ') << std::right << dis(gen)
                        << "    C+G   " << std::setw(39) << std::setfill(' ') << std::left << processName << "   N/A        |\n";
            }
        }

        std::cout << "|=========================================================================================|\n";
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
