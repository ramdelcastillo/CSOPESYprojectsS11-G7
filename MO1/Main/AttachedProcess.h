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

    AttachedProcess(int pid, std::string processName);

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
    void setAssignedCPUID(int cpuId);
    int getAssignedCPUID();
    void setFinishedTime(std::time_t endTime);

private:
    std::vector<std::shared_ptr<ICommand>> commandList;
    std::string processName;
    std::atomic<int> assignedCPUID{ -1 };
    int pid;
    int currentLineOfInstruction = 0;
    std::time_t startOrEndTime;
    ProcessState processState;
    std::mutex stateMutex;
};



