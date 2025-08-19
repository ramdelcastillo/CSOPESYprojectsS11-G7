#pragma once
#include <string>
#include <iostream>

class ICommand
{
public:
	enum CommandType
	{
		PRINT,
		IO
	};

	ICommand(int pid, CommandType CommandType);
	CommandType getCommandType();
	int getPid();
	virtual void execute(int cpuId, std::string processName, std::time_t endTime) = 0;

protected:
	int pid;
	CommandType commandType;
};

inline ICommand::ICommand(int pid, CommandType commandType)
	: pid(pid), commandType(commandType) {}

inline ICommand::CommandType ICommand::getCommandType() {
	return this->commandType;
}

inline int ICommand::getPid()
{
	return this->pid;
}
