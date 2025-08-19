#include "AttachedProcess.h"
#include "ConsoleManager.h"
#include <iostream>

AttachedProcess::AttachedProcess(int pid, std::string processName, uint32_t memoryFrameSize, uint32_t minMemorySize, uint32_t maxMemorySize) : pid(pid), processName(processName), currentLineOfInstruction(0), processState(READY), startOrEndTime(std::time(nullptr)), memoryFrameSize(memoryFrameSize), minMemorySize(minMemorySize), maxMemorySize(maxMemorySize) {
    this->createListOfCommands();
    this->setRandomMemorySize(minMemorySize, maxMemorySize);

    //nPages =  rand(min,max)^2 / memoryFrameSize
    nPages = memorySize / memoryFrameSize;
}

AttachedProcess::AttachedProcess(int pid, std::string processName, uint32_t memoryFrameSize, uint32_t memorySize, int linesOfCode, int currentLineOfInstruction, std::time_t startTime, int assignedCPUID) : pid(pid), processName(processName), memoryFrameSize(memoryFrameSize), memorySize(memorySize), linesOfCode(linesOfCode), currentLineOfInstruction(currentLineOfInstruction), startOrEndTime(startTime), assignedCPUID(assignedCPUID)
{
}

std::string AttachedProcess::getProcessName()
{
    return processName;
}

void AttachedProcess::setAssignedCPUID(int cpuId)
{
    assignedCPUID.store(cpuId);
}

int AttachedProcess::getAssignedCPUID()
{
    return assignedCPUID.load();
}

void AttachedProcess::setFinishedTime(std::time_t endTime)
{
    this->startOrEndTime = endTime;
}

void AttachedProcess::setRandomMemorySize(uint32_t minMemorySize, uint32_t maxMemorySize)
{
    if (minMemorySize == maxMemorySize) {
        memorySize = minMemorySize;
        return;
    }

    std::vector<uint32_t> powersOfTwo;
    for (uint32_t size = minMemorySize; size <= maxMemorySize; size <<= 1) {
        powersOfTwo.push_back(size);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(powersOfTwo.size()) - 1);

    memorySize = powersOfTwo[dist(gen)];
}

uint32_t AttachedProcess::getMemorySize()
{
    return memorySize;
}

uint32_t AttachedProcess::getMinMemorySize()
{
    return minMemorySize;
}

uint32_t AttachedProcess::getMaxMemorySize()
{
    return maxMemorySize;
}

uint32_t AttachedProcess::getMemoryFrameSize()
{
    return memoryFrameSize;
}

uint32_t AttachedProcess::getNPages()
{
    return nPages;
}

int AttachedProcess::getPid() const {
    return pid;
}

int AttachedProcess::getLinesOfCode()
{
    return linesOfCode;
}

int AttachedProcess::getCurrentLineOfInstruction()
{
    return currentLineOfInstruction;
}

AttachedProcess::ProcessState AttachedProcess::getProcessState()
{
    //std::lock_guard<std::mutex> lock(stateMutex);
    return processState;
}

void AttachedProcess::setProcessState(std::string state)
{
    //std::lock_guard<std::mutex> lock(stateMutex);
    if (state == "WAITING") {
        processState = WAITING;
    }
    else if (state == "READY") {
        processState = READY;
    }
    else if (state == "RUNNING") {
        processState = RUNNING;
    }
    else if (state == "FINISHED") {
        processState = FINISHED;
    }
}

void AttachedProcess::executeCurrentCommand()
{
    if (currentLineOfInstruction < linesOfCode) {
        commandList[currentLineOfInstruction]->execute();
    }
    else {
        std::cout << "Error: currentLineOfInstruction exceeds commandList size." << std::endl;
    }
}

void AttachedProcess::moveToNextLine()
{ 
    if (currentLineOfInstruction < linesOfCode) {
        this->currentLineOfInstruction++;
    }
}

bool AttachedProcess::isFinished()
{
    return currentLineOfInstruction == linesOfCode;
}

bool AttachedProcess::isStateFinished()
{
    return processState == FINISHED;
}

void AttachedProcess::createListOfCommands()
{
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    int minIns = consoleManager->getConfigParser()->getMinIns();
    int maxIns = consoleManager->getConfigParser()->getMaxIns();
    int randomSize = consoleManager->getConfigParser()->getRandomCommandListSize(minIns, maxIns);
    linesOfCode = randomSize;
    for (int i = 1; i <= randomSize; ++i) {
        std::shared_ptr<ICommand> printCommand = std::make_shared<PrintCommand>();
        this->addCommand(printCommand);
    }
}

void AttachedProcess::createRemainingListOfCommandsFromBS(int linesOfCode, int currentLineOfInstruction)
{
    int remainingLines = linesOfCode - currentLineOfInstruction;

    if (remainingLines > 0) {
        for (int i = 0; i < remainingLines; ++i) {
            std::shared_ptr<ICommand> printCommand = std::make_shared<PrintCommand>();
            this->addCommand(printCommand);
        }
    }
}

std::time_t AttachedProcess::getStartTime() const {
    return startOrEndTime;
}

void AttachedProcess::addCommand(std::shared_ptr<ICommand> command)
{
     commandList.push_back(command);
}



