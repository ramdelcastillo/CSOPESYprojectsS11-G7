#include "ConsoleManager.h"
#include "AConsole.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "MarqueeConsole.h"
#include "CPUWorker.h"
#include "CPUScheduler.h"
#include "FlatMemoryAllocator.h" 

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
}

void ConsoleManager::createProcessConsole(String screenName)
{
    if (screenName == "MAIN_CONSOLE" || screenName == "MARQUEE_CONSOLE") {
        std::cerr << "Creating consoles are only allowed for processes. " << "\n\n";
        return;
    }

	if (this->consoleTable.find(screenName) != this->consoleTable.end()) {
		std::cerr << "Screen name " << screenName << " already exists. Please use a different name.\n" << std::endl;
		return;
	}

    this->madePid++;


    std::shared_ptr<AttachedProcess> process = std::make_shared<AttachedProcess>(this->madePid, screenName, getConfigParser()->getMemoryPerFrame(), getConfigParser()->getMemoryPerProcess());
    
	std::shared_ptr<ProcessConsole> newConsole = std::make_shared<ProcessConsole>(process);

    getCPUScheduler()->addProcessToQueue(process);

	this->consoleTable[screenName] = newConsole;

	std::cout << "Console " << screenName << " created successfully." << std::endl;

	switchToProcessConsole(screenName);
}

void ConsoleManager::createConsoleWOswitch(String screenName)
{
    if (this->consoleTable.find(screenName) != this->consoleTable.end()) {
        std::cerr << "Screen name " << screenName << " already exists. Please use a different name.\n" << std::endl;
        return;
    }

    this->madePid++;

    std::shared_ptr<AttachedProcess> process = std::make_shared<AttachedProcess>(this->madePid, screenName, getConfigParser()->getMemoryPerFrame(), getConfigParser()->getMemoryPerProcess());

    std::shared_ptr<ProcessConsole> newConsole = std::make_shared<ProcessConsole>(process);

    this->consoleTable[screenName] = newConsole;

    getCPUScheduler()->addProcessToQueue(process);

}

void ConsoleManager::switchToProcessConsole(String screenName) 
{ 
    if (screenName == "MAIN_CONSOLE" || screenName == "MARQUEE_CONSOLE") {
        std::cerr << "Switching consoles are only allowed for processes. " << "\n\n";
        return;
    }

    auto processConsole = getProcessConsole(screenName);

    if (processConsole) {
        const auto& attachedProcess = processConsole->getAttachedProcess();

        if (attachedProcess->getProcessState() == AttachedProcess::FINISHED) {
            std::cerr << "Screen name " << screenName << " not found. Was it initialized?\n" << std::endl;
        }
        else {
            if (this->consoleTable.contains(screenName)) {
                system("cls");
                this->previousConsole = this->currentConsole;
                this->currentConsole = this->consoleTable[screenName];
                this->currentConsole->onEnabled();
            }
            else {
                std::cerr << "Screen name " << screenName << " not found. Was it initialized?\n" << std::endl;
            }
        }
    }
    else {
        std::cerr << "Screen name " << screenName << " not found. Was it initialized?\n" << std::endl;
    }
}

void ConsoleManager::switchToMainConsoleFromProcessOrMarquee(String screenName)
{
	if (screenName == "MAIN_CONSOLE") {
        system("cls");
        this->previousConsole = this->currentConsole;  
		this->currentConsole = this->consoleTable[screenName];  
		this->currentConsole->onEnabled(); 
	}
	else {
		std::cerr << "Invalid operation. You can only switch back to MAIN_CONSOLE from a process console.\n" << std::endl;
	}
}

void ConsoleManager::switchToMarqueeConsoleFromMain(String screenName)
{
    if (screenName == "MARQUEE_CONSOLE") {
        system("cls");
        this->previousConsole = this->currentConsole;
        this->currentConsole = this->consoleTable[screenName];
        this->currentConsole->onEnabled();
    }
    else {
        std::cerr << "Invalid operation. You can only switch back to MAIN_CONSOLE from a process console.\n" << std::endl;
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

void ConsoleManager::setupScheduler(uint32_t nCores, std::string currentSchedulingAlgo, uint32_t quantumCycles, uint32_t batchProcessFreq, uint32_t minIns, uint32_t maxIns, uint32_t delaysPerExecution, uint32_t maxOverallMemory, uint32_t memoryPerFrame, uint32_t memoryPerProcess)
{
    
    cpuScheduler = std::make_shared<CPUScheduler>(nCores, currentSchedulingAlgo, quantumCycles, batchProcessFreq, minIns, maxIns, delaysPerExecution, maxOverallMemory, memoryPerFrame, memoryPerProcess);
}

void ConsoleManager::setupMemoryAllocator()
{
    this->memoryAllocator = std::make_shared<FlatMemoryAllocator>();
}

std::shared_ptr<CPUScheduler> ConsoleManager::getCPUScheduler()
{
    return this->cpuScheduler;
}

std::shared_ptr<IMemoryAllocator> ConsoleManager::getMemoryAllocator()
{
    return this->memoryAllocator;
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

std::shared_ptr<ConfigParser> ConsoleManager::getConfigParser()
{
    return this->configParser;
}

void ConsoleManager::setupConfig()
{
    configParser = std::make_shared<ConfigParser>("config.txt");
}










