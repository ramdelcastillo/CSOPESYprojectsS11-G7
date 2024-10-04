#pragma once
#include <string>
#include <ctime>

class AttachedProcess {
public:
    AttachedProcess();

    AttachedProcess(const std::string& name, int id, int totalInstructions);

    std::string getName() const;
    int getID() const;
    int getCurrentInstruction() const;
    std::time_t getStartTime() const;

private:
    std::string processName;
    int id;
    int currentInstruction;
    int linesOfCode;
    std::time_t startTime;
};
