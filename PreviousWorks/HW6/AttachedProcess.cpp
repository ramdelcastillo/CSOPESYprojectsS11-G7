#include "AttachedProcess.h"
#include <iostream>

AttachedProcess::AttachedProcess()
    : processName("Unnamed"), pid(0), currentLineOfInstruction(0), startTime(std::time(nullptr)) {
}

AttachedProcess::AttachedProcess(const std::string& name, int id, int totalInstructions)
    : processName(name), pid(id), currentLineOfInstruction(0), startTime(std::time(nullptr)) {
}

AttachedProcess::AttachedProcess(int pid, std::string processName) : pid(pid), processName(processName), currentLineOfInstruction(0), processState(READY), startTime(std::time(nullptr)) {
    this->create100PrintCommands();
}

std::string AttachedProcess::getProcessName()
{
    return processName;
}

void AttachedProcess::setAssignedCPUID(int cpuId)
{
    this->assignedCPUID = cpuId;
}

int AttachedProcess::getAssignedCPUID()
{
    return this->assignedCPUID;
}

void AttachedProcess::setFinishedTime(std::time_t endTime)
{
    this->startTime = endTime;
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
    return processState;
}

void AttachedProcess::setProcessState(std::string state)
{
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
    else {
        throw std::invalid_argument("Invalid process state: " + state);
    }
}

void AttachedProcess::executeCurrentCommand(int cpuId, std::string processName)
{
    if (currentLineOfInstruction < commandList.size()) {
        commandList[currentLineOfInstruction]->execute(cpuId, processName, this->getStartTime());
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

void AttachedProcess::create100PrintCommands()
{
    for (int i = 1; i <= 100; ++i) {
        std::string message = "Hello world from " + getProcessName() + "!";

        std::shared_ptr<ICommand> printCommand = std::make_shared<PrintCommand>(this->getPid(), message);

        this->addCommand(printCommand);
    }
}

std::time_t AttachedProcess::getStartTime() const {
    return startTime;
}

void AttachedProcess::addCommand(std::shared_ptr<ICommand> command)
{
     commandList.push_back(command);
}



