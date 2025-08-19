#pragma once
#include "AConsole.h"
#include "AttachedProcess.h"  
#include <memory>
#include <string>

class ProcessConsole : public AConsole {
public:
    ProcessConsole(std::shared_ptr<AttachedProcess> attachedProcess);

    void onEnabled() override;
    void display() override;
    void process() override;
    bool running();
    std::shared_ptr<AttachedProcess> getAttachedProcess();
private:
    std::shared_ptr<AttachedProcess> attachedProcess;
    void displayHeader();
    bool isRunning = false;

    friend class ConsoleManager;
};




