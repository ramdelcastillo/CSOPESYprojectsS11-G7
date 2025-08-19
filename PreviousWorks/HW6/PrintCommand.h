#pragma once
#include "ICommand.h"
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream> 

class PrintCommand : public ICommand
{
public:
	PrintCommand(int pid, std::string& toPrint);
	void execute(int cpuId, std::string processName, std::time_t endTime) override;
private:
	std::string toPrint;
};

