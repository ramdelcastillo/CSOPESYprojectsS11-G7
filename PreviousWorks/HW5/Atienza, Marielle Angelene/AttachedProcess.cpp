#include "AttachedProcess.h"
#include <iostream>

AttachedProcess::AttachedProcess()
    : processName("Unnamed"), id(0), currentInstruction(0), linesOfCode(0), startTime(std::time(nullptr)) {
}

AttachedProcess::AttachedProcess(const std::string& name, int id, int totalInstructions)
    : processName(name), id(id), currentInstruction(0), linesOfCode(totalInstructions), startTime(std::time(nullptr)) {
}

std::string AttachedProcess::getName() const {
    return processName;
}

int AttachedProcess::getID() const {
    return id;
}

int AttachedProcess::getCurrentInstruction() const {
    return currentInstruction;
}

std::time_t AttachedProcess::getStartTime() const {
    return startTime;
}
