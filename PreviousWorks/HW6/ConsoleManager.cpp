#include "ConsoleManager.h"
#include "AConsole.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "MarqueeConsole.h"
#include "CPUWorker.h"
#include "CPUScheduler.h"

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
    const std::shared_ptr<MarqueeConsole> marqueeConsole = std::make_shared<MarqueeConsole>();

	this->consoleTable[MAIN_CONSOLE] = mainConsole;
    this->consoleTable[MARQUEE_CONSOLE] = marqueeConsole;

	//std::cout << "INITIALIZED CONSOLE MANAGER";
}

void ConsoleManager::createConsole(String screenName)
{
	if (this->consoleTable.find(screenName) != this->consoleTable.end()) {
		std::cerr << "Screen name " << screenName << " already exists. Please use a different name." << std::endl;
		return;
	}

    this->madePid++;

	std::shared_ptr<AttachedProcess> process = std::make_shared<AttachedProcess>(this->madePid, screenName);
    
   /* process->create100PrintCommands();*/

    //process->setProcessState("FINISHED");

	std::shared_ptr<ProcessConsole> newConsole = std::make_shared<ProcessConsole>(process);

    getCPUScheduler()->addProcessToQueue(process);

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

    this->madePid++;

    std::shared_ptr<AttachedProcess> process = std::make_shared<AttachedProcess>(screenName, this->madePid, 0);

    std::shared_ptr<ProcessConsole> newConsole = std::make_shared<ProcessConsole>(process);

    this->consoleTable[screenName] = newConsole;

 /*   getCPUScheduler()->addProcessToQueue(process);*/

    /*std::cout << "Console " << screenName << " created successfully." << std::endl;*/
}

void ConsoleManager::switchToConsole(String screenName) 
{ //to-do: make sure it cant switch to MAIN_CONSOlE and MARQUEE_CONSOLE
  //to-do: if the process that youre switching to is AttachedProcess::FINISHED go to else statement also
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
                << ", Name: " << attachedProcess->getProcessName()
                << ", ID: " << attachedProcess->getPid() << std::endl;
        }
    }
}

const ConsoleManager::ConsoleTable& ConsoleManager::getConsoleTable() const {
    return consoleTable;
}

void ConsoleManager::showListOfConsoles()
{
	std::cout << "\n" << "List of Consoles:" << std::endl;
	for (const auto& console : consoleTable) {
		if (console.first != MAIN_CONSOLE && console.first != MARQUEE_CONSOLE) {
			std::cout << " - " << console.first << std::endl; 
		}
	}

	std::vector<String> restrictedConsoles = { MAIN_CONSOLE, MARQUEE_CONSOLE };

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

void ConsoleManager::displayNvidiaSMI()
{
    time_t now = time(0);
    struct tm localTime;

    localtime_s(&localTime, &now);

    std::cout << std::put_time(&localTime, "%a %b %d %H:%M:%S %Y") << std::endl;

    std::cout << "+-----------------------------------------------------------------------------------------+" << std::endl;
    std::cout << "| NVIDIA-SMI 561.09                 Driver Version: 561.09         CUDA Version: 12.6     |" << std::endl;
    std::cout << "|-----------------------------------------+------------------------+----------------------|" << std::endl;
    std::cout << "| GPU  Name                  Driver-Model | Bus-Id          Disp.A | Volatile Uncorr. ECC |" << std::endl;
    std::cout << "| Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |" << std::endl;
    std::cout << "|                                         |                        |               MIG M. |" << std::endl;
    std::cout << "|=========================================+========================+======================|" << std::endl;
    std::cout << "|   0  NVIDIA GeForce GTX 1660 Ti   WDDM  |   00000000:01:00.0  On |                  N/A |" << std::endl;
    std::cout << "| N/A   64C    P0             20W /   80W |    1013MiB /   6144MiB |      0%      Default |" << std::endl;
    std::cout << "|                                         |                        |                  N/A |" << std::endl;
    std::cout << "+-----------------------------------------+------------------------+----------------------+" << std::endl << std::endl;
}

void ConsoleManager::printProcessesTable2()
{
    std::vector<String> processConsoleNames = getProcessConsoleNames();

    if (processConsoleNames.empty()) {
        std::cout << "+-----------------------------------------------------------------------------------------+\n";
        std::cout << "|                                                                                         |\n";
        std::cout << "|                                 No processes available.                                 |\n";
        std::cout << "+-----------------------------------------------------------------------------------------+\n";
        return; 
    }

    std::cout << "+-----------------------------------------------------------------------------------------+\n";
    std::cout << "| Processes:                                                                              |\n";
    std::cout << "|  GPU   GI   CI        PID   Type   Process name                              GPU Memory |\n";
    std::cout << "|        ID   ID                                                               Usage      |\n";
    std::cout << "|=========================================================================================|\n";

    const int defaultGPU = 0;
    const String defaultGI = "N/A";
    const String defaultCI = "N/A";
    const String defaultType = "C+G";
    const String defaultMemoryUsage = "N/A";

    for (const auto& name : processConsoleNames) {
        auto processConsole = getProcessConsole(name);
        if (processConsole && processConsole->getAttachedProcess()) {
            const auto& attachedProcess = processConsole->getAttachedProcess();

            int pid = attachedProcess->getPid();
            String processName = attachedProcess->getProcessName();

            String truncated_name = truncateTextWithEllipsis(processName, 38);
            std::cout << "|  "
                << std::setw(3) << defaultGPU << "   "
                << std::setw(3) << defaultGI << "  "
                << std::setw(3) << defaultGI << "   "
                << std::setw(7) << pid << "   "
                << std::setw(4) << defaultType << "   "
                << std::setw(38) << std::left << truncated_name << std::right
                << std::setw(9) << defaultMemoryUsage << std::setw(8) << " |\n";
        }
    }

    std::cout << "+-----------------------------------------------------------------------------------------+\n";
}

std::string ConsoleManager::truncateTextWithEllipsis(const std::string& text, size_t max_width)
{
    if (text.length() > max_width) {
        return text.substr(0, max_width - 3) + "...";
    }
    return text;
}

bool ConsoleManager::getIsFunctionalityInitialized()
{
    return isFunctionalityInitialized;
}

void ConsoleManager::setIsFunctionalityInitialized(bool in)
{
    this->isFunctionalityInitialized = in;
}

void ConsoleManager::setupScheduler(int nCores, std::string currentSchedulingAlgo, int quantumCycles, int batchProcessFreq, int minIns, int maxIns, int delaysPerExecution)
{
    
    cpuScheduler = std::make_shared<CPUScheduler>(nCores, currentSchedulingAlgo, quantumCycles, batchProcessFreq, minIns, maxIns, delaysPerExecution);
}

std::shared_ptr<CPUScheduler> ConsoleManager::getCPUScheduler()
{
    return this->cpuScheduler;
}

int ConsoleManager::getMadePid()
{
    return madePid;
}

void ConsoleManager::addOneToMadePid()
{
    this->madePid++;
}

void ConsoleManager::addToConsoleTable(std::string screenName, std::shared_ptr<ProcessConsole> newConsole)
{
    this->consoleTable[screenName] = newConsole;
}









