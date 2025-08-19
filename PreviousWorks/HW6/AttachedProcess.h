#pragma once
#include <string>
#include <ctime>
#include <vector>
#include <memory>
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
    AttachedProcess();

    AttachedProcess(const std::string& name, int id, int totalInstructions);
    AttachedProcess(int pid, std::string processName);

    std::string getProcessName();
    int getPid() const;
    int getCurrentLineOfInstruction();
    std::time_t getStartTime() const;

    void addCommand(std::shared_ptr<ICommand> command);
    int getLinesOfCode();
    ProcessState getProcessState();
    void setProcessState(std::string state);
    void executeCurrentCommand(int cpuId, std::string processName);
    void moveToNextLine();
    bool isFinished();
    bool isStateFinished();
    void create100PrintCommands();
    void setAssignedCPUID(int cpuId);
    int getAssignedCPUID();
    void setFinishedTime(std::time_t endTime);

private:
    std::vector<std::shared_ptr<ICommand>> commandList;
    std::string processName;
    int assignedCPUID = -1;
    int pid;
    int currentLineOfInstruction = 0;
    std::time_t startTime;
    ProcessState processState;
};



