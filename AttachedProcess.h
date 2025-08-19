#pragma once
#include <string>
#include <ctime>
#include <vector>
#include <memory>
#include <mutex>
#include "ICommand.h"
#include "PrintCommand.h"

class AttachedProcess {
public:
    enum ProcessState
    {
        WAITING,
        READY,
        RUNNING,
        FINISHED
    };

    AttachedProcess(int pid, std::string processName, uint32_t memoryFrameSize, uint32_t minMemorySize, uint32_t maxMemorySize);
    AttachedProcess(int pid, std::string processName, uint32_t memoryFrameSize, uint32_t memorySize, int linesOfCode, int currentLineOfInstruction, std::time_t startTime, int assignedCPUID);
    std::string getProcessName();
    int getPid() const;
    int getCurrentLineOfInstruction();
    std::time_t getStartTime() const;
    void addCommand(std::shared_ptr<ICommand> command);
    int getLinesOfCode();
    ProcessState getProcessState();
    void setProcessState(std::string state);
    void executeCurrentCommand();
    void moveToNextLine();
    bool isFinished();
    bool isStateFinished();
    void createListOfCommands();
    void createRemainingListOfCommandsFromBS(int linesOfCode, int currentLineOfInstruction);
    void setAssignedCPUID(int cpuId);
    int getAssignedCPUID();
    void setFinishedTime(std::time_t endTime);
    void setRandomMemorySize(uint32_t minMemorySize, uint32_t maxMemorySize);
    uint32_t getMemorySize();
    uint32_t getMinMemorySize();
    uint32_t getMaxMemorySize();
    uint32_t getMemoryFrameSize();
    uint32_t getNPages();
private:
    std::vector<std::shared_ptr<ICommand>> commandList;
    std::string processName;
    std::atomic<int> assignedCPUID{ -1 };
    int pid;
    int currentLineOfInstruction = 0;
    int linesOfCode = 0;
    uint32_t memorySize;
    uint32_t minMemorySize;
    uint32_t maxMemorySize;
    uint32_t memoryFrameSize;
    uint32_t nPages; 
    std::time_t startOrEndTime;
    ProcessState processState;
    std::mutex stateMutex;
};



