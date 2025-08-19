#pragma once
#include "ICommand.h"
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream> 

class PrintCommand : public ICommand
{
public:
	PrintCommand() {
		commandType = CommandType::PRINT;
	}
	void execute() override;
private:
};

