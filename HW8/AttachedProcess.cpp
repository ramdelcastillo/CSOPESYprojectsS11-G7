#include "AttachedProcess.h"
#include "ConsoleManager.h"
#include <iostream>

AttachedProcess::AttachedProcess(int pid, std::string processName, uint32_t memoryFrameSize, uint32_t memorySize) : pid(pid), processName(processName), currentLineOfInstruction(0), processState(READY), startOrEndTime(std::time(nullptr)), memoryFrameSize(memoryFrameSize), memorySize(memorySize) {
    this->createListOfCommands();
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

uint32_t AttachedProcess::getMemorySize()
{
    return memorySize;
}

uint32_t AttachedProcess::getMemoryFrameSize()
{
    return memoryFrameSize;
}

int AttachedProcess::getPid() const {
    return pid;
}

int AttachedProcess::getLinesOfCode()
{
    return (int) commandList.size();
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
    if (currentLineOfInstruction < commandList.size()) {
        commandList[currentLineOfInstruction]->execute();
    }
    else {
        std::cout << "Error: currentLineOfInstruction exceeds commandList size." << std::endl;
    }
}

void AttachedProcess::moveToNextLine()
{ 
    if (currentLineOfInstruction < commandList.size()) {
        this->currentLineOfInstruction++;
    }
}

bool AttachedProcess::isFinished()
{
    return currentLineOfInstruction == commandList.size();
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

    for (int i = 1; i <= randomSize; ++i) {
        std::shared_ptr<ICommand> printCommand = std::make_shared<PrintCommand>();
        this->addCommand(printCommand);
    }
}

std::time_t AttachedProcess::getStartTime() const {
    return startOrEndTime;
}

void AttachedProcess::addCommand(std::shared_ptr<ICommand> command)
{
     commandList.push_back(command);
}



